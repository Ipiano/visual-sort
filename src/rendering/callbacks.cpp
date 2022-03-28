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
void redraw(void)
{
    //cout << "Draw" << endl;
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    global::sorter->draw(global::screen_width, global::screen_height, 0, 0);
    //Swap buffers using double buffering to avoid flickering
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    // store new window dimensions globally
    global::screen_width  = w;
    global::screen_height = h;

    // project 3-D scene onto 2-D
    glMatrixMode(GL_PROJECTION); // use an orthographic projection
    glLoadIdentity();            // initialize transformation matrix
    gluOrtho2D(0.0, w, 0.0, h);  // make OpenGL coordinates
    glViewport(0, 0, w, h);      // the same as the screen coordinates
}

void keypress(unsigned char key, int x, int y)
{
    //Check if the key is specific ones, take appropriate action
    switch (key)
    {
        //Exit
    case ESCAPE:
        glutLeaveMainLoop();
        break;
    case 'r':
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
