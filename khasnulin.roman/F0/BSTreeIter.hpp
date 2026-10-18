#ifndef BSTREE_ITER_HPP
#define BSTREE_ITER_HPP

#include "BSNode.hpp"
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace khasnulin
{
  template < class Key, class Value, bool IsConst > class BSTreeIterator
  {
  public:
    // тип значений которые будет отдавать итератор
    using value_type = std::pair< const Key, Value >;
    // тип расстояния между итераторами для поддержки stl
    using difference_type = std::ptrdiff_t;
    // тип категории итератора
    using iterator_category = std::bidirectional_iterator_tag;
    // указатель на тип пары ключ значение: либо const, либо обычная в зависимости от типа итератора
    using pointer = std::conditional_t< IsConst, const value_type *, value_type * >;
    //  ссылку на пару ключ значение: либо const, либо обычная в зависимости от типа итератора
    using reference = std::conditional_t< IsConst, const value_type &, value_type & >;

    BSTreeIterator() noexcept;

    // default rule of 5
    ~BSTreeIterator() noexcept = default;

    BSTreeIterator(const BSTreeIterator &rhs) noexcept = default;

    BSTreeIterator &operator=(const BSTreeIterator &rhs) noexcept = default;
    BSTreeIterator &operator=(BSTreeIterator &&rhs) noexcept = default;

    // для обычного iter -- обычный конструктор с const ref, для const iter -- конструктор объекта из
    // неконстантного iter
    BSTreeIterator(const BSTreeIterator< Key, Value, false > &rhs) noexcept;

    BSTreeIterator &operator++() noexcept;
    BSTreeIterator operator++(int) noexcept;

    BSTreeIterator &operator--() noexcept;
    BSTreeIterator operator--(int) noexcept;

    reference operator*() const noexcept;
    pointer operator->() const noexcept;

    // шаблоны для перегузки операторов сравнения, чтобы можно было сравнивать между собой const и unconst
    // iters без проблем.
    template < bool OtherConst >
    bool operator==(const BSTreeIterator< Key, Value, OtherConst > &rhs) const noexcept;

    template < bool OtherConst >
    bool operator!=(const BSTreeIterator< Key, Value, OtherConst > &rhs) const noexcept;

  private:
    // тип узла дерева: либо const, либо обычный в зависимости от типа итератора
    using node_ptr = std::conditional_t< IsConst, const Node< Key, Value > *, Node< Key, Value > * >;

    // делаем BSTree дружественным, чтобы он мог вызывать приватный конструктор
    template < class, class, class > friend class BSTree;
    // делаем BSTreeIterator дружественным, чтобы могли брать внутренние поля между const и unconst iters в
    // перегрузках операторов и конструкторах.
    friend class BSTreeIterator< Key, Value, !IsConst >;

    // явный конструктор iter из узла дерева. Будет вызываться в begin и end
    explicit BSTreeIterator(node_ptr node) noexcept;

    // текущий указатель на node в итераторе
    node_ptr node_;
  };
}

#endif
