#include "Callbacks.h"
#include "Constants.h"
#include "Globals.h"
#include "SortHandler.h"
#include "Sorts.h"

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

void usage()
{
    cout << "Usage: visusort <num items> <modifications per draw> <range> <sort identifier|-r/-b/-bb/-q/-m> <unique values|--t/--f>, "
            "<visualization|--v[n]>"
         << endl;
}

int main(int argc, char* argv[])
{
    int max           = 500;
    int sortNum       = 100;
    int modNum        = 5;
    int visualization = 0;
    bool uniques      = true;
    visual_sort* sort = new merge_sort();
    if (argc > 7)
    {
        usage();
        return 1;
    }

    for (int i = 1, j = 0; i < argc; i++)
    {
        if (string(argv[i]) == "help")
        {
            usage();
            return 0;
        }
        if (argv[i][0] == '-')
        {
            if (argv[i][1] == '-')
            {
                if (argv[i][2] == 'v')
                    visualization = stoi(string(argv[i]).substr(3));
                else
                    uniques = (argv[i][2] == 't');
            }
            else
            {
                string ident = string(argv[i]).substr(1);
                if (ident == "r")
                    sort = new radix_sort();
                if (ident == "b")
                    sort = new bubble_sort();
                if (ident == "bb")
                    sort = new bogo_sort();
                if (ident == "q")
                    sort = new quick_sort();
                if (ident == "m")
                    sort = new merge_sort();
                if (ident == "h")
                    sort = new heap_sort();
            }
        }
        else
        {
            int n = stoi(argv[i]);
            switch (j++)
            {
            case 0:
                sortNum = n;
                break;
            case 1:
                modNum = n;
                break;
            case 2:
                max = n;
                break;
            }
        }
    }

    srand(time(NULL));

    init_Program();

    glutInit(&argc, argv);
    initOpenGL();

    sorter->reset(sort, sortNum, max, modNum, uniques, false, visualization);

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

void initOpenGL()
{
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE); // 32-bit graphics and double buffering

    screen_Width  = 500;
    screen_Height = 500;

    glutInitWindowSize(screen_Width, screen_Height); // initial window size
    glutInitWindowPosition(100, 50);                 // initial window  position
    window = glutCreateWindow("Visual Sort");        // window title

    glClearColor(0.0, 0.0, 0.0, 0.0); // use black for glClear command

    // callback routines
    glutDisplayFunc(redraw);  // how to redisplay window
    glutReshapeFunc(reshape); // how to resize window
    glutKeyboardFunc(keypress);
    glutIdleFunc(idle);
}
