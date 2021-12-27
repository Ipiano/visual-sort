#include "args.h"
#include "constants.h"
#include "globals.h"
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

void init_Program();
void denit_Program();
void initOpenGL();

int main(int argc, char* argv[])
{
    const auto args = parse_args(argc, argv);

    srand(time(NULL));

    init_Program();

    glutInit(&argc, argv);
    initOpenGL();

    global::sorter->reset(*args.sort_algorithm, args.set_size, args.set_size, args.steps_per_draw, args.unique_values, false,
                          static_cast<int>(args.draw_mode));

    glutMainLoop();

    denit_Program();

    return 0;
}

void init_Program()
{
    global::sorter = std::make_unique<sorting::sort_handle>();
}

void denit_Program()
{
    global::sorter.reset();
}

void initOpenGL()
{
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // 32-bit graphics and double buffering

    global::screen_width  = 500;
    global::screen_height = 500;

    glutInitWindowSize(global::screen_width, global::screen_height); // initial window size
    glutInitWindowPosition(100, 50);                                 // initial window  position
    global::window = glutCreateWindow("Visual Sort");                // window title

    glClearColor(0.0, 0.0, 0.0, 0.0); // use black for glClear command

    // callback routines
    glutDisplayFunc(rendering::redraw);  // how to redisplay window
    glutReshapeFunc(rendering::reshape); // how to resize window
    glutKeyboardFunc(rendering::keypress);
    glutIdleFunc(rendering::idle);
}
