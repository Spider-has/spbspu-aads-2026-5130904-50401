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
    Category() = default;
    static Category fromNormalized(const std::string &normalized);

    std::string category_;
    friend class Article;
  };

  class Tag
  {
  public:
    explicit Tag(const std::string &tag);
    const std::string &value() const noexcept;

    bool operator<(const Tag &rhs) const noexcept;

    bool operator==(const Tag &rhs) const noexcept;

  private:
    std::string tag_;
  };

  using TagSet = BSTree< Tag, bool >;

  template < class Kind > class Name
  {
  public:
    // бросает std::invalid_argument, если raw не проходит normalizeText
    explicit Name(const std::string &raw);
    const std::string &value() const noexcept;

  private:
    std::string value_;
  };

  struct UserNameKind;
  struct ArticleNameKind;

  using UserName = Name< UserNameKind >;
  using ArticleName = Name< ArticleNameKind >;

  extern template class Name< UserNameKind >;
  extern template class Name< ArticleNameKind >;

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

    // может бросить invalid argument на name
    User(const UserName &name, const Latent &latent);

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

    // может бросить invalid argument на name, category, difficulty
    Article(const ArticleName &name, const Category &category, const Difficulty &difficulty,
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

  // Приводит строку к единому формату: слова, разделённые ровно одним пробелом.
  // Словом считается любая последовательность латинских букв, цифр и спецсимволов (@, +, -, ~ и т. д.).
  // Правила:
  //   - допустимы только печатные ASCII-символы (0x20–0x7E), кроме ";
  //   - в строке должна быть хотя бы одна латинская буква;
  //   - пробелы в начале и в конце удаляются, серии пробелов внутри схлопываются в один;
  //   - результат приводится к нижнему регистру.
  // Если нарушено хотя бы одно правило, бросает std::invalid_argument.
  // Рассчитана на локаль "C" (использует <cctype>).
  std::string normalizeLabel(const std::string &label);

  // То же, что и normalizeLabel, но регистр сохраняется.
  std::string normalizeText(const std::string &text);

  // нормализует raw и сверяет по спец. значениям difficulty, если ничего не подошло -- бросает
  // std::invalid argument
  Difficulty parseDifficulty(const std::string &raw);

  // Перевод из difficulty в строку. Варианты строк: "beginner" / "intermediate" / "advanced"
  const char *toString(Difficulty d) noexcept;

  // Нужна, чтобы при вводе категории не было конфликта или путаницы с сложностью. Правило для названия
  // категории: имя категории не должно совпадать с спец именами использующимися в системе. В данном случае
  // сверяем с спец. именами для уровней сложности
  bool isDifficultyName(const std::string &normalized) noexcept;
}

#endif
