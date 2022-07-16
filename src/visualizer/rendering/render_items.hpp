#pragma once

#include "constants.h"
#include "rendering/render_items.h"
#include "sort_visualizer.h"

#include <algorithm>
#include <iostream>
#include <numeric>

namespace rendering
{
template <class InputIt, class DrawStrategy> void renderItems(InputIt begin, InputIt end, DrawStrategy&& strategy)
{
    for (std::size_t index = 0; begin != end; ++begin, ++index)
    {
        strategy(index, *begin);
    }
    glFlush();
}

inline const std::array<float, 3>& getColor(SortVisualizer::Touch::type touches)
{
    const bool moved    = (touches & SortVisualizer::Touch::MOVE) != 0;
    const bool compared = (touches & SortVisualizer::Touch::COMPARE) != 0;

    if (moved)
    {
        return constants::colors::MOVE;
    }
    if (compared)
    {
        return constants::colors::COMPARE;
    }
    return constants::colors::FREE;
}

template <class CrtpType>
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
LeftToRightStrategyBase<CrtpType>::LeftToRightStrategyBase(glut::Coordinate viewport_origin, glut::Size viewport_size, int max_value,
                                                           std::size_t item_count)
    : m_viewport_origin(viewport_origin)
    , m_viewport_size(viewport_size)
    , m_max_value(max_value)
    , m_item_count(item_count)
    , m_item_width(static_cast<double>(viewport_size.width) / static_cast<double>(item_count))

{
}

template <class CrtpType>
void LeftToRightStrategyBase<CrtpType>::operator()(const std::size_t index, const SortVisualizer::Item& item) const
{
    const auto item_state = item.getAndClearTouches();
    const auto item_color = getColor(item_state);

    float left  = m_viewport_origin.x + m_item_width * static_cast<float>(index);
    float right = left + m_item_width;
    float bottom, top;
    std::tie(bottom, top) = getVerticalBounds(index, item);

    glColor3fv(item_color.data());

    glBegin(GL_POLYGON);

    glVertex2f(left, bottom);
    glVertex2f(left, top);
    glVertex2f(right, top);
    glVertex2f(right, bottom);

    glEnd();
}

template <class CrtpType>
std::pair<int, int> LeftToRightStrategyBase<CrtpType>::getVerticalBounds(const std::size_t index, const SortVisualizer::Item& item) const
{
    return static_cast<const CrtpType&>(*this).getVerticalBounds(index, item);
}

inline std::pair<int, int> LeftToRightSlopeStrategy::getVerticalBounds(const std::size_t index, const SortVisualizer::Item& item) const
{
    return {0, viewportSize().height / static_cast<double>(maxValue()) * static_cast<int>(item) + viewportOrigin().y};
}

inline std::pair<int, int> LeftToRightMirrorStrategy::getVerticalBounds(const std::size_t index, const SortVisualizer::Item& item) const
{
    const auto magnitude = (viewportSize().height / static_cast<double>(maxValue()) * static_cast<int>(item)) / 2.0;
    const auto center    = viewportOrigin().y + viewportSize().height / 2.0;
    return {center - magnitude, center + magnitude};
}

inline std::pair<int, int> LeftToRightDotStrategy::getVerticalBounds(const std::size_t index, const SortVisualizer::Item& item) const
{
    const auto magnitude = itemWidth() / 2.0;
    const auto center    = viewportSize().height / static_cast<double>(maxValue()) * static_cast<int>(item) + viewportOrigin().y;
    return {center - magnitude, center + magnitude};
}

}
