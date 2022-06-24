#pragma once

#include "glut/window.h"

namespace rendering
{
// begin/end - items to draw
//
// origin/size - upper-left corner of viewport, and size
template <class InputIt> void render_items(InputIt begin, InputIt end, glut::Coordinate viewport_origin, glut::Size viewport_size);
}

#include "rendering/render_items.hpp"
