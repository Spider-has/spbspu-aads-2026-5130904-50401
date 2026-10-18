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
  };
}

#endif
