#include "CommandProcessor.hpp"
#include "ArgumentReader.hpp"
#include "ArticleFilter.hpp"
#include "Entities.hpp"
#include "Model.hpp"
#include "Recommender.hpp"
#include "Storage.hpp"
#include "Tokenizer.hpp"
#include "Vector.hpp"
#include <cstddef>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>

khasnulin::CommandProcessor::CommandProcessor(Learner learner, Recommender recommender):
    learner_(learner),
    recommender_(recommender),
    storage_(),
    handlers_()
{
  handlers_.insert({"show-difficulties", &CommandProcessor::showDifficulties});
  handlers_.insert({"add-user", &CommandProcessor::addUser});
  handlers_.insert({"show-users", &CommandProcessor::showUsers});
  handlers_.insert({"delete-user", &CommandProcessor::deleteUser});
  handlers_.insert({"show-user-info", &CommandProcessor::showUserInfo});
  handlers_.insert({"add-article", &CommandProcessor::addArticle});
  handlers_.insert({"add-article-tags", &CommandProcessor::addArticleTags});
  handlers_.insert({"delete-article", &CommandProcessor::deleteArticle});
  handlers_.insert({"list-articles", &CommandProcessor::listArticles});
  handlers_.insert({"show-article-info", &CommandProcessor::showArticleInfo});
  handlers_.insert({"show-user-ratings", &CommandProcessor::showUserRatings});
  handlers_.insert({"rate-article", &CommandProcessor::rateArticle});

  handlers_.insert({"show-tags", &CommandProcessor::showTags});
  handlers_.insert({"show-article-categories", &CommandProcessor::showArticleCategories});

  handlers_.insert({"recommend", &CommandProcessor::recommend});
  handlers_.insert({"recommend-similar", &CommandProcessor::similar});
}

void khasnulin::CommandProcessor::run(std::istream &input, std::ostream &output)
{
  bool isExitCmd = false;
  std::string line;
  while (!isExitCmd && std::getline(input, line))
  {
    try
    {
      isExitCmd = executeLine(line, output);
    }
    catch (const std::invalid_argument &)
    {
      output << "<INVALID COMMAND>\n";
    }
  }
}

bool khasnulin::CommandProcessor::executeLine(const std::string &line, std::ostream &output)
{
  Vector< Token > tokens = tokenize(line);
  if (tokens.isEmpty())
  {
    return false;
  }
  if (tokens[0].isQuoted)
  {
    throw std::invalid_argument("command can't be in quoutes");
  }
  ArgumentReader args(tokens);

  if (tokens[0].text == "exit")
  {
    args.finish();
    return true;
  }

  auto it = handlers_.find(tokens[0].text);
  if (it == handlers_.end())
  {
    throw std::invalid_argument("no such command");
  }

  Handler handler = it->second;
  (this->*handler)(args, output);

  return false;
}

void khasnulin::CommandProcessor::showDifficulties(ArgumentReader &arguments, std::ostream &output)
{
  arguments.finish();
  output << toString(Difficulty::beginner) << "\n"
         << toString(Difficulty::intermediate) << "\n"
         << toString(Difficulty::advanced) << "\n";
}

void khasnulin::CommandProcessor::addUser(ArgumentReader &arguments, std::ostream & /*unused*/)
{
  UserName name = arguments.userName();
  TagSet preferences = arguments.restTags();
  Latent latent = initialLatent(name);
  arguments.finish();

  if (!storage_.addUser(name, latent))
  {
    throw std::invalid_argument("user already exists");
  }

  try
  {
    for (const auto &tag : preferences)
    {
      storage_.addPreference(name, tag.first);
    }
  }
  catch (...)
  {
    storage_.eraseUser(name);
    throw;
  }
}

