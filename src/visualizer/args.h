#pragma once

#include "sort_visualizer.h"

#include <functional>
#include <memory>

struct ProgramArgs
{
    bool loop = false;

    std::size_t steps_between_draws = 0;

    // Function that will produce data sets to sort
    std::function<std::vector<int>()> data_set_factory;

    // Factory to get a sort function and draw function
    std::function<std::pair<SortVisualizer::sort_function, SortVisualizer::draw_function>()> get_sort_and_draw_function;
};

ProgramArgs parse_args(int argc, char** argv);
