#include "Storage.hpp"

bool khasnulin::Storage::addUser(const UserName &user, const Latent &latent)
{
  (void)user;
  (void)latent;
  return false;
}

bool khasnulin::Storage::addArticle(const ArticleName &article, const Category &category,
                                    const Difficulty &difficulty, const Latent &latent)
{
  (void)article;
  (void)category;
  (void)difficulty;
  (void)latent;
  return false;
}

// false только если пользователя нет. Повторный тег — не ошибка, счётчик
// тега при этом не растёт.
bool khasnulin::Storage::addPreference(const UserName &user, const Tag &tag)
{
  (void)user;
  (void)tag;
  return false;
}

// false, если статьи нет. Аналогично addPreference
bool khasnulin::Storage::addArticleTag(const ArticleName &article, const Tag &tag)
{
  (void)article;
  (void)tag;
  return false;
}

// false, если юзера нет. Каскадно удаляет пользователя (и чистит его ссылки на теги)
bool khasnulin::Storage::eraseUser(const UserName &user)
{
  (void)user;
  return false;
}

// false, если артикля нет. Каскадно удаляет статью (и чистит её ссылки на теги). Очистка категорий на
// articleRepo
bool khasnulin::Storage::eraseArticle(const ArticleName &article)
{
  (void)article;
  return false;
}

void khasnulin::Storage::clear()
{
}

void khasnulin::Storage::swap(Storage &rhs) noexcept
{
  (void)rhs;
}

const typename ::khasnulin::User *khasnulin::Storage::findUser(const UserName &user) const
{
  (void)user;

  return nullptr;
}

const typename ::khasnulin::Article *khasnulin::Storage::findArticle(const ArticleName &article) const
{
  (void)article;

  return nullptr;
}

typename khasnulin::Storage::count_type khasnulin::Storage::userRatingsCount(const UserName &user) const
{
  (void)user;

  return 0;
}

typename khasnulin::Storage::count_type
khasnulin::Storage::articleRatingsCount(const ArticleName &article) const
{
  (void)article;

  return 0;
}

bool khasnulin::Storage::hasRating(const UserName &user, const ArticleName &article) const
{
  (void)user;
  (void)article;

  return false;
}
