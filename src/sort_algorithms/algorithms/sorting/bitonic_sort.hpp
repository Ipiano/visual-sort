// Bitonic sort implementation for sort_algorithms::sorting
#pragma once
#include <algorithm>
#include <iterator>

namespace algorithms::sorting
{

// Helper function to find the greatest power of two less than or equal to n
inline size_t greatest_power_of_two_less_than(size_t n)
{
    size_t k = 1;
    while (k << 1 <= n)
    {
        k <<= 1;
    }
    return k;
}

// Bitonic merge: merges a bitonic sequence into a sorted sequence
template <typename RandomIt> void bitonic_merge(RandomIt first, RandomIt last, bool ascending)
{
    auto dist = std::distance(first, last);
    if (dist <= 1)
    {
        return;
    }
    // Only merge if the size is a power of two
    if ((dist & (dist - 1)) != 0)
    {
        // If not a power of two, sort using std::sort as fallback
        if (ascending)
        {
            std::sort(first, last);
        }
        else
        {
            std::sort(first, last, std::greater<typename std::iterator_traits<RandomIt>::value_type>());
        }
        return;
    }
    size_t mid = dist / 2;
    for (size_t i = 0; i < mid; ++i)
    {
        if (ascending == (*(first + i) > *(first + i + mid)))
        {
            std::swap(*(first + i), *(first + i + mid));
        }
    }
    bitonic_merge(first, first + mid, ascending);
    bitonic_merge(first + mid, last, ascending);
}

// Bitonic sort: sorts the range [first, last) in ascending or descending order
template <typename RandomIt> void bitonic_sort(RandomIt first, RandomIt last, bool ascending = true)
{
    auto dist = std::distance(first, last);
    if (dist <= 1)
    {
        return;
    }
    auto mid = dist / 2;
    bitonic_sort(first, first + mid, true);
    bitonic_sort(first + mid, last, false);
    bitonic_merge(first, last, ascending);
}

} // namespace algorithms::sorting
