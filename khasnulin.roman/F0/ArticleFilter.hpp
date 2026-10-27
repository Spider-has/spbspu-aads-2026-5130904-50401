#ifndef ARTICLE_FILTER_HPP
#define ARTICLE_FILTER_HPP

#include "Entities.hpp"

namespace khasnulin
{
  class ArticleFilter
  {
  public:
    // если пробросить nullptr -- фильтр отменяется, по критерию пропускаются все статьи.
    ArticleFilter(const Category *category = nullptr, const Difficulty *difficulty = nullptr) noexcept;

    bool matches(const Article &article) const;

    const Category *category() const noexcept;
    const Difficulty *difficulty() const noexcept;

  private:
    const Category *category_;
    const Difficulty *difficulty_;
  };
}
#endif
