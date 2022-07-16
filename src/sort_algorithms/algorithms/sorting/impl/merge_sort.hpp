#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <utility>
#include <vector>

namespace algorithms::sorting::_impl
{
using std::distance;
using std::iterator_traits;
using std::move;
using std::swap;

template <class Iterator> using value_type_t = typename iterator_traits<Iterator>::value_type;

// Appropriately does a swap to next or emplace depending on how full it
// is
template <class T> class scratch_space
{
  public:
    using iterator = typename std::vector<T>::iterator;

    scratch_space(std::size_t capacity)
    {
        m_buffer.reserve(capacity);
        m_next = m_buffer.begin(); // NOLINT (cppcoreguidelines-prefer-member-initializer) This must be initialized after reserve()
    }

    void add(T& value)
    {
        if (m_next == m_buffer.end())
        {
            // Emplace should never result in a resize because we always
            // reserve enough space for the final merge of the sort
            assert(m_buffer.size() < m_buffer.capacity());

            m_buffer.emplace_back(move(value));
        }
        else
        {
            swap(value, *m_next);
        }
        ++m_next;
    }

    iterator begin() { return m_buffer.begin(); }
    iterator end() { return m_next; }

    void clear() { m_next = m_buffer.begin(); }
    std::size_t size() { return static_cast<std::size_t>(distance(begin(), end())); }

  private:
    std::vector<T> m_buffer;
    iterator m_next;
};

template <class RandomIt, class Compare>
void merge_sort(const RandomIt begin, const RandomIt end, Compare compare, scratch_space<value_type_t<RandomIt>>& scratch)
{
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
            scratch.add(*left);
            ++left;
        }
        else
        {
            scratch.add(*right);
            ++right;
        }
    }

    while (left < pivot)
    {
        scratch.add(*left);
        ++left;
    }

    while (right < end)
    {
        scratch.add(*right);
        ++right;
    }

    assert(scratch.size() == dist);
    move(scratch.begin(), scratch.end(), begin);
}

template <class RandomIt, class Compare> void merge_sort(RandomIt begin, RandomIt end, Compare compare)
{
    scratch_space<value_type_t<RandomIt>> scratch(distance(begin, end));
    merge_sort(begin, end, compare, scratch);
}
}
