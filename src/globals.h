#pragma once

#include <memory>

namespace sorting
{
class sort_handle;
}

namespace global
{
extern int screen_width;
extern int screen_height;
extern int window;

extern std::unique_ptr<sorting::sort_handle> sorter;
}
