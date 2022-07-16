#pragma once

#include <iterator>
#include <utility>

namespace algorithms::sorting::_impl
{
template <class RandomIt, class Compare> void bubble_sort(RandomIt begin, RandomIt end, Compare compare)
{
    using std::distance;
    using std::swap;

    if (distance(begin, end) <= 1)
    {
        return;
    }

    for (auto end_it = end; end_it > begin; --end_it)
    {
        for (auto it = begin; it != end_it - 1; it++)
        {
            if (!compare(it[0], it[1]))
            {
                swap(it[0], it[1]);
            }
        }
    }
}
}
