#define BOOST_TEST_MODULE HashTableTests
#include "HashTable.hpp"
#include <boost/test/included/unit_test.hpp>
#include <string>
#include <vector>

using namespace khasnulin;

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

std::ostream &operator<<(std::ostream &out, const Person &rhs)
{
  return out << rhs.age << ", " << rhs.name << "; ";
}

BOOST_AUTO_TEST_SUITE(hash_table_test_suite)

BOOST_AUTO_TEST_CASE(test_empty_creation)
{
  HashTable< int, std::string > ht;

  BOOST_REQUIRE(ht.empty());
  BOOST_CHECK_EQUAL(ht.size(), 0);
  BOOST_CHECK_EQUAL(ht.overflow_size(), 0);

  BOOST_CHECK(ht.has(42) == false);
}

BOOST_AUTO_TEST_CASE(test_add_and_has)
{
  HashTable< int, std::string > ht;

  ht.add(1, "one");
  ht.add(2, "two");

  BOOST_REQUIRE(ht.empty() == false);
  BOOST_CHECK_EQUAL(ht.size(), 2);

  BOOST_CHECK(ht.has(1));
  BOOST_CHECK(ht.has(2));
  BOOST_CHECK(ht.has(3) == false);
}

BOOST_AUTO_TEST_CASE(test_operator_brackets)
{
  HashTable< std::string, int > ht;

  ht["apples"] = 10;
  BOOST_CHECK_EQUAL(ht.size(), 1);
  BOOST_CHECK(ht.has("apples"));
  BOOST_CHECK_EQUAL(ht["apples"], 10);

  ht["apples"] += 5;
  BOOST_CHECK_EQUAL(ht["apples"], 15);
}

BOOST_AUTO_TEST_CASE(test_add_rvalue_perfect_forwarding)
{
  HashTable< int, Person > ht;
  Person test_person{25, "Oleg"};

  ht.add(1, std::move(test_person));

  BOOST_REQUIRE(ht.empty() == false);
  BOOST_CHECK(ht[1] == Person(25, "Oleg"));

  BOOST_CHECK(test_person.name.empty() == true);
}

BOOST_AUTO_TEST_CASE(test_update_existing_key)
{
  HashTable< int, std::string > ht;

  ht.add(1, "initial");
  BOOST_CHECK_EQUAL(ht[1], "initial");

  ht.add(1, "updated");

  BOOST_CHECK_EQUAL(ht.size(), 1);
  BOOST_CHECK_EQUAL(ht[1], "updated");
}

BOOST_AUTO_TEST_CASE(test_drop)
{
  HashTable< int, std::string > ht;

  ht.add(1, "data");

  std::string dropped_val = ht.drop(1);
  BOOST_CHECK_EQUAL(dropped_val, "data");
  BOOST_CHECK(ht.empty());

  BOOST_CHECK_THROW(ht.drop(99), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(test_overflow_trigger_and_rehash)
{
  HashTable< int, int > ht(1);
  ht.set_overflow_limit(2);

  ht.add(1, 10);
  ht.add(2, 20);
  ht.add(3, 30);
  ht.add(4, 40);

  ht.add(5, 50);
  ht.add(6, 60);

  BOOST_CHECK_THROW(ht.add(7, 70), std::overflow_error);

  BOOST_CHECK_EQUAL(ht.size(), 6);

  ht.rehash(10);

  BOOST_REQUIRE_NO_THROW(ht.add(7, 70));
  BOOST_CHECK_EQUAL(ht.size(), 7);
  BOOST_CHECK_EQUAL(ht[7], 70);
}

BOOST_AUTO_TEST_SUITE_END()
