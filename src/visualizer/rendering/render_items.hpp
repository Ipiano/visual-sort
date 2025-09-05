#pragma once

#include <GL/gl.h>

#include <cstddef>

namespace rendering
{
template <class InputIt, class DrawStrategy, class ColorStrategy, class ToneStrategy>
void renderItems(InputIt begin, InputIt end, DrawStrategy draw_item, ColorStrategy get_color, ToneStrategy play_tone)
{
    for (std::size_t index = 0; begin != end; ++begin, ++index)
    {
        const auto item_state = begin->getAndClearTouches();

        play_tone(static_cast<int>(*begin), item_state);

        const auto item_color = get_color(index, static_cast<int>(*begin), item_state);
        glColor3fv(item_color.data());

        draw_item(index, static_cast<int>(*begin));
    }
    glFlush();
}

}
