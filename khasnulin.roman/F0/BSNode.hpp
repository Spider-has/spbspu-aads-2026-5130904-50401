#ifndef BSNODE_HPP
#define BSNODE_HPP

#include <cassert>
#include <utility>
namespace khasnulin
{
  template < class Key, class Value > struct Node
  {
    using value_pair = std::pair< const Key, Value >;

    Node *parent;
    Node *left;
    Node *right;
    value_pair val;

    Node();

    Node(const value_pair &rhs);
    Node(value_pair &&rhs);

    Node(const Node &) = delete;
    Node &operator=(const Node &) = delete;
  };

  template < class Key, class Value >
  Node< Key, Value >::Node():
      parent(nullptr),
      left(nullptr),
      right(nullptr),
      val()
  {
  }

  template < class Key, class Value >
  Node< Key, Value >::Node(const value_pair &rhs):
      parent(nullptr),
      left(nullptr),
      right(nullptr),
      val(rhs)
  {
  }

  template < class Key, class Value >
  Node< Key, Value >::Node(value_pair &&rhs):
      parent(nullptr),
      left(nullptr),
      right(nullptr),
      val(std::move(rhs))
  {
  }

  template < class NodePtr > NodePtr nextNode(NodePtr node) noexcept
  {
    assert(node != nullptr && "node can't be nullptr");
    if (node->right)
    {
      return minNode(node->right);
    }
    NodePtr prev = node;
    node = node->parent;
    while (node && node->right == prev)
    {
      prev = node;
      node = node->parent;
    }
    return node;
  }

  template < class NodePtr > NodePtr prevNode(NodePtr node) noexcept
  {
    assert(node != nullptr && "node can't be nullptr");
    if (node->left)
    {
      return maxNode(node->left);
    }
    NodePtr prev = node;
    node = node->parent;
    while (node && node->left == prev)
    {
      prev = node;
      node = node->parent;
    }
    return node;
  }

  template < class NodePtr > NodePtr minNode(NodePtr node) noexcept
  {
    assert(node != nullptr && "node can't be nullptr");
    while (node->left)
    {
      node = node->left;
    }
    return node;
  }

  template < class NodePtr > NodePtr maxNode(NodePtr node) noexcept
  {
    assert(node != nullptr && "node can't be nullptr");
    while (node->right)
    {
      node = node->right;
    }
    return node;
  }
}

#endif
