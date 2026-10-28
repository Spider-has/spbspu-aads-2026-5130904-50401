#include "CommandProcessor.hpp"
#include "ArgumentReader.hpp"
#include "Entities.hpp"
#include "Model.hpp"
#include "Recommender.hpp"
#include "RefCountIndex.hpp"
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
  storage_.addUser(name, latent);
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
  std::size_t limit = arguments.limit();
  arguments.finish();
  std::size_t total = storage_.usersCount();
  if (limit > total)
  {
    throw std::invalid_argument("limit can't be higher than total count of users");
  }
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
  storage_.eraseUser(name);
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
  arguments.finish();
  storage_.addArticle(name, category, difficulty, latent);
}

void khasnulin::CommandProcessor::addArticleTags(ArgumentReader &arguments, std::ostream & /*unused*/)
{
  ArticleName name = arguments.articleName();
  TagSet tags = arguments.restTags();
  arguments.finish();
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
  double sum = 0;
  storage_.traverseArticleRatings(name,
                                  [&](const User &, const Rating &rating)
                                  {
                                    sum += rating.value();
                                    ++ratingsCount;
                                  });
  double avg = sum / static_cast< double >(ratingsCount) / 10.0;
  output << "Article: \"" << name.value() << "\"\n";
  output << "Category: " << category.value() << " | " << toString(difficulty) << "\n";
  output << "Tags:";
  article->tags().traverseLNR([&](const auto &tag) { output << " " << tag.first.value(); });

  output << "\nAvg Rating: " << avg << "/" << Rating::maxRating << " (" << ratingsCount << " ratings)\n";
}

void khasnulin::CommandProcessor::deleteArticle(ArgumentReader &arguments, std::ostream & /*unused*/)
{
  ArticleName name = arguments.articleName();
  arguments.finish();
  storage_.eraseArticle(name);
}

void khasnulin::CommandProcessor::listArticles(ArgumentReader &arguments, std::ostream &output)
{
  std::size_t limit = arguments.limit();
  arguments.finish();
  std::size_t total = storage_.articlesCount();
  if (limit > total)
  {
    throw std::invalid_argument("limit can't be higher than total count of articles");
  }
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
  storage_.traverseUserRatings(name,
                               [&](const Article &article, const Rating &rating)
                               {
                                 output << "\"" << article.name().value() << "\" - " << rating.value() << "/"
                                        << Rating::maxRating << "\n";
                               });
}
