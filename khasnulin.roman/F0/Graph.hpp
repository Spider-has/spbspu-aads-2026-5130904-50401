#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "AutoHashTable.hpp"
#include "BSTree.hpp"
#include <memory>

namespace khasnulin
{
  template < class UserKey, class ArticleKey, class Weight > class Graph
  {
  public:
    using size_type = std::size_t;

    Graph();

    void clear();
    void swap(Graph &rhs) noexcept;

    bool rate(const UserKey &user, const ArticleKey &article, const Weight &rating);

    bool hasRating(const UserKey &user, const ArticleKey &article) const;
    const Weight *findRating(const UserKey &user, const ArticleKey &article) const;

    size_type userRatingsCount(const UserKey &user) const;
    size_type articleRatingsCount(const ArticleKey &article) const;

    size_type ratingsCount() const noexcept;
    bool empty() const noexcept;

    bool eraseRating(const UserKey &user, const ArticleKey &article);
    size_type eraseUser(const UserKey &user);
    size_type eraseArticle(const ArticleKey &article);

    template < class F > F traverseUserRatings(const UserKey &user, F f) const;
    template < class F > F traverseArticleRatings(const ArticleKey &article, F f) const;
    template < class F > F traverseAllRatings(F f) const;

  private:
    using UserTree = BSTree< ArticleKey, Weight >;
    using ArticleTree = BSTree< UserKey, Weight >;
    using UserIndex = AutoHashTable< UserKey, UserTree >;
    using ArticleIndex = AutoHashTable< ArticleKey, ArticleTree >;

    UserIndex byUser_;
    ArticleIndex byArticle_;
    size_type edges_;
  };

  template < class UserKey, class ArticleKey, class Weight >
  Graph< UserKey, ArticleKey, Weight >::Graph():
      byUser_(),
      byArticle_(),
      edges_(0)
  {
  }

  template < class UserKey, class ArticleKey, class Weight >
  void Graph< UserKey, ArticleKey, Weight >::clear()
  {
    Graph tmp;
    swap(tmp);
  }

  template < class UserKey, class ArticleKey, class Weight >
  void Graph< UserKey, ArticleKey, Weight >::swap(Graph &rhs) noexcept
  {
    std::swap(byUser_, rhs.byUser_);
    std::swap(byArticle_, rhs.byArticle_);
    std::swap(edges_, rhs.edges_);
  }

  template < class UserKey, class ArticleKey, class Weight >
  bool Graph< UserKey, ArticleKey, Weight >::rate(const UserKey &user, const ArticleKey &article,
                                                  const Weight &rating)
  {

    UserTree *userTree = byUser_.find(user);
    ArticleTree *articleTree = byArticle_.find(article);
    if (userTree && articleTree)
    {
      auto userIt = userTree->find(article);
      auto articleIt = articleTree->find(user);
      if (userIt != userTree->end() && articleIt != articleTree->end())
      {
        userIt->second = rating;
        articleIt->second = rating;
        return false;
      }
    }

    const bool newUser = (userTree == nullptr);
    const bool newArticle = (articleTree == nullptr);

    if (newUser)
    {
      UserTree tmp;
      tmp.insert({article, rating});
      byUser_.add(user, std::move(tmp));
    }
    else
    {
      userTree->insert({article, rating});
    }
    try
    {
      if (newArticle)
      {
        ArticleTree tmp;
        tmp.insert({user, rating});
        byArticle_.add(article, std::move(tmp));
      }
      else
      {
        articleTree->insert({user, rating});
      }
    }
    catch (...)
    {
      if (newUser)
      {
        byUser_.erase(user);
      }
      else
      {
        userTree->erase(article);
      }
      throw;
    }

    ++edges_;
    return true;
  }

  bool hasRating(const UserKey &user, const ArticleKey &article) const;
  const Weight *findRating(const UserKey &user, const ArticleKey &article) const;

  size_type userRatingsCount(const UserKey &user) const;
  size_type articleRatingsCount(const ArticleKey &article) const;

  size_type ratingsCount() const noexcept;
  bool empty() const noexcept;

  bool eraseRating(const UserKey &user, const ArticleKey &article);
  size_type eraseUser(const UserKey &user);
  size_type eraseArticle(const ArticleKey &article);

  template < class F > F traverseUserRatings(const UserKey &user, F f) const;
  template < class F > F traverseArticleRatings(const ArticleKey &article, F f) const;
  template < class F > F traverseAllRatings(F f) const;

}

#endif
