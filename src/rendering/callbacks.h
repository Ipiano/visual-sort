#pragma once

#include "glut/window.h"

namespace rendering
{
void redraw();
void reshape(glut::Size size);
void keypress(unsigned char key, glut::Coordinate coord);
void idle();
}
