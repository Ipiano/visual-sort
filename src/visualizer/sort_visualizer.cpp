#include "sort_visualizer.h"

#include <algorithm>
#include <cassert>
#include <numeric>
#include <utility>

using Item = SortVisualizer::Item;

Item::Item(Item&& other) noexcept : m_value(other.m_value), m_visual(other.m_visual), m_touches(SortVisualizer::Touch::MOVE)
{
    other.addTouch(Touch::MOVE);
    addMove();
}

Item& Item::operator=(Item&& other) noexcept
{
    m_value   = other.m_value;
    m_touches = other.m_touches;

    other.addTouch(Touch::MOVE);
    addMove();

    // During any run of the sort, it should not be possible for any
    // item to get created except by the constructor where we set the
    // visualizer or the move constructor where that gets propagated.
    assert(&m_visual == &other.m_visual);

    return *this;
}

bool Item::operator<(const Item& other) const
{
    other.addTouch(Touch::COMPARE);
    addCompare();

    return m_value < other.m_value;
}

SortVisualizer::Touch::type Item::getAndClearTouches() const
{
    const auto result = m_touches;
    m_touches         = Touch::NONE | (Touch::COMPLETE & result); // Completed state is sticky
    return result;
}

SortVisualizer::SortVisualizer(std::size_t moves_per_draw) : m_moves_per_draw(moves_per_draw)
{
}

SortVisualizer::~SortVisualizer()
{
    cancel();
}

void SortVisualizer::start(const std::vector<int>& values, sort_function sort_fn, draw_function draw_fn)
{
    // If a different sort was running, kill it and wait for it to finish
    cancel();

    // Set cancelled while we initialize the items to prevent trying to
    // wait for a draw from within this function
    m_cancel_flag = true;
    m_draw_fn     = std::move(draw_fn);

    m_max_value = std::accumulate(values.begin(), values.end(), 0, [](int l, int r) { return std::max(l, r); });

    m_items.clear();
    m_items.reserve(values.size());
    std::transform(values.begin(), values.end(), std::back_inserter(m_items), [this](int x) { return Item(*this, x); });

    m_moves_since_draw = 0;
    m_total_moves      = 0;

    m_compares_since_draw = 0;
    m_total_compares      = 0;

    m_cancel_flag   = false;
    m_complete_flag = false;
    m_ready_draw    = false;

    m_thread = std::thread(
        [this, sort_fn = std::move(sort_fn)]()
        {
            sort_fn(m_items, m_cancel_flag);

            // Paint the whole thing green
            for (auto& x : m_items)
            {
                x.complete();
            }

            // Force a few redraws, just get the screen all nice and clean
            // for the result. Experimentally determined that three is needed...
            // really not sure why
            waitForDraw();
            waitForDraw();
            waitForDraw();

            m_complete_flag = true;
        });
}

void SortVisualizer::cancel()
{
    if (m_thread.joinable())
    {
        // Cancel the sort, wake up the sort thread if it happens to be
        // blocked waiting for a draw
        {
            std::unique_lock<std::mutex> lck(m_ready_draw_mutex);
            m_cancel_flag = true;
            m_wait_draw.notify_one();
        }
        m_thread.join();
    }
}

bool SortVisualizer::readyToDraw() const
{
    return !m_cancel_flag && m_ready_draw;
}

bool SortVisualizer::sortCompleted() const
{
    return !m_cancel_flag && m_complete_flag;
}

void SortVisualizer::draw(glut::Coordinate viewport_origin, glut::Size viewport_size)
{
    // If the thread is waiting to draw, draw the screen and then
    // wake up that thread
    if (m_ready_draw)
    {
        m_draw_fn(m_items, m_max_value, viewport_origin, viewport_size);

        std::unique_lock<std::mutex> lck(m_ready_draw_mutex);
        m_ready_draw = false;
        m_wait_draw.notify_one();
    }
}

void SortVisualizer::onCompare()
{
    m_compares_since_draw++;
    m_total_compares++;
    checkDraw();
}

void SortVisualizer::onMove()
{
    m_moves_since_draw++;
    m_total_moves++;
    checkDraw();
}

void SortVisualizer::onComplete()
{
    m_moves_since_draw++;
    checkDraw();
}

void SortVisualizer::checkDraw()
{
    if (needDraw())
    {
        waitForDraw();
    }
}

bool SortVisualizer::needDraw()
{
    // Never draw if cancelled
    if (m_cancel_flag)
    {
        return false;
    }

    return m_moves_since_draw > m_moves_per_draw;
}

void SortVisualizer::waitForDraw()
{
    std::unique_lock<std::mutex> lck(m_ready_draw_mutex);
    m_ready_draw = true;
    m_wait_draw.wait(lck, [this] { return !m_ready_draw || m_cancel_flag; });
    m_moves_since_draw    = 0;
    m_compares_since_draw = 0;
}
