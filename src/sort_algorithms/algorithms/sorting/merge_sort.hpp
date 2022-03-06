#pragma once

#include "algorithms/sorting/impl/merge_sort.hpp"

#include <functional>

namespace algorithms::sorting
{
template <class RandomIt> void merge_sort(RandomIt begin, RandomIt end)
{
    _impl::merge_sort(begin, end, std::less<>());
}

template <class RandomIt, class Compare> void merge_sort(RandomIt begin, RandomIt end, Compare c)
{
    _impl::merge_sort(begin, end, std::less<>());
}
}
