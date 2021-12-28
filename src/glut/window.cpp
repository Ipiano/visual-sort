#include "window.h"

#include <cassert>
#include <mutex>

namespace glut
{
decltype(Window::s_windows) Window::s_windows = {};

Window::Window(const std::string& name, display_mode_t mode, Coordinate position, Size size)
{
    glutInitDisplayMode(mode);
    glutInitWindowPosition(position.x, position.y);
    glutInitWindowSize(size.width, size.height);

    m_window_id            = glutCreateWindow(name.c_str());
    s_windows[m_window_id] = {};

    // The callbacks need to be initialized after each window created
    // because the glut*Func functions apply to the current window
    initCallbacks();
}

Window::~Window()
{
    glutDestroyWindow(m_window_id);
    s_windows.erase(m_window_id);
}

void Window::setDisplayCallback(display_callback_t&& callback)
{
    s_windows[m_window_id].onDisplay = std::move(callback);
}

void Window::setReshapeCallback(reshape_callback_t&& callback)
{
    s_windows[m_window_id].onReshape = std::move(callback);
}

void Window::setKeyPressCallback(keypress_callback_t&& callback)
{
    s_windows[m_window_id].onKey = std::move(callback);
}

void Window::setIdleCallback(idle_callback_t&& callback)
{
    s_windows[m_window_id].onIdle = std::move(callback);
}

void Window::initCallbacks()
{
    glutDisplayFunc(&Window::onDisplay);
    glutReshapeFunc(&Window::onReshape);
    glutKeyboardFunc(&Window::onKey);
    glutIdleFunc(&Window::onIdle);
}

void Window::onDisplay()
{
    onCurrentWindow(
        [](const WindowData& w)
        {
            if (w.onDisplay)
            {
                w.onDisplay();
            }
        });
}

void Window::onReshape(int width, int height)
{
    onCurrentWindow(
        [width, height](const WindowData& w)
        {
            if (w.onKey)
            {
                w.onReshape({width, height});
            }
        });
}

void Window::onKey(unsigned char k, int x, int y)
{
    onCurrentWindow(
        [k, x, y](const WindowData& w)
        {
            if (w.onKey)
            {
                w.onKey(k, {x, y});
            }
        });
}

void Window::onIdle()
{
    onCurrentWindow(
        [](const WindowData& w)
        {
            if (w.onIdle)
            {
                w.onIdle();
            }
        });
}
}
