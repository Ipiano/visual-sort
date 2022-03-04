#pragma once

#include <memory>

namespace sorting
{
class visual_sort;
}

enum class Visualization
{
    // Slope from bottom left to upper right
    SLOPE_BARS,

    // Slope, but it's shifted up and mirrored across
    // the X axis
    SLOPE_MIRRORED,

    // Slope, but only the top of each bar is drawn
    SLOPE_POINTS,

    // Placeholder for max value
    MAX_VALUE
};

struct ProgramArgs
{
    Visualization draw_mode;

    // If true, all sorted values should be unique
    bool unique_values;

    // Number of sort algorithm steps to execute between draws
    size_t steps_per_draw;

    // Number of items to sort
    size_t set_size;

    // Always set; contains a pointer that lives for the lifetime
    // of the application
    sorting::visual_sort* sort_algorithm;
};

ProgramArgs parse_args(int argc, char** argv);
