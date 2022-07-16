#pragma once

#include "GL/gl.h"
#include "glut/window.h"
#include "sort_visualizer.h"

namespace rendering
{
// Renders all the items using a specific render strategy
template <class InputIt, class DrawStrategy> void renderItems(InputIt begin, InputIt end, DrawStrategy&& strategy);

// CRTP Base for strategies that render as a series of rectangles from
// left to right. Inheriting types only need to define the stategy for
// picking the top and bottom bounds of a bar.
template <class CrtpType> struct LeftToRightStrategyBase
{
    LeftToRightStrategyBase(glut::Coordinate viewport_origin, glut::Size viewport_size, int max_value, std::size_t item_count);
    void operator()(std::size_t index, const SortVisualizer::Item& item) const;

  protected:
    glut::Coordinate viewportOrigin() const noexcept { return m_viewport_origin; }
    glut::Size viewportSize() const noexcept { return m_viewport_size; };

    int maxValue() const noexcept { return m_max_value; };
    std::size_t itemCount() const noexcept { return m_item_count; };
    double itemWidth() const noexcept { return m_item_width; };

  private:
    const glut::Coordinate m_viewport_origin;
    const glut::Size m_viewport_size;

    const int m_max_value;
    const std::size_t m_item_count;
    const double m_item_width;

    // Returns the bottom and top edges (respectively)
    std::pair<int, int> getVerticalBounds(std::size_t index, const SortVisualizer::Item& item) const;
};

// Render strategy that renders items from left to right as a series of
// bars, using the bottom of the viewport as the bottom edge of each
// item and the magnitude of each value to determine to top edge of each
// item.
struct LeftToRightSlopeStrategy : public LeftToRightStrategyBase<LeftToRightSlopeStrategy>
{
    using LeftToRightStrategyBase::LeftToRightStrategyBase;
    std::pair<int, int> getVerticalBounds(std::size_t index, const SortVisualizer::Item& item) const;
};

// Render strategy that renders items from left to right as a series of
// bars, using  the magnitude of each value to determine to top and
// bottom edges of each item such that the view is mirrored vertically
// in the viewport.
struct LeftToRightMirrorStrategy : public LeftToRightStrategyBase<LeftToRightMirrorStrategy>
{
    using LeftToRightStrategyBase::LeftToRightStrategyBase;
    std::pair<int, int> getVerticalBounds(std::size_t index, const SortVisualizer::Item& item) const;
};

// Render strategy that renders items from left to right as a series of
// dots, using  the magnitude of each value to determine the height that
// the dot is drawn at
struct LeftToRightDotStrategy : public LeftToRightStrategyBase<LeftToRightDotStrategy>
{
    using LeftToRightStrategyBase::LeftToRightStrategyBase;
    std::pair<int, int> getVerticalBounds(std::size_t index, const SortVisualizer::Item& item) const;
};
}

#include "rendering/render_items.hpp"
