#pragma once

#include <functional>

namespace constants
{
namespace keys
{
const int ESCAPE = 27;
const int ITEMS  = 4;
const int RESET  = 'r';
}

namespace colors
{
const float FREE[]   = {1, 1, 1};
const float ACCESS[] = {1, 0, 0};
const float HOLD[]   = {0, 1, 0};
const float TEXT[]   = {0.5, 0.5, 0.5};
const float BLACK[]  = {0, 0, 0};
}
}

using byte        = unsigned char;
using semfunction = std::function<void()>;
