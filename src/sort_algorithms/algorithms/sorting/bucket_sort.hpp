#pragma once

#include "algorithms/sorting/impl/bucket_sort.hpp"

#include <functional>
#include <iterator>
#include <limits>
#include <numeric>
#include <type_traits>

namespace algorithms::sorting
{
namespace bucket
{
// Traits type must be defined for whatever is being sorted. This library provides
// implementations for std::string and any integer types
//
// The second paramter can be used with an enable_if to specify the traits for
// a swath of types (e.g. all integers)
template <class ValueT, class EnableT = void> struct traits;

// Bucket sort for integers is just a radix sort
template <class ValueT> struct traits<ValueT, std::enable_if_t<std::is_integral_v<ValueT>, void>>
{
    // Sort by 0 or 1 bits
    constexpr static std::size_t bucket_count() noexcept { return 2; }

    // Check order is least-significant to most
    // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
    static std::size_t bucket_index(ValueT v, std::size_t iteration /*iteration is 0-based*/, std::size_t max_iterations) noexcept
    {
        static_cast<void>(max_iterations);
        return ((static_cast<uint64_t>(v) & (1U << iteration)) != 0) ? 1 : 0;
    }

    // Always just check all bits - could maybe go through and figure out what the most
    // significant bit in use is and just sort to that point.
    template <class IteratorT> static std::size_t max_iterations(IteratorT begin, IteratorT end) noexcept
    {
        return std::accumulate(begin, end, 0, [](std::size_t curr, ValueT n) { return std::max(curr, most_significant_bit(n)); }) + 1;
    }

  private:
    // Get the index of the most significant bit set in n. Returns 0 if N is 0
    static std::size_t most_significant_bit(ValueT n) noexcept
    {
        std::size_t bit = (sizeof(ValueT) * 8) - 1;
        while (bit > 0 && (n & (1 << bit)) == 0)
        {
            bit--;
        }
        return bit;
    }
};

// Bucket sort for strings uses 255 buckets
template <> struct traits<std::string>
{
    using ValueT = std::string;

    // Sort by 0 or 1 bits
    constexpr static std::size_t bucket_count() noexcept { return 256; }

    // Check order is last character to first; if the iteration count is
    // larger than the string, then it will return bucket 0 until we finish
    // enough iterations
    // NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
    static std::size_t bucket_index(const ValueT& v, std::size_t iteration /*iteration is 0-based*/, std::size_t max_iterations) noexcept
    {
        const auto desired_index = max_iterations - iteration - 1;
        return desired_index >= v.size() ? 0U : (static_cast<size_t>(v[desired_index]) + 1U);
    }

    // Need to iterate once per character in the longest string
    template <class IteratorT> static std::size_t max_iterations(IteratorT begin, IteratorT end) noexcept
    {
        return std::accumulate(begin, end, 0, [](std::size_t curr, const std::string& s) { return std::max(curr, s.size()); });
    }
};
}

template <class RandomIt> void bucket_sort(RandomIt begin, RandomIt end)
{
    using value_type  = typename std::iterator_traits<RandomIt>::value_type;
    using traits_type = typename bucket::traits<value_type>;

    _impl::bucket_sort(begin, end, traits_type());
}

template <class RandomIt, class Traits> void bucket_sort(RandomIt begin, RandomIt end, Traits traits)
{
    _impl::bucket_sort(begin, end, traits);
}
}
