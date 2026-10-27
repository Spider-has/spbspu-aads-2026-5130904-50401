#ifndef BOUNDED_PRIORITY_QUEUE_HPP
#define BOUNDED_PRIORITY_QUEUE_HPP

#include "MaxHeap.hpp"
#include "Vector.hpp"
#include <cstddef>

namespace khasnulin
{
  // Compare(const T& a, const T& b), a лучше b; хранит limit лучших
  template < class T, class Compare > class BoundedPriorityQueue
  {
  public:
    using size_type = std::size_t;
    using value_compare = Compare;
    using value_type = T;
    using reference = T &;
    using const_reference = const T &;

    explicit BoundedPriorityQueue(size_type limit, const Compare &better);

    // попал кандидат в отбор или нет. true -- попал
    bool suggest(const T &val);
    bool suggest(T &&val);

    size_type limit() const noexcept;
    size_type size() const noexcept;
    bool empty() const noexcept;

    void clear() noexcept;

    template < class F > void traverseAndClear(F f);

  private:
    size_type limit_;
    MaxHeap< T, Compare > heap_;
  };
}

#endif
