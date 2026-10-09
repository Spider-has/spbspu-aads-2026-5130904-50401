#ifndef MAX_HEAP_HPP
#define MAX_HEAP_HPP

#include "Vector.hpp"
#include <cstddef>

namespace khasnulin
{
  template < class T, class Compare > class MaxHeap
  {
  public:
    using value_compare = Compare;
    using value_type = T;
    using size_type = size_t;
    using reference = T &;
    using const_reference = const T &;

    // конструктор по умолчанию
    MaxHeap();

    explicit MaxHeap(const Compare &compare);

    // положить в кучу элемент.
    void push(const_reference value);
    void push(value_type &&value);

    // Убрать элемент из сверху кучи. Если куча пуста -- проброс std::out_of_range
    void pop();

    // Реализуем swap для ADL
    void swap(MaxHeap &rhs) noexcept;

    // Взять элемент с верха кучи, строго по константной ссылке. Если куча пуста -- проброс std::out_of_range
    const_reference top() const;

    // Проверить, пуста ли куча
    bool empty() const noexcept;

    // Вернуть размер кучи
    size_type size() const noexcept;

  private:
    static size_type parent(size_type node) noexcept;
    static size_type leftChild(size_type node) noexcept;
    static size_type rightChild(size_type node) noexcept;

    void siftUp(size_type node);
    void siftDown(size_type node);

    Compare compare_;
    Vector< value_type > heap_;
  };

  template < class T, class Compare >
  MaxHeap< T, Compare >::MaxHeap():
      compare_(),
      heap_()
  {
  }

  template < class T, class Compare >
  MaxHeap< T, Compare >::MaxHeap(const Compare &compare):
      compare_(compare),
      heap_()
  {
  }

  template < class T, class Compare >
  typename MaxHeap< T, Compare >::size_type MaxHeap< T, Compare >::parent(size_type node) noexcept
  {
    if (node != 0)
    {
      return (node - 1) / 2;
    }
    return 0;
  }

  template < class T, class Compare >
  typename MaxHeap< T, Compare >::size_type MaxHeap< T, Compare >::leftChild(size_type node) noexcept
  {
    return (node * 2) + 1;
  }

  template < class T, class Compare >
  typename MaxHeap< T, Compare >::size_type MaxHeap< T, Compare >::rightChild(size_type node) noexcept
  {
    return (node * 2) + 2;
  }

  template < class T, class Compare > void MaxHeap< T, Compare >::siftUp(size_type node)
  {
    size_type par = parent(node);
    if (par != node && compare_(heap_[par], heap_[node]))
    {
      std::swap(heap_[par], heap_[node]);
      siftUp(par);
    }
  }

  template < class T, class Compare > void MaxHeap< T, Compare >::siftDown(size_type node)
  {
    size_type par = parent(node);
    if (par != node && compare_(heap_[par], heap_[node]))
    {
      std::swap(heap_[par], heap_[node]);
      siftUp(par);
    }
  }
}

#endif
