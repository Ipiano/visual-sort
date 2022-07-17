#pragma once

#include <cassert>
#include <vector>

namespace algorithms::sorting::_impl
{
using std::distance;
using std::iterator_traits;
using std::move;
using std::swap;

// Appropriately does a swap to next or emplace depending on how full it
// is
template <class T> class scratch_space
{
  public:
    using iterator = typename std::vector<T>::iterator;

    scratch_space(std::size_t capacity = 0)
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
}
