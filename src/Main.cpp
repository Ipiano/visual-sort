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
    int sortNum = 2500;
    int modNum = 50;
    if (argc != 3 && argc != 2)
        cout << "Usage: visusort [num items] <modifications per draw>" << endl;
    if (argc > 3)
    {
        return 1;
    }
    else if (argc == 2)
    {
        cout << "No modification number given; defaulting to " << modNum << endl;
        sortNum = stoi(string(argv[1]));
    }
    else if (argc == 1)
    {
        cout << "No number of items given; defaulting to " << sortNum << endl;
        cout << "No modification number given; defaulting to " << modNum << endl;
    }
    else
    {
        sortNum = stoi(string(argv[1]));
        modNum = stoi(string(argv[2]));
    }

    srand(time(NULL));

    init_Program();

    glutInit( &argc, argv );
    initOpenGL();


    visual_sort* sort = new bogo_sort();
    sorter -> reset(sort, sortNum, MAXVAL, modNum, false);

    glutMainLoop();

    denit_Program();

    delete sort;
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
