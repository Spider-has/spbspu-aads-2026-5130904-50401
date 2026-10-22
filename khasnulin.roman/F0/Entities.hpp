#ifndef ENTITIES_HPP
#define ENTITIES_HPP

#include "BSTree.hpp"
#include <cstddef>
#include <string>

namespace khasnulin
{
  enum class Difficulty : unsigned char
  {
    beginner,
    intermediate,
    advanced
  };

  class Category
  {
  public:
    explicit Category(const std::string &category);
    const std::string &value() const noexcept;

  private:
    static Category fromNormalized(const std::string &normalized);
    std::string category_;
    friend class Article;
  };

  class Tag
  {
  public:
    explicit Tag(const std::string &tag);
    const std::string &value() const noexcept;

    bool operator<(const Tag &rhs) const noexcept
    {
      return tag_ < rhs.tag_;
    }

    bool operator==(const Tag &rhs) const noexcept
    {
      return tag_ == rhs.tag_;
    }

  private:
    std::string tag_;
  };

  using TagSet = BSTree< Tag, bool >;

  constexpr std::size_t latentDimensionCount = 8;

  struct Latent
  {
    double v[latentDimensionCount];
  };

  class User
  {
  public:
    // Конструктор по умолчанию только для HashTable контейнера.
    User();

    User(const std::string &name, const Latent &latent);

    const std::string &name() const noexcept;

    bool addPreference(const Tag &tag);
    const TagSet &preferences() const noexcept;

    const Latent &latent() const noexcept;
    Latent &latent() noexcept;

  private:
    std::string name_;
    TagSet preferences_;
    Latent latent_;
  };

  class Article
  {
  public:
    // только для HashTable; доменный код такие объекты не создаёт и не читает; у такой статьи пустая
    // категория, и category() вернёт невалидную Category
    Article();

    Article(const std::string &name, const Category &category, const Difficulty &difficulty,
            const Latent &latent);

    const std::string &name() const noexcept;
    Category category() const;
    Difficulty difficulty() const noexcept;

    bool addTag(const Tag &tag);
    const TagSet &tags() const noexcept;

    const Latent &latent() const noexcept;
    Latent &latent() noexcept;

  private:
    std::string name_;
    std::string category_;
    Difficulty difficulty_;
    TagSet tags_;
    Latent latent_;
  };

  // приводит строку к единому формату: строго слова, разделенные одним пробелом
  // словом считается любая последовательность букв, чисел и специсимволов(@, +, -, ~ и etc)
  // правила: используются строго латинские буквы, в строке должна быть хотя бы одна буква. допустимы только
  // печатные ASCII-символы (0x20–0x7E), кроме ".
  // все лишние пробелы удаляются строка всегда приводится к нижнему регистру если в одно из правил
  // нарушено -- бросаем std::invalid_argument
  std::string normalizeLabel(const std::string &label);

  // нормализует raw и сверяет по спец. значениям difficulty, если ничего не подошло -- бросает std::invalid
  // argument
  Difficulty parseDifficulty(const std::string &raw);

  // Перевод из difficulty в строку. Варианты строк: "beginner" / "intermediate" / "advanced"
  const char *toString(Difficulty d) noexcept;

  // Нужна, чтобы при вводе категории не было конфликта или путаницы с сложностью. Правило для названия
  // категории: имя категории не должно совпадать с спец именами использующимися в системе. В данном случае
  // сверяем с спец. именами для уровней сложности
  bool isDifficultyName(const std::string &normalized) noexcept;
}

#endif
