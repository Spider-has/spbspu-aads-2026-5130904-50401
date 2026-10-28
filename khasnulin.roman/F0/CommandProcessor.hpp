#ifndef COMMAND_PROCESSOR_HPP
#define COMMAND_PROCESSOR_HPP

#include "ArgumentReader.hpp"
#include "Model.hpp"
#include "Recommender.hpp"
#include "Storage.hpp"
#include <iosfwd>
#include <string>

namespace khasnulin
{

  class CommandProcessor
  {
  public:
    CommandProcessor(const CommandProcessor &rhs) = delete;
    CommandProcessor &operator=(const CommandProcessor &rhs) = delete;

    explicit CommandProcessor(Learner learner = Learner(), Recommender recommender = Recommender());

    void run(std::istream &input, std::ostream &output);

  private:
    using Handler = void (CommandProcessor::*)(ArgumentReader &, std::ostream &);

    Learner learner_;
    Recommender recommender_;
    Storage storage_;

    BSTree< std::string, Handler > handlers_;

    bool executeLine(const std::string &line, std::ostream &output);

    void showDifficulties(ArgumentReader &arguments, std::ostream &output);
    void addUser(ArgumentReader &arguments, std::ostream &output);
    void showUsers(ArgumentReader &arguments, std::ostream &output);
    void deleteUser(ArgumentReader &arguments, std::ostream &output);
    void showUserInfo(ArgumentReader &arguments, std::ostream &output);
    void addArticle(ArgumentReader &arguments, std::ostream &output);
    void addArticleTags(ArgumentReader &arguments, std::ostream &output);
    void showArticleInfo(ArgumentReader &arguments, std::ostream &output);
    void deleteArticle(ArgumentReader &arguments, std::ostream &output);
    void listArticles(ArgumentReader &arguments, std::ostream &output);
    void showUserRatings(ArgumentReader &arguments, std::ostream &output);
  };
}

#endif
