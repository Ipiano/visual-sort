#pragma once

#include <GL/freeglut.h>

#include <cassert>
#include <functional>
#include <string>
#include <tuple>
#include <unordered_map>

namespace glut
{

// Display mode flags that can be passed when a window is created to set
// it's display type. Can be bitwise or'ed together to combine flags.
using display_mode_t = decltype(GLUT_SINGLE);
namespace display_mode
{
constexpr display_mode_t single_buffered = GLUT_SINGLE;
constexpr display_mode_t double_buffered = GLUT_DOUBLE;
constexpr display_mode_t rgb             = GLUT_RGB;
constexpr display_mode_t rgba            = GLUT_RGBA;
}

// A location in the screen; may be relative different locations
// depending on how it is generated
struct Coordinate
{
    int x;
    int y;
};

// A size of something with width and height
struct Size
{
    int width;
    int height;
};

// Represents a window created with GLUT
//
// Constructing a new Window will invoke glutCreateWindow; destroying
// the window object will invoke glutDestroyWindow. Since under the hood
// all callbacks have to be handled by void(void) functions, the Window
// object also acts as a singleton that routes those callbacks to the
// appropriate Window object.
//
// This object is not threadsafe, and should only be used from the main
// thread.
//
// glut::init() must be invoked before a Window can be created
class Window
{
    template <class... Args> using callback_t = std::function<void(Args...)>;
    using void_callback_t                     = std::function<void(void)>;

    using display_callback_t  = void_callback_t;
    using reshape_callback_t  = callback_t<Size>;
    using idle_callback_t     = void_callback_t;
    using keypress_callback_t = callback_t<unsigned char, Coordinate>;

  public:
    Window(const std::string& name, display_mode_t mode, Coordinate position = {-1, -1}, Size size = {300, 300});
    ~Window();

    template <class T> void setDisplayCallback(T&& callback) { setDisplayCallback(display_callback_t(std::forward<T>(callback))); }
    template <class T> void setReshapeCallback(T&& callback) { setReshapeCallback(reshape_callback_t(std::forward<T>(callback))); }
    template <class T> void setKeyPressCallback(T&& callback) { setKeyPressCallback(keypress_callback_t(std::forward<T>(callback))); }
    template <class T> void setIdleCallback(T&& callback) { setIdleCallback(idle_callback_t(std::forward<T>(callback))); }

  private:
    struct WindowData
    {
        display_callback_t onDisplay;
        reshape_callback_t onReshape;
        keypress_callback_t onKey;
        idle_callback_t onIdle;
    };

    static std::unordered_map<int, WindowData> s_windows;

    void setDisplayCallback(display_callback_t&& callback);
    void setReshapeCallback(reshape_callback_t&& callback);
    void setKeyPressCallback(keypress_callback_t&& callback);
    void setIdleCallback(idle_callback_t&& callback);

    static void initCallbacks();
    static void onDisplay();
    static void onReshape(int width, int height);
    static void onKey(unsigned char k, int x, int y);
    static void onIdle();

    // Invoke some action for the current window
    template <class F> static void onCurrentWindow(F&& f)
    {
        const int win     = glutGetWindow();
        const auto win_it = s_windows.find(win);
        assert(win_it != s_windows.end());

        f(win_it->second);
    }

    int m_window_id;
};
}
