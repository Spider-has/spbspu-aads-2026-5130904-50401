#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <Vector.hpp>
#include <string>
namespace khasnulin
{
  struct Token
  {
    bool isQuoted;
    std::string text;
    Token() noexcept;
  };

  Vector< Token > tokenize(const std::string &line);
}

#endif
