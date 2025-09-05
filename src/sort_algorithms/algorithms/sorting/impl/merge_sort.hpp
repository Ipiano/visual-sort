#pragma once

#include "algorithms/sorting/impl/scratch_space.hpp"

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

namespace algorithms::sorting::_impl
{

template <class Iterator> using value_type_t = typename iterator_traits<Iterator>::value_type;

template <class RandomIt, class Compare>
void merge_sort(const RandomIt begin, const RandomIt end, Compare compare, scratch_space<value_type_t<RandomIt>>& scratch)
{
    using std::distance;
    using std::iterator_traits;
    using std::swap;

    const auto dist = distance(begin, end);

    if (dist < 2)
    {
        return;
    }

    const auto pivot = begin + (dist / 2);
    merge_sort(begin, pivot, compare, scratch);
    merge_sort(pivot, end, compare, scratch);

    auto left  = begin;
    auto right = pivot;
    scratch.clear();

    while (left < pivot && right < end)
    {
        if (compare(*left, *right))
        {
            scratch.add(std::move(*left));
            ++left;
        }
        else
        {
            scratch.add(std::move(*right));
            ++right;
        }
    }

    while (left < pivot)
    {
        scratch.add(std::move(*left));
        ++left;
    }

    while (right < end)
    {
        scratch.add(std::move(*right));
        ++right;
    }

    assert(scratch.size() == dist);
    std::move(scratch.begin(), scratch.end(), begin);
}

template <class RandomIt, class Compare> void merge_sort(RandomIt begin, RandomIt end, Compare compare)
{
    scratch_space<value_type_t<RandomIt>> scratch(distance(begin, end));
    merge_sort(begin, end, compare, scratch);
}
}
