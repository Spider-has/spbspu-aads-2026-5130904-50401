#ifndef MAX_HEAP_HPP
#define MAX_HEAP_HPP

#include "VectorRandomIter.hpp"
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
}

#endif
