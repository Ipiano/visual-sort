#pragma once

#include <algorithm>
#include <chrono>
#include <iterator>
#include <random>

namespace algorithms::sorting::_impl
{
template <class RandomIt, class Compare, class CancelPredicate>
void bogo_sort(RandomIt begin, RandomIt end, Compare compare, CancelPredicate cancelled)
{
    using std::distance;

    if (distance(begin, end) <= 1)
    {
        return;
    }

    std::mt19937 reng(static_cast<std::mt19937::result_type>(std::chrono::system_clock::now().time_since_epoch().count()));
    bool sorted = false;
    while (!sorted && !cancelled())
    {
        std::shuffle(begin, end, reng);

        sorted = true;
        for (auto left = begin, right = std::next(left); sorted && right != end; left = right, right = std::next(right))
        {
            sorted = compare(*left, *right);
        }
    }
}
}
