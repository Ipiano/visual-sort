#include "globals.h"

#include "sorting/sort_handler.h"

namespace global
{
int screen_width  = 0;
int screen_height = 0;
int window        = 0;

std::unique_ptr<sorting::sort_handle> sorter = nullptr;
}
