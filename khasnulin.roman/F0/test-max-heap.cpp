#include "MaxHeap.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <stdexcept>

using namespace khasnulin;

BOOST_AUTO_TEST_SUITE(max_heap_test_suite)

BOOST_AUTO_TEST_CASE(test_empty_creation_positive)
{
  MaxHeap< int, std::less< int > > heap;

  BOOST_CHECK(heap.empty() == true);
  BOOST_CHECK(heap.size() == 0);
}

BOOST_AUTO_TEST_CASE(test_empty_creation_get_top_negaitive)
{
  MaxHeap< int, std::less< int > > heap;

  BOOST_CHECK_THROW(heap.top(), std::out_of_range);
  BOOST_CHECK_THROW(heap.pop(), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_push_element_positive)
{
  MaxHeap< int, std::less< int > > heap;

  heap.push(5);

  BOOST_CHECK(!heap.empty());
  BOOST_CHECK_EQUAL(heap.top(), 5);
  BOOST_CHECK_EQUAL(heap.size(), 1);
}

BOOST_AUTO_TEST_CASE(test_push_decreasing_sequence_positive)
{
  MaxHeap< int, std::less< int > > heap;

  heap.push(5);
  heap.push(4);
  heap.push(3);

  BOOST_CHECK(!heap.empty());
  BOOST_CHECK_EQUAL(heap.top(), 5);
  BOOST_CHECK_EQUAL(heap.size(), 3);

  heap.pop();
  BOOST_CHECK_EQUAL(heap.top(), 4);
  BOOST_CHECK_EQUAL(heap.size(), 2);

  heap.pop();
  BOOST_CHECK_EQUAL(heap.top(), 3);
  BOOST_CHECK_EQUAL(heap.size(), 1);

  heap.pop();
  BOOST_CHECK_EQUAL(heap.size(), 0);
  BOOST_CHECK_THROW(heap.top(), std::out_of_range);
  BOOST_CHECK_THROW(heap.pop(), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_push_increasing_sequence_positive)
{
  MaxHeap< int, std::less< int > > heap;

  heap.push(1);
  BOOST_CHECK(!heap.empty());
  BOOST_CHECK_EQUAL(heap.top(), 1);
  BOOST_CHECK_EQUAL(heap.size(), 1);

  heap.push(2);
  BOOST_CHECK_EQUAL(heap.top(), 2);
  BOOST_CHECK_EQUAL(heap.size(), 2);

  heap.push(3);
  BOOST_CHECK_EQUAL(heap.top(), 3);
  BOOST_CHECK_EQUAL(heap.size(), 3);

  heap.push(4);
  BOOST_CHECK_EQUAL(heap.top(), 4);
  BOOST_CHECK_EQUAL(heap.size(), 4);
}

BOOST_AUTO_TEST_CASE(test_push_dublicate_sequence_positive)
{
  MaxHeap< int, std::less< int > > heap;

  heap.push(5);
  BOOST_CHECK(!heap.empty());
  BOOST_CHECK_EQUAL(heap.top(), 5);
  BOOST_CHECK_EQUAL(heap.size(), 1);

  heap.push(5);
  BOOST_CHECK_EQUAL(heap.top(), 5);
  BOOST_CHECK_EQUAL(heap.size(), 2);

  heap.push(5);
  BOOST_CHECK_EQUAL(heap.top(), 5);
  BOOST_CHECK_EQUAL(heap.size(), 3);
}

BOOST_AUTO_TEST_CASE(test_pop_sequence_positive)
{
  MaxHeap< int, std::less< int > > heap;

  heap.push(10);
  heap.push(15);
  BOOST_CHECK(!heap.empty());
  BOOST_CHECK_EQUAL(heap.top(), 15);
  BOOST_CHECK_EQUAL(heap.size(), 2);

  heap.pop();
  BOOST_CHECK_EQUAL(heap.top(), 10);
  BOOST_CHECK_EQUAL(heap.size(), 1);

  heap.push(12);
  BOOST_CHECK_EQUAL(heap.top(), 12);
  BOOST_CHECK_EQUAL(heap.size(), 2);

  heap.pop();
  heap.pop();

  BOOST_CHECK(heap.empty());
  BOOST_CHECK_EQUAL(heap.size(), 0);
}

struct Person
{
  size_t age;
  std::string name;
  Person():
      age(0),
      name("unknown")
  {
  }
  Person(size_t a):
      age(a),
      name("unknown")
  {
  }
  Person(const std::string &n):
      age(0),
      name(n)
  {
  }
  Person(size_t a, const std::string &n):
      age(a),
      name(n)
  {
  }

  bool operator==(const Person &rhs) const
  {
    return age == rhs.age && name == rhs.name;
  }
  bool operator!=(const Person &rhs) const
  {
    return !(*this == rhs);
  }
};

struct PersonComparator
{
  bool operator()(const Person &left, const Person &right)
  {
    return left.name < right.name;
  };
};

struct PersonInversedComparator
{
  bool operator()(const Person &left, const Person &right)
  {
    return left.name > right.name;
  };
};

std::ostream &operator<<(std::ostream &out, const Person &rhs)
{
  return out << rhs.age << ", " << rhs.name << "; ";
}

BOOST_AUTO_TEST_CASE(test_person_heap_custom_comparator_positive)
{
  MaxHeap< Person, PersonComparator > heap;

  heap.push(Person("Alex"));
  BOOST_CHECK(!heap.empty());
  BOOST_CHECK_EQUAL(heap.top(), Person("Alex"));
  BOOST_CHECK_EQUAL(heap.size(), 1);

  heap.push(Person("Ivan"));
  heap.push(Person("Roman"));
  heap.push(Person("Vasiliy"));
  heap.push(Person("ZZZZZZZZ"));
  BOOST_CHECK_EQUAL(heap.top(), Person("ZZZZZZZZ"));
  BOOST_CHECK_EQUAL(heap.size(), 5);

  heap.pop();
  BOOST_CHECK_EQUAL(heap.top(), Person("Vasiliy"));
  BOOST_CHECK_EQUAL(heap.size(), 4);
}

BOOST_AUTO_TEST_CASE(test_person_heap_custom_inversed_comparator_positive)
{
  MaxHeap< Person, PersonInversedComparator > heap;

  heap.push(Person("Alex"));
  BOOST_CHECK(!heap.empty());
  BOOST_CHECK_EQUAL(heap.top(), Person("Alex"));
  BOOST_CHECK_EQUAL(heap.size(), 1);

  heap.push(Person("Ivan"));
  heap.push(Person("Roman"));
  heap.push(Person("Vasiliy"));
  heap.push(Person("AAAAAAA"));
  BOOST_CHECK_EQUAL(heap.top(), Person("AAAAAAA"));
  BOOST_CHECK_EQUAL(heap.size(), 5);

  heap.pop();
  BOOST_CHECK_EQUAL(heap.top(), Person("Alex"));
  BOOST_CHECK_EQUAL(heap.size(), 4);
}

BOOST_AUTO_TEST_CASE(test_swap_heaps_positive)
{
  MaxHeap< int, std::less< int > > heap_A;
  MaxHeap< int, std::less< int > > heap_B;

  heap_A.push(1);
  heap_A.push(2);
  heap_A.push(3);

  heap_A.swap(heap_B);

  BOOST_CHECK(heap_A.empty());
  BOOST_CHECK_EQUAL(heap_A.size(), 0);

  BOOST_CHECK(!heap_B.empty());
  BOOST_CHECK_EQUAL(heap_B.size(), 3);
  BOOST_CHECK_EQUAL(heap_B.top(), 3);
}

BOOST_AUTO_TEST_SUITE_END()
