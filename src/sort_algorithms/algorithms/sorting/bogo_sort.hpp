#pragma once

#include "algorithms/sorting/impl/bogo_sort.hpp"

#include <atomic>
#include <functional>

namespace algorithms::sorting
{
template <class RandomIt> void bogo_sort(RandomIt begin, RandomIt end)
{
    _impl::bogo_sort(begin, end, std::less<>(), [] { return false; });
}

template <class RandomIt, class Compare> void bogo_sort(RandomIt begin, RandomIt end, Compare c)
{
    _impl::bogo_sort(begin, end, c, [] { return false; });
}

template <class RandomIt> void bogo_sort(RandomIt begin, RandomIt end, const std::atomic_bool& cancel_flag)
{
    _impl::bogo_sort(begin, end, std::less<>(), [&cancel_flag]() -> bool { return cancel_flag; });
}

template <class RandomIt, class Compare> void bogo_sort(RandomIt begin, RandomIt end, Compare c, const std::atomic_bool& cancel_flag)
{
    _impl::bogo_sort(begin, end, c, [&cancel_flag]() -> bool { return cancel_flag; });
}
}
