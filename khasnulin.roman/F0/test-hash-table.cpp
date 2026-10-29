#include "HashTable.hpp"
#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <string>

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

BOOST_AUTO_TEST_CASE(test_constructor_zero_buckets)
{
  BOOST_CHECK_THROW((HashTable< int, std::string >(0)), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_metrics_calculation)
{
  struct DummyHash
  {
    size_t operator()(int p) const
    {
      return 0;
    }
  };

  HashTable< int, int, DummyHash > ht(2);
  ht.set_overflow_limit(5);

  ht.add(1, 10);
  ht.add(2, 20);
  ht.add(3, 30);
  ht.add(4, 40);
  ht.add(5, 50);

  BOOST_CHECK_EQUAL(ht.size(), 5);
  BOOST_CHECK_EQUAL(ht.overflow_size(), 1);

  BOOST_CHECK_CLOSE(ht.average_bucket_size(), 2.0, 1e-5);
}

BOOST_AUTO_TEST_CASE(test_rehash_strong_exception_safety)
{
  HashTable< int, int > ht(5);
  ht.set_overflow_limit(2);
  for (int i = 0; i < 15; ++i)
  {
    ht.add(i, i * 10);
  }

  BOOST_CHECK_THROW(ht.rehash(1), std::overflow_error);

  BOOST_CHECK_EQUAL(ht.size(), 15);
  BOOST_CHECK(ht.has(0));
  BOOST_CHECK(ht.has(14));
}

BOOST_AUTO_TEST_CASE(test_drop_shift_logic)
{
  struct DummyHash
  {
    size_t operator()(int p) const
    {
      return 0;
    }
  };

  HashTable< int, int, DummyHash > ht(1);
  ht.add(1, 10);
  ht.add(2, 20);
  ht.add(3, 30);
  ht.add(4, 40);

  int dropped = ht.drop(2);
  BOOST_CHECK_EQUAL(dropped, 20);
  BOOST_CHECK_EQUAL(ht.size(), 3);

  BOOST_CHECK(ht.has(1));
  BOOST_CHECK(ht.has(3));
  BOOST_CHECK(ht.has(4));

  BOOST_REQUIRE_NO_THROW(ht.add(5, 50));
}

BOOST_AUTO_TEST_CASE(test_drop_from_overflow)
{
  struct DummyHash
  {
    size_t operator()(int a) const
    {
      return 0;
    }
  };

  HashTable< int, int, DummyHash > ht(1);
  ht.set_overflow_limit(2);
  for (int i = 1; i <= 5; ++i)
  {
    ht.add(i, i * 10);
  }

  BOOST_CHECK_EQUAL(ht.overflow_size(), 1);

  int dropped = ht.drop(5);
  BOOST_CHECK_EQUAL(dropped, 50);
  BOOST_CHECK_EQUAL(ht.overflow_size(), 0);
  BOOST_CHECK_EQUAL(ht.size(), 4);
}

BOOST_AUTO_TEST_CASE(test_iterator_skip_empty_buckets)
{
  HashTable< int, int > ht(10);

  ht.add(1, 10);
  ht.add(42, 420);

  int count = 0;
  for (auto it = ht.begin(); it != ht.end(); ++it)
  {
    count++;
  }

  BOOST_CHECK_EQUAL(count, 2);
}

BOOST_AUTO_TEST_CASE(test_const_iterators_and_range_for)
{
  HashTable< int, int > ht;
  ht.add(1, 100);
  ht.add(2, 200);
  ht.add(3, 300);

  const HashTable< int, int > &const_ht = ht;

  int sum = 0;

  for (const auto &item : const_ht)
  {
    sum += item.second;
  }

  BOOST_CHECK_EQUAL(sum, 600);
}

BOOST_AUTO_TEST_CASE(test_operator_brackets_creation_exception_safety)
{
  struct DummyHash
  {
    size_t operator()(int p) const
    {
      return 0;
    }
  };

  HashTable< int, std::string, DummyHash > ht(1);
  ht.set_overflow_limit(0);

  ht[1] = "A";
  ht[2] = "B";
  ht[3] = "C";
  ht[4] = "D";

  BOOST_CHECK_THROW(ht[5] = "E", std::overflow_error);
  BOOST_CHECK_EQUAL(ht.size(), 4);
  BOOST_CHECK(ht.has(5) == false);
}

BOOST_AUTO_TEST_CASE(test_find_existing_key_positive)
{
  HashTable< int, std::string > ht;
  ht.add(1, "one");
  ht.add(2, "two");

  std::string *found = ht.find(1);

  BOOST_REQUIRE(found != nullptr);
  BOOST_CHECK_EQUAL(*found, "one");
}

BOOST_AUTO_TEST_CASE(test_find_missing_key_negative)
{
  HashTable< int, std::string > ht;
  ht.add(1, "one");

  BOOST_CHECK(ht.find(42) == nullptr);
  BOOST_CHECK_EQUAL(ht.size(), 1);
  BOOST_CHECK(ht.has(42) == false);
}

BOOST_AUTO_TEST_CASE(test_find_in_empty_table_negative)
{
  HashTable< int, std::string > ht;

  BOOST_CHECK(ht.find(1) == nullptr);
  BOOST_CHECK(ht.empty());
}

BOOST_AUTO_TEST_CASE(test_find_modify_through_pointer)
{
  HashTable< std::string, int > ht;
  ht.add("apples", 10);

  int *found = ht.find("apples");
  BOOST_REQUIRE(found != nullptr);

  *found += 5;

  BOOST_CHECK_EQUAL(ht["apples"], 15);
  BOOST_CHECK_EQUAL(ht.size(), 1);
}

BOOST_AUTO_TEST_CASE(test_find_const_table)
{
  HashTable< int, int > ht;
  ht.add(1, 100);
  ht.add(2, 200);

  const HashTable< int, int > &const_ht = ht;

  const int *found = const_ht.find(2);
  BOOST_REQUIRE(found != nullptr);
  BOOST_CHECK_EQUAL(*found, 200);

  BOOST_CHECK(const_ht.find(3) == nullptr);
}

BOOST_AUTO_TEST_CASE(test_find_in_overflow)
{
  struct DummyHash
  {
    size_t operator()(int a) const
    {
      return 0;
    }
  };

  HashTable< int, int, DummyHash > ht(1);
  ht.set_overflow_limit(3);
  for (int i = 1; i <= 6; ++i)
  {
    ht.add(i, i * 10);
  }

  BOOST_REQUIRE_EQUAL(ht.overflow_size(), 2);

  const int *in_bucket = ht.find(2);
  const int *in_overflow = ht.find(6);

  BOOST_REQUIRE(in_bucket != nullptr);
  BOOST_REQUIRE(in_overflow != nullptr);
  BOOST_CHECK_EQUAL(*in_bucket, 20);
  BOOST_CHECK_EQUAL(*in_overflow, 60);
  BOOST_CHECK(ht.find(7) == nullptr);
}

BOOST_AUTO_TEST_CASE(test_find_in_overflow_after_bucket_drop)
{
  struct DummyHash
  {
    size_t operator()(int a) const
    {
      return 0;
    }
  };

  HashTable< int, int, DummyHash > ht(1);
  for (int i = 1; i <= 5; ++i)
  {
    ht.add(i, i * 10);
  }

  ht.drop(1);

  // 5 остался в overflow, хотя в бакете освободилось место
  const int *found = ht.find(5);
  BOOST_REQUIRE(found != nullptr);
  BOOST_CHECK_EQUAL(*found, 50);
}

BOOST_AUTO_TEST_CASE(test_find_after_drop_negative)
{
  HashTable< int, std::string > ht;
  ht.add(1, "one");
  ht.drop(1);

  BOOST_CHECK(ht.find(1) == nullptr);
}

BOOST_AUTO_TEST_CASE(test_find_after_rehash)
{
  HashTable< int, int > ht(2);
  for (int i = 0; i < 8; ++i)
  {
    ht.add(i, i * 10);
  }

  ht.rehash(16);

  for (int i = 0; i < 8; ++i)
  {
    const int *found = ht.find(i);
    BOOST_REQUIRE(found != nullptr);
    BOOST_CHECK_EQUAL(*found, i * 10);
  }
}

BOOST_AUTO_TEST_SUITE_END()
