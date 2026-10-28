#include "CommandProcessor.hpp"
#include "Model.hpp"
#include "Recommender.hpp"
#include "Storage.hpp"

khasnulin::CommandProcessor::CommandProcessor(Learner learner, Recommender recommender):
    learner_(learner),
    recommender_(recommender),
    storage_(),
    handlers_()
{
}

void khasnulin::CommandProcessor::run(std::istream &input, std::ostream &output)
{
  (void)input;
  (void)output;
}

bool khasnulin::CommandProcessor::executeLine(const std::string &line, std::ostream &output)
{
  (void)line;
  (void)output;
  return false;
}

void khasnulin::CommandProcessor::showDifficulties(ArgumentReader &arguments, std::ostream &output)
{
  (void)arguments;
  (void)output;
}
