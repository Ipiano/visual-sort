#include "algorithms/sorting/bubble_sort.hpp"
#include "args.h"
#include "constants.h"
#include "glut/window.h"
#include "rendering/render_items.h"
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

    std::uniform_int_distribution<int> dist(0, 100);
    std::mt19937_64 reng;
    std::vector<int> nums(100, 0);
    std::transform(nums.begin(), nums.end(), nums.begin(), [&](int) { return dist(reng); });

    visualizer.start(
        nums,
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
