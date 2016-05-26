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
    int sortNum = 100;
    int modNum = 5;
    bool uniques = true;
    visual_sort* sort;
    if (argc > 4)
    {
        cout << "Usage: visusort <num items> <modifications per draw> <sort identifier|-r/-b/-q/-m> <unique values|--t/--f>" << endl;
        return 1;
    }
 
    for(int i=1, j=0; i<argc; i++)
    {
        if(argv[i][0] == '-')
        {
            if(argv[i][1]=='-')
            {
                uniques = (argv[i][2]=='t');
            }
            else
            {
                char ident = argv[i][1];
                switch (ident)
                {
                    case 'r': sort = new radix_sort(); break;
                    case 'b': sort = new bogo_sort(); break;
                    case 'q': sort = new quick_sort(); break;
                    case 'm': sort = new merge_sort(); break;
                }
            }
        }
        else
        {
            int n = stoi(argv[i]);
            switch(j++)
            {
                case 0: sortNum=n;break;
                case 1: modNum=n;break;
            }
        }
    }

    srand(time(NULL));

    init_Program();

    glutInit( &argc, argv );
    initOpenGL();

    sorter -> reset(sort, sortNum, MAXVAL, modNum, uniques);

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
