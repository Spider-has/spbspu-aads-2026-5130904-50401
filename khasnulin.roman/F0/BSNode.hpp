#ifndef BSNODE_HPP
#define BSNODE_HPP

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

  template < class NodePtr > NodePtr nextNode(NodePtr node) noexcept
  {
    // TODO: get next node;
    return nullptr;
  }

  template < class NodePtr > NodePtr prevNode(NodePtr node) noexcept
  {
    // TODO: get prev node;
    return nullptr;
  }

  template < class NodePtr > NodePtr minNode(NodePtr node) noexcept
  {
    // TODO: get most left node;
    return nullptr;
  }

  template < class NodePtr > NodePtr maxNode(NodePtr node) noexcept
  {
    // TODO: get most right node;
    return nullptr;
  }
}

#endif
