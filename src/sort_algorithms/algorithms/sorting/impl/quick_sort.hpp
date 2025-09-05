#pragma once

#include <iterator>
#include <utility>

namespace algorithms::sorting::_impl
{
using std::distance;
using std::next;
using std::prev;
using std::swap;

template <class RandomIt, class Compare> RandomIt choose_pivot(RandomIt begin, RandomIt end, Compare compare)
{
    assert(begin < end);
    const auto mid = next(begin, distance(begin, end) / 2);
    if (*begin <= *mid && *mid <= *prev(end))
    {
        return mid;
    }

    if (*mid <= *begin && *begin <= *prev(end))
    {
        return begin;
    }

    return prev(end);
}

template <class RandomIt, class Compare>
void extend_pivot_range(RandomIt begin, RandomIt end, RandomIt& pivot_begin, RandomIt& pivot_end, Compare compare)
{
    const auto equal = [&compare](RandomIt l, RandomIt r) { return !compare(*l, *r) && !compare(*r, *l); };

    // Extend pivot range if the previous adjustment resulted in a larger
    // contiguous equal range than before
    while (pivot_begin > begin && equal(prev(pivot_begin), pivot_begin))
    {
        pivot_begin = prev(pivot_begin);
    }

    while (pivot_end < end && equal(pivot_begin, pivot_end))
    {
        pivot_end = next(pivot_end);
    }
}

template <class RandomIt, class Compare> void move_left_pointer(RandomIt& left, RandomIt& pivot_begin, Compare compare)
{
    // Move the left until a value that belongs on the right is found
    bool continue_left = true;
    while (continue_left)
    {
        continue_left = false;

        // Move through values that are strictly less than pivot
        while (compare(*left, *pivot_begin) && left < pivot_begin)
        {
            left = next(left);
        }

        // If values equal to the pivot are found on the left, move them to
        // the left of the middle partition and keep moving the left index
        if (left < prev(pivot_begin) && !compare(*pivot_begin, *left))
        {
            pivot_begin = prev(pivot_begin);
            swap(*left, *pivot_begin);
            continue_left = true;
        }
    }
}

template <class RandomIt, class Compare>
void move_right_pointer(RandomIt& right, RandomIt& pivot_begin, RandomIt& pivot_end, Compare compare)
{
    // Move the right until a value that belongs on the left is found
    bool continue_right = true;
    while (continue_right)
    {
        continue_right = false;

        // Move through values that are strictly greater than the pivot
        while (compare(*pivot_begin, *right) && right >= pivot_end)
        {
            right = prev(right);
        }

        // If values equal to the pivot are found on the right, move them to
        // the right of the middle partition and keep moving the right index
        if (right > pivot_end && !compare(*right, *pivot_begin))
        {
            swap(*right, *pivot_end);
            pivot_end      = next(pivot_end);
            continue_right = true;
        }
    }
}

template <class RandomIt> void handle_partition_boundaries(RandomIt& left, RandomIt& right, RandomIt& pivot_begin, RandomIt& pivot_end)
{
    // At this point we have one of four conditions
    // * Left < middle and right > middle -> swap left, right
    // * Left = middle, right > middle -> shift middle one to right
    // * Left < middle, right = middle -> shift middle one to left
    // * Left = middle, right = middle -> recurse on left and right partitions

    if (left < pivot_begin && right >= pivot_end)
    {
        swap(*left, *right);
    }
    else if (left == pivot_begin && right >= pivot_end)
    {
        swap(*pivot_begin, *pivot_end);
        pivot_begin = next(pivot_begin);
        pivot_end   = next(pivot_end);
    }
    else if (right < pivot_end && left < pivot_begin)
    {
        pivot_begin = prev(pivot_begin);
        pivot_end   = prev(pivot_end);
        swap(*pivot_begin, *pivot_end);
    }
}

template <class RandomIt, class Compare> void quick_sort(RandomIt begin, RandomIt end, Compare compare)
{
    if (distance(begin, end) < 2)
    {
        return;
    }
    if (distance(begin, end) == 2)
    {
        if (!compare(*begin, *prev(end)))
        {
            swap(*begin, *prev(end));
        }
        return;
    }

    // Select a pivot value
    auto pivot = choose_pivot(begin, end, compare);

    // Move the pivot value to the middle, assuming half on left and half on
    // right
    const auto mid = next(begin, distance(begin, end) / 2);
    if (mid != pivot)
    {
        swap(*pivot, *mid);
        pivot = mid;
    }

    // Build three partitions - less than pivot, equal pivot, greater than pivot
    // After we init these iterators, pivot becomes useless; use pivot_begin as
    // the canonical "value of pivot" iterator
    auto left        = begin;
    auto pivot_begin = pivot;
    auto pivot_end   = next(pivot);
    auto right       = prev(end);

    while (left < pivot_begin && right >= pivot_end)
    {
        extend_pivot_range(begin, end, pivot_begin, pivot_end, compare);
        move_left_pointer(left, pivot_begin, compare);
        move_right_pointer(right, pivot_begin, pivot_end, compare);
        handle_partition_boundaries(left, right, pivot_begin, pivot_end);
    }

    quick_sort(begin, pivot_begin, compare);
    quick_sort(pivot_end, end, compare);
}
}
