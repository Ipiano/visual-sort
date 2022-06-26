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

enum class AppState
{
    NOT_SORTING,
    SORTING
};

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);

    const auto args = parse_args(argc, argv);

    glut::Size screen_size {500, 500};
    glut::Window win("Visual Sort", glut::display_mode::double_buffered | glut::display_mode::rgba, {-1, -1}, screen_size);

    glClearColor(0.0, 0.0, 0.0, 0.0); // use black for glClear command

    AppState m_state = AppState::NOT_SORTING;

    SortVisualizer visualizer(args.steps_between_draws);
    win.setDisplayCallback(
        [&]
        {
            //cout << "Draw" << endl;
            glClear(GL_COLOR_BUFFER_BIT);
            glFlush();

            if (m_state == AppState::SORTING)
            {
                visualizer.draw({0, 0}, screen_size);

                const auto text_left   = 0;
                const auto text_top    = screen_size.height;
                const auto text_height = 15;

                const auto reads  = visualizer.totalCompares();
                const auto writes = visualizer.totalMoves();

                //drawText("Cycles: " + to_string(_cycles), text_left, text_top - text_height);
                rendering::renderText("Array Reads: " + to_string(reads), text_left, text_top - text_height * 2);
                rendering::renderText("Array Writes: " + to_string(writes), text_left, text_top - text_height * 3);
                rendering::renderText("Total Array Operations: " + to_string(reads + writes), text_left, text_top - text_height * 4);
            }

            //Swap buffers using double buffering to avoid flickering
            glutSwapBuffers();
        });

    win.setReshapeCallback(
        [&](glut::Size size)
        {
            screen_size = size;

            // project 3-D scene onto 2-D
            glMatrixMode(GL_PROJECTION); // use an orthographic projection
            glLoadIdentity();            // initialize transformation matrix

            // make OpenGL coordinates
            gluOrtho2D(0.0, screen_size.width, 0.0, screen_size.height);

            // the same as the screen coordinates
            glViewport(0, 0, screen_size.width, screen_size.height);
        });

    win.setKeyPressCallback(
        [&](unsigned char key, glut::Coordinate coord)
        {
            std::cout << "Key " << key << " @ " << coord.x << ", " << coord.y << std::endl;

            switch (key)
            {
            case constants::keys::RESET:
                visualizer.cancel();
                m_state = AppState::NOT_SORTING;
            }
        });

    win.setIdleCallback(
        [&]
        {
            if (visualizer.readyToDraw())
            {
                glutPostRedisplay();
            }

            if (m_state == AppState::NOT_SORTING)
            {
                auto data_set = args.data_set_factory();

                visualizer.start(data_set, args.sort_function,
                                 [&](const std::vector<SortVisualizer::Item>& items, int max_value, glut::Coordinate viewport_origin,
                                     glut::Size viewport_size)
                                 {
                                     args.draw_function(items, max_value, viewport_origin, viewport_size);
                                     std::this_thread::sleep_for(chrono::milliseconds(1));
                                 });

                m_state = AppState::SORTING;
            }
        });

    glutMainLoop();

    return 0;
}
