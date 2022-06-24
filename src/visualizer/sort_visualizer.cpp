#include "sort_visualizer.h"

#include <algorithm>
#include <cassert>
#include <utility>

using Item = SortVisualizer::Item;

Item::Item(Item&& other) : m_value(other.m_value), m_visual(other.m_visual), m_touches(SortVisualizer::Touch::MOVE)
{
    other.addTouch(Touch::MOVE);
    m_visual.onMove();
}

Item& Item::operator=(Item&& other)
{
    std::swap(m_value, other.m_value);
    std::swap(m_touches, other.m_touches);

    addTouch(Touch::MOVE);
    other.addTouch(Touch::MOVE);

    // During any run of the sort, it should not be possible for any
    // item to get created except by the constructor where we set the
    // visualizer or the move constructor where that gets propagated.
    assert(&m_visual == &other.m_visual);

    return *this;
}

bool Item::operator<(const Item& other) const
{
    addTouch(Touch::COMPARE);
    other.addTouch(Touch::COMPARE);

    m_visual.onCompare();
    return m_value < other.m_value;
}

SortVisualizer::Touch::type Item::getAndClearTouches() const
{
    const auto result = m_touches;
    m_touches         = Touch::NONE;
    return result;
}

SortVisualizer::~SortVisualizer()
{
    cancel();
}

void SortVisualizer::start(const std::vector<int>& values, sort_function sort_fn, draw_function draw_fn)
{
    // Should never get called during an active sort
    if (m_thread.joinable())
    {
        assert(m_complete_flag);
        m_thread.join();
    }

    // Set cancelled while we initialize the items to prevent trying to
    // wait for a draw from within this function
    m_cancel_flag = true;
    m_draw_fn     = std::move(draw_fn);

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
        [this, sort_fn]()
        {
            sort_fn(m_items, m_cancel_flag);
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

bool SortVisualizer::readyToDraw()
{
    return !m_cancel_flag && m_ready_draw;
}

void SortVisualizer::draw()
{
    // If the thread is waiting to draw, draw the screen and then
    // wake up that thread
    if (m_ready_draw)
    {
        m_draw_fn(m_items);

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

    return m_moves_since_draw > 10;
}

void SortVisualizer::waitForDraw()
{
    std::unique_lock<std::mutex> lck(m_ready_draw_mutex);
    m_ready_draw = true;
    m_wait_draw.wait(lck, [this] { return !m_ready_draw || m_cancel_flag; });
    m_moves_since_draw    = 0;
    m_compares_since_draw = 0;
}
