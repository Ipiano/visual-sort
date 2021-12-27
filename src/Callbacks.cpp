#include "Callbacks.h"

#include "Constants.h"
#include "Globals.h"
#include "SortHandler.h"

#include <GL/freeglut.h>

#include <cstring>
#include <iostream>

using namespace std;

void redraw(void)
{
    //cout << "Draw" << endl;
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();

    sorter->draw(screen_Width, screen_Height, 0, 0);
    //Swap buffers using double buffering to avoid flickering
    glutSwapBuffers();
}

void reshape(int w, int h)
{
    // store new window dimensions globally
    screen_Width  = w;
    screen_Height = h;

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
        sorter->reset(true);
    default:
        //Redraw the display
        glutPostRedisplay();
    }
}

void idle()
{
    //cin.get();
    glutPostRedisplay();

    sorter->reset();
    sorter->animate();
}
