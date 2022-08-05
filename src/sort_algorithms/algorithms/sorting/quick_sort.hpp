#pragma once

#include "algorithms/sorting/impl/quick_sort.hpp"

#include <functional>

namespace algorithms::sorting
{
template <class RandomIt> void quick_sort(RandomIt begin, RandomIt end)
{
    _impl::quick_sort(begin, end, std::less<>());
}

template <class RandomIt, class Compare> void quick_sort(RandomIt begin, RandomIt end, Compare c)
{
    _impl::quick_sort(begin, end, std::less<>());
}
}
