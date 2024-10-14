#pragma once

#include "algorithms/sorting/impl/heap_sort.hpp"

#include <functional>

namespace algorithms::sorting
{
template <class RandomIt> void heap_sort(RandomIt begin, RandomIt end)
{
    _impl::heap_sort(begin, end, std::less<>());
}

template <class RandomIt, class Compare> void heap_sort(RandomIt begin, RandomIt end, Compare c)
{
    _impl::heap_sort(begin, end, std::less<>());
}
}