void khasnulin::CommandProcessor::showUsers(ArgumentReader &arguments, std::ostream &output)
{
  std::size_t total = storage_.usersCount();
  std::size_t limit = total;
  if (arguments.remaining() > 0)
  {
    limit = arguments.limit();
  }
  arguments.finish();
  std::size_t curr = 0;
  storage_.traverseUsers(
      [&](const User &user)
      {
        if (curr < limit)
        {
          output << user.name().value() << "\n";
          ++curr;
        }
      });
}

void khasnulin::CommandProcessor::deleteUser(ArgumentReader &arguments, std::ostream & /*unused*/)
{
  UserName name = arguments.userName();
  arguments.finish();
  if (!storage_.eraseUser(name))
  {
    throw std::invalid_argument("user with such name not found");
  }
}

void khasnulin::CommandProcessor::showUserInfo(ArgumentReader &arguments, std::ostream &output)
{
  UserName name = arguments.userName();
  arguments.finish();
  const User *user = storage_.findUser(name);
  if (!user)
  {
    throw std::invalid_argument("user with such name not found");
  }
  output << "User: " << user->name().value() << "\nPreferences:";
  user->preferences().traverseLNR([&](const auto &preference) { output << " " << preference.first.value(); });
  output << "\nRatings count: " << storage_.userRatingsCount(name) << "\n";
}

void khasnulin::CommandProcessor::addArticle(ArgumentReader &arguments, std::ostream & /*unused*/)
{
  ArticleName name = arguments.articleName();
  Category category = arguments.category();
  Difficulty difficulty = arguments.difficulty();
  Latent latent = initialLatent(name);
  TagSet tags = arguments.restTags();

  arguments.finish();
  if (!storage_.addArticle(name, category, difficulty, latent))
  {
    throw std::invalid_argument("article already exists");
  }
  try
  {
    for (const auto &tag : tags)
    {
      storage_.addArticleTag(name, tag.first);
    }
  }
  catch (...)
  {
    storage_.eraseArticle(name);
    throw;
  }
}

void khasnulin::CommandProcessor::addArticleTags(ArgumentReader &arguments, std::ostream & /*unused*/)
{
  ArticleName name = arguments.articleName();
  if (arguments.remaining() == 0)
  {
    throw std::invalid_argument("list of tags can't be empty");
  }
  TagSet tags = arguments.restTags();
  arguments.finish();
  if (!storage_.findArticle(name))
  {
    throw std::invalid_argument("article with such title not found");
  }
  tags.traverseLNR([&](const auto &tag) { storage_.addArticleTag(name, tag.first); });
}

void khasnulin::CommandProcessor::showArticleInfo(ArgumentReader &arguments, std::ostream &output)
{
  ArticleName name = arguments.articleName();
  arguments.finish();
  const Article *article = storage_.findArticle(name);
  if (!article)
  {
    throw std::invalid_argument("article with such title not found");
  }
  const Category &category = article->category();
  Difficulty difficulty = article->difficulty();
  std::size_t ratingsCount = 0;
  std::size_t sum = 0;
  storage_.traverseArticleRatings(name,
                                  [&](const User &, const Rating &rating)
                                  {
                                    sum += rating.value();
                                    ++ratingsCount;
                                  });
  std::size_t tenths = 0;
  if (ratingsCount > 0)
  {
    tenths = (20 * sum + ratingsCount) / (2 * ratingsCount);
  }
  output << "Article: \"" << name.value() << "\"\n";
  output << "Category: " << category.value() << " | Difficulty: " << toString(difficulty) << "\n";
  output << "Tags:";
  article->tags().traverseLNR([&](const auto &tag) { output << " " << tag.first.value(); });

  output << "\nAvg Rating: " << tenths / 10 << '.' << tenths % 10 << "/" << Rating::maxRating << " ("
         << ratingsCount << " ratings)\n";
}

void khasnulin::CommandProcessor::deleteArticle(ArgumentReader &arguments, std::ostream & /*unused*/)
{
  ArticleName name = arguments.articleName();
  arguments.finish();
  if (!storage_.eraseArticle(name))
  {
    throw std::invalid_argument("article with such title not found");
  }
}

