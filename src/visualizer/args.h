#pragma once

#include "sort_visualizer.h"

#include <functional>
#include <memory>

struct ProgramArgs
{
    std::size_t steps_between_draws = 0;

    // Function that will produce data sets to sort
    std::function<std::vector<int>()> data_set_factory;

    // Function that should be invoked to sort a list
    SortVisualizer::sort_function sort_function;

    // Function that should be invoked to draw the list
    SortVisualizer::draw_function draw_function;
};

ProgramArgs parse_args(int argc, char** argv);
