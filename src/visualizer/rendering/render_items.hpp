#pragma once

#include <GL/gl.h>

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

}
