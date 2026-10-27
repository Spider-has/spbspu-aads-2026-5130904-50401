#include "Recommender.hpp"
#include "Vector.hpp"

khasnulin::Recommender::Recommender(Scorer scorer, std::size_t recommendLimit,
                                    std::size_t similarLimit) noexcept
{
  (void)scorer;
  (void)recommendLimit;
  (void)similarLimit;
}

// возвращает вектор из top-k рекомендаций Recommendation для пользователя с UserName на основе фильтров и
// его предпочтений. порядок выдачи: от лучшего к худшему, не больше лимита;
// std::invalid_argument, если нет пользователя или категории из фильтра
khasnulin::Vector< typename khasnulin::Recommender::Recommendation >
khasnulin::Recommender::recommend(const Storage &storage, const UserName &name,
                                  const ArticleFilter &articleFilter) const
{
  (void)storage;
  (void)name;
  (void)articleFilter;
  return Vector< Recommendation >();
}

// возвращает вектор из top-k  похожих Recommendation статей на статью с переданным ArticleName. порядок
// выдачи: от лучшего к худшему, не больше лимита;
// std::invalid_argument, если нет статьи
khasnulin::Vector< typename khasnulin::Recommender::Recommendation >
khasnulin::Recommender::similar(const Storage &storage, const ArticleName &name) const
{
  (void)storage;
  (void)name;
  return Vector< Recommendation >();
}
