#include "Tokenizer.hpp"
#include "Vector.hpp"

khasnulin::Token::Token() noexcept:
    isQuoted(false),
    text()
{
}

khasnulin::Vector< khasnulin::Token > khasnulin::tokenize(const std::string &line)
{
  (void)line;
  return Vector< Token >();
}
