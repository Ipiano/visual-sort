#pragma once

#include "rendering/render_items.h"
#include "sort_visualizer.h"

#include <algorithm>
#include <numeric>

namespace rendering
{
std::array<float, 3> getColor(SortVisualizer::Touch::type touches)
{
    const bool moved    = touches & SortVisualizer::Touch::MOVE;
    const bool compared = touches & SortVisualizer::Touch::COMPARE;

    if (moved)
    {
        return {constants::colors::HOLD[0], constants::colors::HOLD[1], constants::colors::HOLD[2]};
    }
    else if (compared)
    {
        return {constants::colors::ACCESS[0], constants::colors::ACCESS[1], constants::colors::ACCESS[2]};
    }
    else
    {
        return {constants::colors::FREE[0], constants::colors::FREE[1], constants::colors::FREE[2]};
    }
}

template <class InputIt> void render_items(InputIt begin, InputIt end, glut::Coordinate viewport_origin, glut::Size viewport_size)
{
    const auto item_count = std::distance(begin, end);

    double left = viewport_origin.x;
    double right;
    double top;
    double bottom = viewport_origin.y;

    double item_width = (static_cast<double>(viewport_size.width) / item_count);

    const int max_value = std::accumulate(begin, end, std::numeric_limits<int>::min(), [](int a, int b) { return std::max(a, b); });

    for (; begin != end; begin = std::next(begin))
    {
        right = left + item_width;
        top   = viewport_size.height * (static_cast<int>(*begin) / static_cast<double>(max_value)) + viewport_origin.y;

        const auto item_state = begin->getAndClearTouches();
        const auto item_color = getColor(item_state);

        glColor3fv(item_color.data());

        glBegin(GL_POLYGON);

        glVertex2f(left, bottom);
        glVertex2f(left, top);
        glVertex2f(right, top);
        glVertex2f(right, bottom);

        glEnd();

        left = right;
    }
    glFlush();
}
}