void khasnulin::CommandProcessor::listArticles(ArgumentReader &arguments, std::ostream &output)
{
  std::size_t total = storage_.articlesCount();
  std::size_t limit = total;
  if (arguments.remaining() > 0)
  {
    limit = arguments.limit();
  }
  arguments.finish();
  std::size_t curr = 0;
  storage_.traverseArticles(
      [&](const Article &article)
      {
        if (curr < limit)
        {
          output << "\"" << article.name().value() << "\" [" << article.category().value() << " | "
                 << toString(article.difficulty()) << "]\n";
          ++curr;
        }
      });
}

void khasnulin::CommandProcessor::showUserRatings(ArgumentReader &arguments, std::ostream &output)
{
  UserName name = arguments.userName();
  arguments.finish();
  if (!storage_.findUser(name))
  {
    throw std::invalid_argument("user with such name not found");
  }
  storage_.traverseUserRatings(name,
                               [&](const Article &article, const Rating &rating)
                               {
                                 output << "\"" << article.name().value() << "\" - " << rating.value() << "/"
                                        << Rating::maxRating << "\n";
                               });
}

void khasnulin::CommandProcessor::rateArticle(ArgumentReader &arguments, std::ostream & /*unused*/)
{
  UserName userName = arguments.userName();
  ArticleName articleName = arguments.articleName();
  Rating rating = arguments.rating();

  arguments.finish();
  if (!storage_.findUser(userName) || !storage_.findArticle(articleName))
  {
    throw std::invalid_argument("incorrect username or article title");
  }

  storage_.rate(userName, articleName, rating, learner_);
}

void khasnulin::CommandProcessor::showTags(ArgumentReader &arguments, std::ostream &output)
{
  arguments.finish();
  storage_.traverseTags([&](const Tag &tag, std::size_t) { output << tag.value() << "\n"; });
}

void khasnulin::CommandProcessor::showArticleCategories(ArgumentReader &arguments, std::ostream &output)
{
  arguments.finish();
  storage_.traverseCategories([&](const Category &category, std::size_t)
                              { output << category.value() << "\n"; });
}

void khasnulin::CommandProcessor::similar(ArgumentReader &arguments, std::ostream &output)
{
  ArticleName article = arguments.articleName();
  arguments.finish();
  if (storage_.findArticle(article))
  {
    throw std::invalid_argument("article with such title not found");
  }
  Vector< Recommender::Recommendation > sims = recommender_.similar(storage_, article);
  for (const auto &recommendation : sims)
  {
    output << "\"" << recommendation.article->name().value() << "\"\n";
  }
}

void khasnulin::CommandProcessor::recommend(ArgumentReader &arguments, std::ostream &output)
{
  UserName name = arguments.userName();
  if (arguments.remaining() > 0)
  {
    if (arguments.nextIsDifficulty())
    {
      Difficulty difficulty = arguments.difficulty();
      recommendWithFilter(arguments, output, name, nullptr, &difficulty);
    }
    else
    {
      Category category = arguments.category();
      if (arguments.remaining() > 0)
      {
        Difficulty difficulty = arguments.difficulty();
        recommendWithFilter(arguments, output, name, &category, &difficulty);
      }
      else
      {
        recommendWithFilter(arguments, output, name, &category, nullptr);
      }
    }
  }
  else
  {
    recommendWithFilter(arguments, output, name, nullptr, nullptr);
  }
}

void khasnulin::CommandProcessor::recommendWithFilter(ArgumentReader &arguments, std::ostream &output,
                                                      const UserName &name, Category *category,
                                                      Difficulty *difficulty)
{
  arguments.finish();
  if (!storage_.findUser(name))
  {
    throw std::invalid_argument("user with such name not found");
  }
  ArticleFilter filter(category, difficulty);
  Vector< Recommender::Recommendation > recs = recommender_.recommend(storage_, name, filter);
  for (const auto &rec : recs)
  {
    output << "\"" << rec.article->name().value() << "\"\n";
  }
}
