#include "args.h"
#include "constants.h"
#include "globals.h"
#include "glut/window.h"
#include "rendering/callbacks.h"
#include "sorting/sort_handler.h"
#include "sorting/sorts.h"

#include <GL/freeglut.h>

#include <ctime>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

    const auto args = parse_args(argc, argv);

    srand(time(NULL));

    global::sorter = std::make_unique<sorting::sort_handle>();

    global::screen_width  = 500;
    global::screen_height = 500;

    glut::Window win("Visual Sort", glut::display_mode::double_buffered | glut::display_mode::rgba, {-1, -1},
                     {global::screen_width, global::screen_height});

    glClearColor(0.0, 0.0, 0.0, 0.0); // use black for glClear command

    win.setDisplayCallback(&rendering::redraw);
    win.setReshapeCallback(&rendering::reshape);
    win.setKeyPressCallback(&rendering::keypress);
    win.setIdleCallback(&rendering::idle);

    global::sorter->reset(*args.sort_algorithm, args.set_size, args.set_size, args.steps_per_draw, args.unique_values, false,
                          static_cast<int>(args.draw_mode));

    glutMainLoop();

    global::sorter.reset();

    return 0;
}
