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

    std::unique_ptr<sorting::visual_sort> sort = std::make_unique<sorting::merge_sort>();

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
                    sort = std::make_unique<sorting::radix_sort>();
                if (ident == "b")
                    sort = std::make_unique<sorting::bubble_sort>();
                if (ident == "bb")
                    sort = std::make_unique<sorting::bogo_sort>();
                if (ident == "q")
                    sort = std::make_unique<sorting::quick_sort>();
                if (ident == "m")
                    sort = std::make_unique<sorting::merge_sort>();
                if (ident == "h")
                    sort = std::make_unique<sorting::heap_sort>();
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

    global::sorter->reset(*sort, sortNum, max, modNum, uniques, false, visualization);

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
