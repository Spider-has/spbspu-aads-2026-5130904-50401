#ifndef BSTREE_HPP
#define BSTREE_HPP

#include "BSNode.hpp"
#include "BSTreeIter.hpp"
#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace khasnulin
{
  template < class Key, class Value > using BSTreeIter = BSTreeIterator< Key, Value, false >;

  template < class Key, class Value > using CBSTreeIter = BSTreeIterator< Key, Value, true >;

  template < class Key, class Value, class Compare = std::less< Key > > class BSTree
  {

    static_assert(std::is_nothrow_default_constructible< Key >::value,
                  "BSTree: Key must be nothrow default constructible (used by sentinel node)");
    static_assert(std::is_nothrow_default_constructible< Value >::value,
                  "BSTree: Value must be nothrow default constructible (used by sentinel node)");
    static_assert(std::is_nothrow_destructible< Key >::value && std::is_nothrow_destructible< Value >::value,
                  "BSTree: Key and Value destructors must not throw");
    static_assert(std::is_nothrow_move_constructible< Compare >::value &&
                      std::is_nothrow_move_assignable< Compare >::value,
                  "BSTree: Compare must be nothrow movable (required by noexcept move/swap)");

  public:
    using key_type = Key;
    using mapped_type = Value;
    using size_type = size_t;

    using value_type = std::pair< const Key, Value >;

    using comparator = Compare;

    using iter = BSTreeIter< Key, Value >;
    using const_iter = CBSTreeIter< Key, Value >;

    BSTree();

    explicit BSTree(Compare comparator);

    BSTree(const BSTree &rhs);
    BSTree(BSTree &&rhs) noexcept;

    BSTree &operator=(const BSTree &rhs);
    BSTree &operator=(BSTree &&rhs) noexcept;

    ~BSTree() noexcept;

    bool empty() const noexcept;
    size_type size() const noexcept;

    // Возвращает высоту самого длинного пути от корня в дереве
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
    mapped_type &get(const Key &k);
    const mapped_type &get(const Key &k) const;

    // получить значение по ключу, если такого нет -- создать такой узел с пустым значением
    mapped_type &operator[](const Key &k);

    // найти элемент в дереве по ключу, вернуть итератор, если элемента нет, итератор в end
    iter find(const Key &k);
    const_iter find(const Key &k) const;

    // вставить элемент value в дереве по ключу и вернуть итератор на него. Если key уже есть в дереве, просто
    // возвращает iter на этот элемент без изменений.
    iter insert(const value_type &pair);
    iter insert(value_type &&pair);

    // удалить элемент с ключом k из дерева
    size_t erase(const Key &k);
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
    struct FindResult
    {
      Node< Key, Value > *node;
      Node< Key, Value > *parent;
      bool isLeft;
    };
    // const метод возвращает не const данные. Сделано специально для внутренного использования.
    FindResult findByKey(const Key &k) const;
    template < class P > iter insertImpl(P &&pair);
    void addNewNodeToTree(Node< Key, Value > *node, Node< Key, Value > *parent, bool isLeft) noexcept;

    void removeLeaf(Node< Key, Value > *node) noexcept;

    size_type size_;

    Node< Key, Value > fakeRoot_;

    Compare comparator_;
  };

  template < class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::removeLeaf(Node< Key, Value > *node) noexcept
  {
    assert(node != nullptr && node != &fakeRoot_);
    assert(node->left == nullptr && node->right == nullptr);
    Node< Key, Value > *parent = node->parent;
    bool isLeft = (parent->left == node);
    if (isLeft)
    {
      parent->left = nullptr;
    }
    else
    {
      parent->right = nullptr;
    }
    delete node;
    --size_;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree():
      size_(0),
      fakeRoot_(),
      comparator_()
  {
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(Compare comparator):
      size_(0),
      fakeRoot_(),
      comparator_(comparator)
  {
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(const BSTree &rhs):
      size_(0),
      fakeRoot_(),
      comparator_(rhs.comparator_)
  {
    // TODO: add deep copy
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare >::BSTree(BSTree &&rhs) noexcept:
      size_(rhs.size_),
      fakeRoot_(),
      comparator_(std::move(rhs.comparator_))
  {
    fakeRoot_.left = rhs.fakeRoot_.left;
    if (fakeRoot_.left != nullptr)
    {
      fakeRoot_.left->parent = &fakeRoot_;
    }
    rhs.size_ = 0;
    rhs.fakeRoot_.left = nullptr;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > &BSTree< Key, Value, Compare >::operator=(const BSTree &rhs)
  {
    if (std::addressof(rhs) == this)
    {
      return *this;
    }
    // TODO copy and swap idiom
    return *this;
  }

  template < class Key, class Value, class Compare >
  BSTree< Key, Value, Compare > &BSTree< Key, Value, Compare >::operator=(BSTree &&rhs) noexcept
  {
    if (std::addressof(rhs) == this)
    {
      return *this;
    }

    swap(rhs);
    return *this;
  }

  template < class Key, class Value, class Compare > BSTree< Key, Value, Compare >::~BSTree() noexcept
  {
    // TODO: add clear;
  }

  template < class Key, class Value, class Compare >
  bool BSTree< Key, Value, Compare >::empty() const noexcept
  {
    return size_ == 0;
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::size_type BSTree< Key, Value, Compare >::size() const noexcept
  {
    return size_;
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::size_type BSTree< Key, Value, Compare >::height() const
  {
    // TODO: add BFS to find max height;
    return 0;
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::size_type BSTree< Key, Value, Compare >::height(const_iter it) const
  {
    size_type maxHeight = 0;
    return maxHeight;
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::begin() noexcept
  {
    Node< Key, Value > *min = minNode(&fakeRoot_);
    return iter(min);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iter BSTree< Key, Value, Compare >::begin() const noexcept
  {
    return const_iter(const_cast< BSTree< Key, Value, Compare > * >(this)->begin());
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iter BSTree< Key, Value, Compare >::cbegin() const noexcept
  {
    return const_iter(begin());
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::end() noexcept
  {
    return iter(&fakeRoot_);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iter BSTree< Key, Value, Compare >::end() const noexcept
  {
    return const_iter(&fakeRoot_);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iter BSTree< Key, Value, Compare >::cend() const noexcept
  {
    return const_iter(end());
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::mapped_type &BSTree< Key, Value, Compare >::get(const Key &k)
  {
    FindResult result = findByKey(k);
    if (!result.node)
    {
      throw std::out_of_range("can't find node with such key in tree");
    }
    return result.node->val.second;
  }

  template < class Key, class Value, class Compare >
  const typename BSTree< Key, Value, Compare >::mapped_type &
  BSTree< Key, Value, Compare >::get(const Key &k) const
  {
    FindResult result = findByKey(k);
    if (!result.node)
    {
      throw std::out_of_range("can't find node with such key in tree");
    }
    return result.node->val.second;
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::mapped_type &BSTree< Key, Value, Compare >::operator[](const Key &k)
  {
    FindResult result = findByKey(k);
    if (result.node)
    {
      return result.node->val.second;
    }
    Node< Key, Value > *newNode = new Node< Key, Value >({k, Value()});
    addNewNodeToTree(newNode, result.parent, result.isLeft);
    return newNode->val.second;
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::FindResult
  BSTree< Key, Value, Compare >::findByKey(const Key &k) const
  {
    Node< Key, Value > *parent = const_cast< Node< Key, Value > * >(&fakeRoot_);
    Node< Key, Value > *curr = fakeRoot_.left;
    bool isLeft = true;
    while (curr)
    {
      if (comparator_(k, curr->val.first))
      {
        parent = curr;
        curr = curr->left;
        isLeft = true;
      }
      else if (comparator_(curr->val.first, k))
      {
        parent = curr;
        curr = curr->right;
        isLeft = false;
      }
      else
      {
        return {curr, parent, isLeft};
      }
    }
    return {nullptr, parent, isLeft};
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::find(const Key &k)
  {
    FindResult result = findByKey(k);
    return result.node ? iter(result.node) : end();
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::const_iter BSTree< Key, Value, Compare >::find(const Key &k) const
  {
    FindResult result = findByKey(k);
    return result.node ? const_iter(result.node) : end();
  }

  template < class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::addNewNodeToTree(Node< Key, Value > *node, Node< Key, Value > *parent,
                                                       bool isLeft) noexcept
  {
    if (isLeft)
    {
      parent->left = node;
    }
    else
    {
      parent->right = node;
    }
    node->parent = parent;
    ++size_;
  }

  template < class Key, class Value, class Compare >
  template < class P >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::insertImpl(P &&pair)
  {
    FindResult result = findByKey(pair.first);
    if (result.node)
    {
      return iter(result.node);
    }
    Node< Key, Value > *newNode = new Node< Key, Value >(std::forward< P >(pair));
    addNewNodeToTree(newNode, result.parent, result.isLeft);
    return iter(newNode);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::insert(const value_type &pair)
  {
    return insertImpl(pair);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::insert(value_type &&pair)
  {
    return insertImpl(std::move(pair));
  }

  template < class Key, class Value, class Compare > size_t BSTree< Key, Value, Compare >::erase(const Key &k)
  {
    // TODO: find an elem with such key, if there no item with such key -> throw out of range, if we have that
    // node, than we analise its child nodes and choose on of them to replace current node, then delete this
    // node;
    return 0;
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::erase(const_iter it)
  {
    // TODO: check iter refers to key in this tree, if not -> throw out of range error, else we get next elem
    // of this node and analise its child nodes and choose on of them to replace current node, than delete
    // this node, then return next elem
    return iter(&fakeRoot_);
  }

  template < class Key, class Value, class Compare > void BSTree< Key, Value, Compare >::clear() noexcept
  {
    Node< Key, Value > *curr = fakeRoot_.left;
    while (fakeRoot_.left != nullptr)
    {
      if (curr->left)
      {
        curr = curr->left;
      }
      else if (curr->right)
      {
        curr = curr->right;
      }
      else
      {
        Node< Key, Value > *parent = curr->parent;
        removeLeaf(curr);
        curr = parent;
      }
    }
  }

  template < class Key, class Value, class Compare >
  void BSTree< Key, Value, Compare >::swap(BSTree &rhs) noexcept
  {
    if (std::addressof(rhs) == this)
    {
      return;
    }

    std::swap(fakeRoot_.left, rhs.fakeRoot_.left);
    if (fakeRoot_.left)
    {
      fakeRoot_.left->parent = &fakeRoot_;
    }
    if (rhs.fakeRoot_.left)
    {
      rhs.fakeRoot_.left->parent = &rhs.fakeRoot_;
    }

    std::swap(size_, rhs.size_);
    std::swap(comparator_, rhs.comparator_);
  }

  // 4 метода поворотов для будущей обретки других деревьев.
  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::rotateLeft(const_iter it)
  {
    // Add tree left rotation, return new node in place of old node;
    return iter(&fakeRoot_);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::rotateRight(const_iter it)
  {
    // Add tree right rotation, return new node in place of old node;
    return iter(&fakeRoot_);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::rotateLargeLeft(const_iter it)
  {
    // Add tree large left rotation, return new node in place of old node;
    return iter(&fakeRoot_);
  }

  template < class Key, class Value, class Compare >
  typename BSTree< Key, Value, Compare >::iter BSTree< Key, Value, Compare >::rotateLargeRight(const_iter it)
  {
    // Add tree large right rotation, return new node in place of old node;
    return iter(&fakeRoot_);
  }

  template < class Key, class Value, class Compare >
  template < class F >
  F BSTree< Key, Value, Compare >::traverseLNR(F f) const
  {
    // TODO:: add LNR traverse with F func. It can be just with iters
    return f;
  }

  template < class Key, class Value, class Compare >
  template < class F >
  F BSTree< Key, Value, Compare >::traverseRNL(F f) const
  {
    // TODO:: add RNL traverse with F func. It can be just with iters
    return f;
  }

  template < class Key, class Value, class Compare >
  template < class F >
  F BSTree< Key, Value, Compare >::traverseBreadth(F f) const
  {
    // TODO:: add BFS traverse with F func. It can be just with iters
    return f;
  }
}

#endif
