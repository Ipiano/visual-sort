#pragma once

#include "algorithms/sorting/impl/scratch_space.hpp"

#include <array>
#include <iterator>
#include <utility>
#include <vector>

namespace algorithms::sorting::_impl
{
template <class RandomIt, class Traits> void bucket_sort(RandomIt begin, RandomIt end, Traits /*traits*/)
{
    using std::distance;
    using std::swap;
    using value_type = typename std::iterator_traits<RandomIt>::value_type;

    const auto item_count = static_cast<std::size_t>(std::distance(begin, end));

    std::array<scratch_space<value_type>, Traits::bucket_count()> buckets;
    for (auto& bucket : buckets)
    {
        bucket = algorithms::sorting::_impl::scratch_space<value_type>(item_count);
    }

    const std::size_t max_iterations = Traits::max_iterations(begin, end);
    for (std::size_t iteration = 0; iteration < max_iterations; ++iteration)
    {
        // Sort into buckets
        for (auto it = begin; it != end; ++it)
        {
            const auto bucket_idx = Traits::bucket_index(*it, iteration, max_iterations);
            buckets.at(bucket_idx).add(std::move(*it));
        }

        // Move back to original list and clears the buckets for the next round
        // (but this should preserve the space allocated)
        auto it = begin;
        for (auto& bucket : buckets)
        {
            it = std::move(bucket.begin(), bucket.end(), it);
            bucket.clear();
        }
    }
}
}
