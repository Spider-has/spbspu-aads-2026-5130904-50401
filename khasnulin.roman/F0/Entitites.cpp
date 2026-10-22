#include "Entities.hpp"

khasnulin::Category::Category(const std::string &category):
    category_(category)
{
  // TODO: normilize
}

const std::string &khasnulin::Category::value() const noexcept
{
  return category_;
}

khasnulin::Category khasnulin::Category::fromNormalized(const std::string &normalized)
{
  Category result;
  result.category_ = normalized;
  return result;
}

khasnulin::Tag::Tag(const std::string &tag):
    tag_(tag)
{
  // TODO :normilize
}

const std::string &khasnulin::Tag::value() const noexcept
{
  return tag_;
}

bool khasnulin::Tag::operator<(const Tag &rhs) const noexcept
{
  return tag_ < rhs.tag_;
}

bool khasnulin::Tag::operator==(const Tag &rhs) const noexcept
{
  return tag_ == rhs.tag_;
}

khasnulin::User::User():
    name_(),
    preferences_(),
    latent_{}
{
  // TODO: impl
}

khasnulin::User::User(const std::string &name, const Latent &latent):
    name_(name),
    preferences_(),
    latent_(latent)
{
  // TODO: impl
}

const std::string &khasnulin::User::name() const noexcept
{
  return name_;
}

bool khasnulin::User::addPreference(const Tag &tag)
{
  bool isNew = preferences_.find(tag) != preferences_.end();
  preferences_.insert({tag, true});
  return isNew;
}

const khasnulin::TagSet &khasnulin::User::preferences() const noexcept
{
  return preferences_;
}

const khasnulin::Latent &khasnulin::User::latent() const noexcept
{
  return latent_;
}

khasnulin::Latent &khasnulin::User::latent() noexcept
{
  return latent_;
}

khasnulin::Article::Article():
    name_(),
    category_(),
    difficulty_(),
    tags_(),
    latent_{}
{
  // TODO: impl
}

khasnulin::Article::Article(const std::string &name, const Category &category, const Difficulty &difficulty,
                            const Latent &latent):
    name_(name),
    category_(category.value()),
    difficulty_(difficulty),
    tags_(),
    latent_(latent)
{
  // TODO : impl
}

const std::string &khasnulin::Article::name() const noexcept
{
  return name_;
}

khasnulin::Category khasnulin::Article::category() const
{
  return Category::fromNormalized(category_);
}

khasnulin::Difficulty khasnulin::Article::difficulty() const noexcept
{
  return difficulty_;
}

bool khasnulin::Article::addTag(const Tag &tag)
{
  bool isNew = tags_.find(tag) != tags_.end();
  tags_.insert({tag, true});
  return isNew;
}

const khasnulin::TagSet &khasnulin::Article::tags() const noexcept
{
  return tags_;
}

const khasnulin::Latent &khasnulin::Article::latent() const noexcept
{
  return latent_;
}

khasnulin::Latent &khasnulin::Article::latent() noexcept
{
  return latent_;
}

std::string khasnulin::normalizeLabel(const std::string &label)
{
  return label;
}

khasnulin::Difficulty khasnulin::parseDifficulty(const std::string &raw)
{
  (void)raw;
  return Difficulty::advanced;
}

const char *khasnulin::toString(Difficulty d) noexcept
{
  (void)d;
  return "";
}

bool khasnulin::isDifficultyName(const std::string &normalized) noexcept
{
  return normalized.empty();
}
