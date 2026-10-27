#include "ArticleFilter.hpp"
#include "Entities.hpp"

khasnulin::ArticleFilter::ArticleFilter(const Category *category, const Difficulty *difficulty) noexcept
{
  (void)category;
  (void)difficulty;
}

bool khasnulin::ArticleFilter::matches(const Article &article) const
{
  (void)article;
  return false;
}

const khasnulin::Category *khasnulin::ArticleFilter::category() const noexcept
{
  return category_;
}
const khasnulin::Difficulty *khasnulin::ArticleFilter::difficulty() const noexcept
{
  return difficulty_;
}
