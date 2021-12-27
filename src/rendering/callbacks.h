#pragma once

namespace rendering
{
void redraw(void);
void reshape(int w, int h);
void keypress(unsigned char key, int x, int y);
void idle();
}
