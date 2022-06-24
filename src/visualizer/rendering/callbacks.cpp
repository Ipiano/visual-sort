#include "callbacks.h"

#include "constants.h"
#include "globals.h"
#include "sorting/sort_handler.h"

#include <GL/freeglut.h>

#include <cstring>
#include <iostream>

using namespace std;

namespace rendering
{
void redraw()
{
    //cout << "Draw" << endl;
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    global::sorter->draw(global::screen_width, global::screen_height, 0, 0);
    //Swap buffers using double buffering to avoid flickering
    glutSwapBuffers();
}

void reshape(glut::Size size)
{
    // store new window dimensions globally
    global::screen_width  = size.width;
    global::screen_height = size.height;

    // project 3-D scene onto 2-D
    glMatrixMode(GL_PROJECTION); // use an orthographic projection
    glLoadIdentity();            // initialize transformation matrix

    // make OpenGL coordinates
    gluOrtho2D(0.0, global::screen_width, 0.0, global::screen_height);

    // the same as the screen coordinates
    glViewport(0, 0, global::screen_width, global::screen_height);
}

void keypress(unsigned char key, glut::Coordinate coord)
{
    static_cast<void>(coord);

    //Check if the key is specific ones, take appropriate action
    switch (key)
    {
        //Exit
    case constants::keys::ESCAPE:
        glutLeaveMainLoop();
        break;
    case constants::keys::RESET:
        global::sorter->reset(true);
    default:
        //Redraw the display
        glutPostRedisplay();
    }
}

void idle()
{
    //cin.get();
    glutPostRedisplay();

    global::sorter->reset();
    global::sorter->animate();
}
}
