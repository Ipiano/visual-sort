#pragma once

#include "color_strategy.hpp"
#include "glut/window.h"
#include "sort_visualizer.h"

#include <GL/gl.h>

#include <cstdint>

namespace rendering
{
// CRTP Base for strategies that render as a series of rectangles from
// left to right. Inheriting types only need to define the stategy for
// picking the top and bottom bounds of a bar.
template <class CrtpType> struct LeftToRightStrategyBase
{
  public:
    // Invoke to draw an item
    void operator()(std::size_t index, const SortVisualizer::Item& item) const;
    LeftToRightStrategyBase(glut::Coordinate viewport_origin, glut::Size viewport_size, int max_value, std::size_t item_count);

  protected:
    [[nodiscard]] glut::Coordinate viewportOrigin() const noexcept { return m_viewport_origin; }
    [[nodiscard]] glut::Size viewportSize() const noexcept { return m_viewport_size; };

    [[nodiscard]] int maxValue() const noexcept { return m_max_value; };
    [[nodiscard]] std::size_t itemCount() const noexcept { return m_item_count; };
    [[nodiscard]] double itemWidth() const noexcept { return m_item_width; };

  private:
    const glut::Coordinate m_viewport_origin;
    const glut::Size m_viewport_size;

    const int m_max_value;
    const std::size_t m_item_count;
    const double m_item_width;

    // Returns the bottom and top edges (respectively)
    [[nodiscard]] std::pair<int, int> getVerticalBounds(std::size_t index, const SortVisualizer::Item& item) const;
};

// Render strategy that renders items from left to right as a series of
// bars, using the bottom of the viewport as the bottom edge of each
// item and the magnitude of each value to determine to top edge of each
// item.
struct LeftToRightSlopeStrategy : public LeftToRightStrategyBase<LeftToRightSlopeStrategy>
{
    using LeftToRightStrategyBase::LeftToRightStrategyBase;
    [[nodiscard]] std::pair<int, int> getVerticalBounds(std::size_t index, const SortVisualizer::Item& item) const;
};

// Render strategy that renders items from left to right as a series of
// bars, using  the magnitude of each value to determine to top and
// bottom edges of each item such that the view is mirrored vertically
// in the viewport.
struct LeftToRightMirrorStrategy : public LeftToRightStrategyBase<LeftToRightMirrorStrategy>
{
    using LeftToRightStrategyBase::LeftToRightStrategyBase;
    [[nodiscard]] std::pair<int, int> getVerticalBounds(std::size_t index, const SortVisualizer::Item& item) const;
};

// Render strategy that renders items from left to right as a series of
// dots, using  the magnitude of each value to determine the height that
// the dot is drawn at
struct LeftToRightDotStrategy : public LeftToRightStrategyBase<LeftToRightDotStrategy>
{
    using LeftToRightStrategyBase::LeftToRightStrategyBase;
    [[nodiscard]] std::pair<int, int> getVerticalBounds(std::size_t index, const SortVisualizer::Item& item) const;
};

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
