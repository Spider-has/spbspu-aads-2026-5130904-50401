#ifndef BSTREE_HPP
#define BSTREE_HPP

#include "BSNode.hpp"
#include "BSTreeIter.hpp"
#include <cstddef>
#include <functional>
#include <utility>

namespace khasnulin
{
  template < class Key, class Value, bool IsConst > class BSTreeIterator;

  template < class Key, class Value > using BSTreeIter = BSTreeIterator< Key, Value, false >;

  template < class Key, class Value > using CBSTreeIter = BSTreeIterator< Key, Value, true >;

  template < class Key, class Value, class Compare = std::less< Key > > class BSTree
  {
  public:
    using key_type = Key;
    using value_type = Value;
    using size_type = size_t;

    using public_value_pair_type = std::pair< const Key, Value >;

    using comparator = Compare;

    using iter = BSTreeIter< Key, Value >;
    using const_iter = CBSTreeIter< Key, Value >;

    ~BSTree();

    BSTree();

    explicit BSTree(Compare comparator);

    BSTree(const BSTree &rhs);
    BSTree(BSTree &&rhs) noexcept;

    BSTree &operator=(const BSTree &rhs);
    BSTree &operator=(BSTree &&rhs) noexcept;

    bool empty() const noexcept;
    size_type size() const noexcept;

    // ВОзвращает высоту самого длинного пути от корня в дереве
    size_type height() const;
    // Возвращает высоту самого длинного пути от текущего элемента в корне.
    size_type height(const_iter it) const;

    // итераторы начала обхода дерева
    iter begin() noexcept;
    const_iter begin() const noexcept;
    const_iter cbegin() const noexcept;

    iter end() noexcept;
    const_iter end() const noexcept;
    const_iter cend() const noexcept;

    // получить значение по ключу
    value_type &get(const Key &k);
    const value_type &get(const Key &k) const;

    // получить значение по ключу, если такого нет -- создать такой узел с пустым значением
    value_type &operator[](const Key &k);

    // найти элемент в дереве по ключу, вернуть итератор, если элемента нет, итератор в end
    iter find(const Key &k);
    const_iter find(const Key &k) const;

    // вставить элемент value в дереве по ключу.
    void insert(const public_value_pair_type &pair);
    void insert(public_value_pair_type &&pair);

    // удалить элемент с ключом k из дерева
    void erase(const Key &k);
    // удалить элемент по итератору. вернуть итератор на следующий элемент
    iter erase(const_iter it);

    // очистить полностью дерево от всех узлов
    void clear() noexcept;

    // swap для ADL
    void swap(BSTree &rhs) noexcept;

    // 4 метода поворотов для будущей обретки других деревьев.
    iter rotateLeft(const_iter it);
    iter rotateRight(const_iter it);
    iter rotateLargeLeft(const_iter it);
    iter rotateLargeRight(const_iter it);

    // методы обхода разными способами с применением функционального объекта f
    template < class F > F traverseLNR(F f) const;
    template < class F > F traverseRNL(F f) const;
    template < class F > F traverseBreadth(F f) const;

  private:
    size_type size_;

    Node< Key, Value > fake_root_;

    Compare comparator_;
  };

}

#endif
