#include "CommandArguments.hpp"
#include "Entities.hpp"
#include "Tokenizer.hpp"
#include "Vector.hpp"

khasnulin::ArgumentReader::ArgumentReader(const Vector< Token > &tokens):
    tokens_(tokens),
    position_(1)
{
}

khasnulin::UserName khasnulin::ArgumentReader::userName()
{
  return UserName("a");
}

khasnulin::Category khasnulin::ArgumentReader::category()
{
  return Category("a");
}

khasnulin::Difficulty khasnulin::ArgumentReader::difficulty()
{
  return Difficulty::advanced;
}

khasnulin::ArticleName khasnulin::ArgumentReader::articleName()
{
  return ArticleName("a");
}

khasnulin::Rating khasnulin::ArgumentReader::rating()
{
  return Rating(5);
}
std::size_t khasnulin::ArgumentReader::limit()
{
  return 10;
}

khasnulin::TagSet khasnulin::ArgumentReader::restTags()
{
  return TagSet();
}

std::size_t khasnulin::ArgumentReader::remaining() const noexcept
{
  return 10;
}

bool khasnulin::ArgumentReader::nextIsDifficulty() const noexcept
{
  return false;
}

void khasnulin::ArgumentReader::finish() const
{
}

const khasnulin::Token &khasnulin::ArgumentReader::next()
{
  return tokens_[0];
}
const khasnulin::Token &khasnulin::ArgumentReader::nextPlain()
{
  return tokens_[0];
}

std::size_t khasnulin::parseUnsignedNumber(const std::string &)
{
  return 0;
}
