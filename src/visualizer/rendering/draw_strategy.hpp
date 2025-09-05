#pragma once

#include "color_strategy.hpp"
#include "glut/window.h"
#include "sort_visualizer.h"

#include <GL/gl.h>

#include <cstdint>

namespace rendering
{

enum class DrawStrategyChoices : std::uint8_t
{
    // Placeholder for the min value; equivalent to the first valid value
    min_value = 0,

    // Slope from bottom left to upper right
    slope_bars = min_value,

    // Slope, but it's shifted up and mirrored across
    // the X axis
    slope_mirrored = 1,

    // Slope, but only the top of each bar is drawn
    slope_points = 2,

    // Placeholder for max value
    max_value = 3
};

// Provides 'type' typedef
template <DrawStrategyChoices strategy> struct draw_strategy_traits;

// CRTP Base for strategies that render as a series of rectangles from
// left to right. Inheriting types only need to define the stategy for
// picking the top and bottom bounds of a bar.
template <class CrtpType> struct LeftToRightStrategyBase
{
  public:
    // Invoke to draw an item
    void operator()(std::size_t index, int item_value) const;

  protected:
    [[nodiscard]] glut::Coordinate viewportOrigin() const noexcept { return m_viewport_origin; }
    [[nodiscard]] glut::Size viewportSize() const noexcept { return m_viewport_size; };

    [[nodiscard]] int maxValue() const noexcept { return m_max_value; };
    [[nodiscard]] std::size_t itemCount() const noexcept { return m_item_count; };
    [[nodiscard]] double itemWidth() const noexcept { return m_item_width; };

  private:
    friend CrtpType;
    LeftToRightStrategyBase(glut::Coordinate viewport_origin, glut::Size viewport_size, int max_value, std::size_t item_count);

    glut::Coordinate m_viewport_origin;
    glut::Size m_viewport_size;

    int m_max_value;
    std::size_t m_item_count;
    double m_item_width;

    // Returns the bottom and top edges (respectively)
    [[nodiscard]] std::pair<int, int> getVerticalBounds(std::size_t index, int item_value) const;
};

// Render strategy that renders items from left to right as a series of
// bars, using the bottom of the viewport as the bottom edge of each
// item and the magnitude of each value to determine to top edge of each
// item.
struct LeftToRightSlopeStrategy : public LeftToRightStrategyBase<LeftToRightSlopeStrategy>
{
    LeftToRightSlopeStrategy(glut::Coordinate viewport_origin, glut::Size viewport_size, int max_value, std::size_t item_count)
        : LeftToRightStrategyBase(viewport_origin, viewport_size, max_value, item_count)
    {
    }

    [[nodiscard]] std::pair<int, int> getVerticalBounds(std::size_t index, int item_value) const;
};

// Render strategy that renders items from left to right as a series of
// bars, using  the magnitude of each value to determine to top and
// bottom edges of each item such that the view is mirrored vertically
// in the viewport.
struct LeftToRightMirrorStrategy : public LeftToRightStrategyBase<LeftToRightMirrorStrategy>
{
    LeftToRightMirrorStrategy(glut::Coordinate viewport_origin, glut::Size viewport_size, int max_value, std::size_t item_count)
        : LeftToRightStrategyBase(viewport_origin, viewport_size, max_value, item_count)
    {
    }

    [[nodiscard]] std::pair<int, int> getVerticalBounds(std::size_t index, int item_value) const;
};

// Render strategy that renders items from left to right as a series of
// dots, using  the magnitude of each value to determine the height that
// the dot is drawn at
struct LeftToRightDotStrategy : public LeftToRightStrategyBase<LeftToRightDotStrategy>
{
    LeftToRightDotStrategy(glut::Coordinate viewport_origin, glut::Size viewport_size, int max_value, std::size_t item_count)
        : LeftToRightStrategyBase(viewport_origin, viewport_size, max_value, item_count)
    {
    }

    [[nodiscard]] std::pair<int, int> getVerticalBounds(std::size_t index, int item_value) const;
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

template <class CrtpType> void LeftToRightStrategyBase<CrtpType>::operator()(const std::size_t index, int item_value) const
{
    float left  = m_viewport_origin.x + (m_item_width * static_cast<float>(index));
    float right = left + m_item_width;
    float bottom, top;
    std::tie(bottom, top) = getVerticalBounds(index, item_value);

    glBegin(GL_POLYGON);

    glVertex2f(left, bottom);
    glVertex2f(left, top);
    glVertex2f(right, top);
    glVertex2f(right, bottom);

    glEnd();
}

template <class CrtpType>
std::pair<int, int> LeftToRightStrategyBase<CrtpType>::getVerticalBounds(const std::size_t index, int item_value) const
{
    return static_cast<const CrtpType&>(*this).getVerticalBounds(index, item_value);
}

inline std::pair<int, int> LeftToRightSlopeStrategy::getVerticalBounds(const std::size_t index, int item_value) const
{
    return {0, ((viewportSize().height / static_cast<double>(maxValue())) * item_value) + viewportOrigin().y};
}

inline std::pair<int, int> LeftToRightMirrorStrategy::getVerticalBounds(const std::size_t index, int item_value) const
{
    const auto magnitude = (viewportSize().height / static_cast<double>(maxValue()) * item_value) / 2.0;
    const auto center    = viewportOrigin().y + (viewportSize().height / 2.0);
    return {center - magnitude, center + magnitude};
}

inline std::pair<int, int> LeftToRightDotStrategy::getVerticalBounds(const std::size_t index, int item_value) const
{
    const auto magnitude = itemWidth() / 2.0;
    const auto center    = ((viewportSize().height / static_cast<double>(maxValue())) * item_value) + viewportOrigin().y;
    return {center - magnitude, center + magnitude};
}

template <> struct draw_strategy_traits<DrawStrategyChoices::slope_bars>
{
    using type = LeftToRightSlopeStrategy;
};

template <> struct draw_strategy_traits<DrawStrategyChoices::slope_mirrored>
{
    using type = LeftToRightMirrorStrategy;
};

template <> struct draw_strategy_traits<DrawStrategyChoices::slope_points>
{
    using type = LeftToRightDotStrategy;
};

}
