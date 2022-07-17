#pragma once

#include <array>
#include <iterator>
#include <utility>
#include <vector>

namespace algorithms::sorting::_impl
{
template <class RandomIt, class Traits> void bucket_sort(RandomIt begin, RandomIt end, Traits)
{
    using std::distance;
    using std::swap;
    using value_type = typename std::iterator_traits<RandomIt>::value_type;

    const auto item_count = static_cast<std::size_t>(std::distance(begin, end));

    std::array<std::vector<value_type>, Traits::bucket_count()> buckets;
    for (auto& bucket : buckets)
    {
        bucket.reserve(item_count);
    }

    const std::size_t max_iterations = Traits::max_iterations(begin, end);
    for (std::size_t iteration = 0; iteration < max_iterations; ++iteration)
    {
        // Sort into buckets
        for (auto it = begin; it != end; ++it)
        {
            buckets[Traits::bucket_index(*it, iteration, max_iterations)].emplace_back(std::move(*it));
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
