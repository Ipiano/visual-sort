#pragma once

#include <array>

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
const std::array<float, 3> FREE    = {1, 1, 1};
const std::array<float, 3> MOVE    = {1, 0, 0};
const std::array<float, 3> COMPARE = {0, 0, 1};
const std::array<float, 3> TEXT    = {0.5, 0.5, 0.5};
const std::array<float, 3> BLACK   = {0, 0, 0};
}
}
