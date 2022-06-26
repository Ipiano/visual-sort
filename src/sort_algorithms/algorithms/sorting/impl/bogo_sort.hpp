#pragma once

#include <algorithm>
#include <chrono>
#include <iterator>
#include <random>
#include <utility>

namespace algorithms::sorting::_impl
{
template <class RandomIt, class Compare, class CancelPredicate>
void bogo_sort(RandomIt begin, RandomIt end, Compare compare, CancelPredicate&& cancelled)
{
    if (begin == end)
    {
        return;
    }

    std::mt19937 reng(static_cast<std::mt19937::result_type>(std::chrono::system_clock::now().time_since_epoch().count()));
    bool sorted = false;
    do
    {
        std::shuffle(begin, end, reng);

        sorted = true;
        for (auto left = begin, right = std::next(left); sorted && right != end; left = right, right = std::next(right))
        {
            sorted = compare(*left, *right);
        }
    } while (!sorted && !cancelled());
}
}
