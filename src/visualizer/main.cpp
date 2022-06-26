#include "algorithms/sorting/bubble_sort.hpp"
#include "args.h"
#include "constants.h"
#include "glut/window.h"
#include "rendering/render_items.h"
#include "rendering/utils.h"
#include "sort_visualizer.h"

#include <GL/freeglut.h>

#include <iostream>
#include <random>
#include <string>

using namespace std;

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

    const auto args = parse_args(argc, argv);

    int screen_width  = 500;
    int screen_height = 500;

    glut::Window win("Visual Sort", glut::display_mode::double_buffered | glut::display_mode::rgba, {-1, -1},
                     {screen_width, screen_height});

    glClearColor(0.0, 0.0, 0.0, 0.0); // use black for glClear command

    SortVisualizer visualizer;

    win.setDisplayCallback(
        [&]
        {
            //cout << "Draw" << endl;
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();

            visualizer.draw();

            const auto text_left   = 0;
            const auto text_top    = screen_height;
            const auto text_height = 15;

            const auto reads  = visualizer.totalCompares();
            const auto writes = visualizer.totalMoves();

            //drawText("Cycles: " + to_string(_cycles), text_left, text_top - text_height);
            rendering::renderText("Array Reads: " + to_string(reads), text_left, text_top - text_height * 2);
            rendering::renderText("Array Writes: " + to_string(writes), text_left, text_top - text_height * 3);
            rendering::renderText("Total Array Operations: " + to_string(reads + writes), text_left, text_top - text_height * 4);

            //Swap buffers using double buffering to avoid flickering
            glutSwapBuffers();
        });

    win.setReshapeCallback(
        [&](glut::Size size)
        {
            screen_width  = size.width;
            screen_height = size.height;

            // project 3-D scene onto 2-D
            glMatrixMode(GL_PROJECTION); // use an orthographic projection
            glLoadIdentity();            // initialize transformation matrix

            // make OpenGL coordinates
            gluOrtho2D(0.0, screen_width, 0.0, screen_height);

            // the same as the screen coordinates
            glViewport(0, 0, screen_width, screen_height);
        });

    win.setKeyPressCallback([](unsigned char key, glut::Coordinate coord)
                            { std::cout << "Key " << key << " @ " << coord.x << ", " << coord.y << std::endl; });

    win.setIdleCallback(
        [&]
        {
            if (visualizer.readyToDraw())
            {
                glutPostRedisplay();
            }
        });

    auto data_set = args.data_set_factory();

    visualizer.start(
        data_set,
        [](std::vector<SortVisualizer::Item>& items, const std::atomic_bool& cancelled)
        { algorithms::sorting::bubble_sort(items.begin(), items.end()); },
        [&](const std::vector<SortVisualizer::Item>& items)
        {
            rendering::render_items(items.begin(), items.end(), {0, 0}, {screen_width, screen_height});
            std::this_thread::sleep_for(chrono::milliseconds(1));
        });

    glutMainLoop();

    return 0;
}
