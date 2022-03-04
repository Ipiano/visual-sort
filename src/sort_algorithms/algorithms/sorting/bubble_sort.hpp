#pragma once

#include "algorithms/sorting/impl/bubble_sort.hpp"

#include <functional>

namespace algorithms::sorting
{
template <class RandomIt> void bubble_sort(RandomIt begin, RandomIt end)
{
    _impl::bubble_sort(begin, end, std::less<>());
}

template <class RandomIt, class Compare> void bubble_sort(RandomIt begin, RandomIt end, Compare c)
{
    _impl::bubble_sort(begin, end, std::less<>());
}
}
