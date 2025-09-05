#pragma once

#include <algorithm>
#include <chrono>
#include <iterator>
#include <random>
#include <utility>

namespace algorithms::sorting::_impl
{
template <class RandomIt, class Compare> void sift_item(RandomIt begin, RandomIt end, RandomIt sift_item, Compare compare)
{
    using std::distance;
    using std::next;
    using std::swap;

    assert(sift_item >= begin);
    assert(sift_item < end);

    const auto end_dist = static_cast<std::size_t>(distance(begin, end));

    RandomIt root  = sift_item;
    auto root_dist = distance(begin, root);

    while ((root_dist * 2) + 1 < end_dist)
    {
        const auto child_1_dist = (root_dist * 2) + 1;
        const auto child_2_dist = child_1_dist + 1;

        const auto child_1 = next(begin, child_1_dist);
        const auto child_2 = next(begin, child_2_dist);

        RandomIt swap_candidate = root;

        if (compare(*swap_candidate, *child_1))
        {
            swap_candidate = child_1;
        }

        if (child_2_dist < end_dist && compare(*swap_candidate, *child_2))
        {
            swap_candidate = child_2;
        }

        if (swap_candidate == root)
        {
            return;
        }

        swap(*root, *swap_candidate);

        root      = swap_candidate;
        root_dist = distance(begin, root);
    }
}

template <class RandomIt, class Compare> void heap_sort(RandomIt begin, RandomIt end, Compare compare)
{
    assert(begin <= end);

    using std::distance;

    if (distance(begin, end) < 2)
    {
        return;
    }

    auto it = end - 1;

    // Build initial heap by organizing all items initially to be a valid heap
    for (; it != begin; --it)
    {
        sift_item(begin, end, it, compare);
    }
    sift_item(begin, end, begin, compare);

    // For each item, swap with the front (which is the biggest item remaining)
    // and shift that new item into the heap
    it = end - 1;
    while (it > begin)
    {
        swap(*it, *begin);
        sift_item(begin, it, begin, compare);
        --it;
    }
}
}
