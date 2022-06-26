#pragma once

#include <functional>
#include <memory>

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

    // Function that will produce data sets to sort
    std::function<std::vector<int>()> data_set_factory;

    // Number of sort algorithm steps to execute between draws
    size_t steps_per_draw;

};

ProgramArgs parse_args(int argc, char** argv);
