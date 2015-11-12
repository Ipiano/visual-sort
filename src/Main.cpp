#include <sstream>
#include <iostream>
#include <random>
#include <ctime>
#include <GL/freeglut.h>
#include "Constants.h"
#include "Globals.h"
#include "Callbacks.h"
#include "SortHandler.h"
#include "Sorts.h"

using namespace std;

void init_Program();
void denit_Program();
void initOpenGL();

int main(int argc, char* argv[])
{
    int sortNum = 500;
    if (argc != 2)
    {
        cout << "Usage: visusort [num items]" << endl;
        if (argc > 2)
            return 1;
        cout << "No number given; defaulting to 1000";
    }
    else
    {
        sortNum = stoi(string(argv[1]));
    }

    srand(time(NULL));

    init_Program();

    glutInit( &argc, argv );
    initOpenGL();


    sorter -> reset(new merge_sort(), sortNum, MAXVAL);

    glutMainLoop();

    denit_Program();

    return 0;
}

void init_Program()
{
    sorter = new sort_handle();
}

void denit_Program()
{
    delete sorter;
}

void initOpenGL( )
{
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );	        // 32-bit graphics and double buffering

    screen_Width = 500;
    screen_Height = 500;

    glutInitWindowSize( screen_Width, screen_Height );	    // initial window size
    glutInitWindowPosition( 100, 50 );			    // initial window  position
    window = glutCreateWindow( "Visual Sort" );			    // window title

    glClearColor( 0.0, 0.0, 0.0, 0.0 );			    // use black for glClear command

    // callback routines
    glutDisplayFunc( redraw );				// how to redisplay window
    glutReshapeFunc( reshape );				// how to resize window
    glutKeyboardFunc( keypress );
    glutIdleFunc( idle );
}
