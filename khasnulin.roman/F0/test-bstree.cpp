#include <boost/test/tools/old/interface.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>

#include "BSTree.hpp"

using namespace khasnulin;

BOOST_AUTO_TEST_SUITE(EmptyTreeSuite)

BOOST_AUTO_TEST_CASE(DefaultConstructorCreatesEmptyTree)
{
  BSTree< int, std::string > tree;
  BOOST_TEST(tree.empty());
  BOOST_TEST(tree.size() == 0);
  BOOST_TEST(tree.height() == 0);
  BOOST_TEST((tree.begin() == tree.end()));
  BOOST_TEST((tree.cbegin() == tree.cend()));
}

BOOST_AUTO_TEST_CASE(LookupInEmptyTree)
{
  BSTree< int, std::string > tree;
  BOOST_TEST((tree.find(42) == tree.end()));

  const auto &ctree = tree;
  BOOST_TEST((ctree.find(42) == ctree.cend()));

  BOOST_CHECK_THROW(tree.get(42), std::out_of_range);
  BOOST_CHECK_THROW(ctree.get(42), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(ClearAndEraseOnEmptyTree)
{
  BSTree< int, std::string > tree;
  BOOST_CHECK_NO_THROW(tree.clear());
  BOOST_CHECK_NO_THROW(tree.erase(10));
  BOOST_TEST(tree.empty());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(InsertAndLookupSuite)

BOOST_AUTO_TEST_CASE(InsertSingleAndMultipleElements)
{
  BSTree< int, std::string > tree;

  std::pair< const int, std::string > lval_pair{10, "ten"};
  tree.insert(lval_pair);
  BOOST_TEST(!tree.empty());
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree.height() == 1);
  BOOST_TEST(tree.get(10) == "ten");

  tree.insert({5, "five"});
  tree.insert({15, "fifteen"});
  tree.insert({13, "thirteen"});

  BOOST_TEST(tree.size() == 4);
  BOOST_TEST(tree.height() == 3);
  BOOST_TEST(tree.get(5) == "five");
  BOOST_TEST(tree.get(15) == "fifteen");
  BOOST_TEST(tree.get(13) == "thirteen");
}

BOOST_AUTO_TEST_CASE(InsertDuplicateKeyDoesNotOverwrite)
{
  BSTree< int, std::string > tree;
  tree.insert({10, "original"});
  tree.insert({10, "duplicate"});

  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree.get(10) == "original");
}

BOOST_AUTO_TEST_CASE(SubscriptOperatorCreatesAndModifies)
{
  BSTree< int, std::string > tree;

  tree[20] = "twenty";
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree.get(20) == "twenty");

  tree[20] = "modified";
  BOOST_TEST(tree.size() == 1);
  BOOST_TEST(tree.get(20) == "modified");

  BOOST_TEST(tree[30] == "");
  BOOST_TEST(tree.size() == 2);
}

BOOST_AUTO_TEST_CASE(FindAndConstGet)
{
  BSTree< int, int > tree;
  tree.insert({10, 100});
  tree.insert({5, 50});
  tree.insert({15, 150});

  auto it = tree.find(5);
  BOOST_TEST((it != tree.end()));
  BOOST_TEST(it->first == 5);
  BOOST_TEST(it->second == 50);

  it->second = 55;
  BOOST_TEST(tree.get(5) == 55);
  tree.get(5) = 60;

  const auto &ctree = tree;
  BOOST_TEST(ctree.get(5) == 60);
  BOOST_TEST((ctree.find(999) == ctree.cend()));
  BOOST_CHECK_THROW(ctree.get(999), std::out_of_range);
}

BOOST_AUTO_TEST_CASE(CustomComparatorDescendingOrder)
{
  BSTree< int, std::string, std::greater< int > > tree;
  tree.insert({10, "ten"});
  tree.insert({20, "twenty"});
  tree.insert({5, "five"});

  BOOST_TEST(tree.begin()->first == 20);
}

BOOST_AUTO_TEST_SUITE_END()
