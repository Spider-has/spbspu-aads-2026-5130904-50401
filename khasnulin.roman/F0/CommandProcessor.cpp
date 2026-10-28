#include "CommandProcessor.hpp"
#include "ArgumentReader.hpp"
#include "Entities.hpp"
#include "Model.hpp"
#include "Recommender.hpp"
#include "Storage.hpp"
#include "Tokenizer.hpp"
#include "Vector.hpp"
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
