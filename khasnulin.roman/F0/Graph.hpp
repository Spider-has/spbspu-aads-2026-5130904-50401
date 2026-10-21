#ifndef GRAPH_HPP
#define GRAPH_HPP

#include "AutoHashTable.hpp"
#include "BSTree.hpp"

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
    using UserIndex = AutoHashTable< UserKey, BSTree< ArticleKey, Weight > >;
    using ArticleIndex = AutoHashTable< ArticleKey, BSTree< UserKey, Weight > >;

    UserIndex byUser_;
    ArticleIndex byArticle_;
    size_type edges_;
  };
}

#endif
