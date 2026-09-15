#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "List.hpp"
#include "Vector.hpp"
#include <functional>

namespace khasnulin
{
  // Шаблон с типами: КЛЮЧ, ЗНАЧЕНИЕ, ОБЪЕКТ-ХЭШЕР, ОБЪЕКТ КОМПАРАТОР
  template < class Key, class Value, class Hash = std::hash< Key >, class Equal = std::equal_to< Key > >
  class HashTable
  {
  public:
    // Тип ключа
    using key_type = Key;

    // тип значения
    using mapped_type = Key;

    // Пара ключ-значение для хранения
    using value_type = std::pair< const Key, Value >;

    // Тип размерности для массива
    using size_type = std::size_t;

    class iterator;
    class const_iterator;

    // Явный констркутор с необязательным параметром изначального количества слотов
    explicit HashTable(size_type initial_buckets = 16);

    // rule of 5
    ~HashTable() = default;

    HashTable(const HashTable &rhs);
    HashTable &operator=(const HashTable &rhs);

    HashTable(HashTable &&rhs) noexcept;
    HashTable &operator=(HashTable &&rhs) noexcept;

    // Добавить по ключу K значение V.
    template < class V > void add(const Key &k, V &&v);

    // Удалить значение K
    Value drop(const Key &k);

    // Проверить, если ли по ключу K значение
    bool has(const Key &k);

    // Перехэшировать таблицу с новым числом слотов
    void rehash(size_type slots);

    // Перегрузка оператора квадратные скобки для обращения к значению по ключу
    Value &operator[](const Key &k);

    // итераторы (Forwarding iter, так как внутри у нас связный список)
    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    // Получить текущее количество слотов
    size_type size() const noexcept;

    // Пустая ли hash-map
    bool empty() const noexcept;

    // Получить текущий лимит для списка overflow значений
    size_type overflow_size() const noexcept;

    // Получить среднюю загруженность каждого слота
    double average_bucket_size() const noexcept;

    // Поставить новый лимит для списка overflow значений
    void set_overflow_limit(size_type limit) noexcept;

  private:
    // swap метод для copy and swap идиомы.
    void swap(HashTable &rhs) noexcept;

    // Константа для вместимости каждого бакета
    static constexpr size_type BUCKET_CAPACITY = 4;

    static constexpr size_type DEFAULT_OVERFLOW_LIMIT = 10;

    // структура одного бакета
    struct Bucket
    {
      size_type count;
      value_type data[BUCKET_CAPACITY];
    };

    // текущее количество элементов в hashmap
    size_type elements_count_;

    // лимит для списка overflow значений
    size_type max_overflow_limit_;

    // объекты компаратор и хэшер
    Hash hasher_;
    Equal equal_;

    // массив бакетов (слотов)
    Vector< Bucket > buckets_;
    // список overflow значений
    BiList< value_type > overflow_;
  };

  // Swap метод
  // для реализации copy and swap идиомы
  // для каждого поля объектов left и right вызываем swap
  template < class Key, class Value, class Hasher, class Equal >
  void HashTable< Key, Value, Hasher, Equal >::swap(HashTable &rhs) noexcept
  {
    std::swap(buckets_, rhs.buckets_);
    std::swap(overflow_, rhs.overflow_);
    std::swap(hasher_, rhs.hasher_);
    std::swap(equal_, rhs.equal_);
    std::swap(elements_count_, rhs.elements_count_);
    std::swap(max_overflow_limit_, rhs.max_overflow_limit_);
  }

  // Конструктор класса HashTable
  // Передаем в аргументах изначальное количество бакетов внутри HashTable (по дефолту -- 16)
  // Инициализируем все внутренние поля: Массив bucket, пустой список, переменные количества и
  // max_overflow_limit
  // конструируем Hasher и Equal объекты
  template < class Key, class Value, class Hasher, class Equal >
  HashTable< Key, Value, Hasher, Equal >::HashTable(size_type initial_buckets):
      buckets_(initial_buckets),
      overflow_(),
      hasher_(),
      equal_(),
      elements_count_(0),
      max_overflow_limit_(DEFAULT_OVERFLOW_LIMIT)
  {
  }

  // Конструктор перемещения
  // Берем все поля из объекта rhs и перемещаем их с помощью move семантики
  // объект rhs в итоге ничем не владеет.
  template < class Key, class Value, class Hasher, class Equal >
  HashTable< Key, Value, Hasher, Equal >::HashTable(HashTable< Key, Value, Hasher, Equal > &&rhs) noexcept:
      buckets_(std::move(rhs.buckets_)),
      overflow_(std::move(rhs.overflow_)),
      hasher_(std::move(rhs.hasher_)),
      equal_(std::move(rhs.equal_)),
      elements_count_(rhs.elements_count_),
      max_overflow_limit_(rhs.max_overflow_limit_)
  {
    rhs.elements_count_ = 0;
  }

  // Конструктор копирования по const &
  // передаем в списки инициализации const ссылки на поля в rhs объекте
  template < class Key, class Value, class Hasher, class Equal >
  HashTable< Key, Value, Hasher, Equal >::HashTable(const HashTable< Key, Value, Hasher, Equal > &rhs):
      buckets_(rhs.buckets_),
      overflow_(rhs.overflow_),
      hasher_(rhs.hasher_),
      equal_(rhs.equal_),
      elements_count_(rhs.elements_count_),
      max_overflow_limit_(rhs.max_overflow_limit_)
  {
  }

  // Перегрузка оператора копирующего присваивания
  // реализуем через идиому copy and swap
  // создаем копию объекта rhs, кладем её в cp с помощью копирующего конструктора
  // меняем местами содержимое this и cp, таким образом в this все копии полей из rhs, а в cp все старые
  // данные из this которые уничтожаются в конце действия этой функции в деструкторе
  template < class Key, class Value, class Hasher, class Equal >
  HashTable< Key, Value, Hasher, Equal > &
  HashTable< Key, Value, Hasher, Equal >::operator=(const HashTable< Key, Value, Hasher, Equal > &rhs)
  {
    if (this == &rhs)
    {
      return *this;
    }
    HashTable< Key, Value, Hasher, Equal > cp = rhs;
    swap(cp);
    return *this;
  }

  // Перегрузка опреатора перемещающего присваивания
  // Просто меняем местами содержимое rhs и *this, таким образом в rhs все старые поля из this, которые вскоре
  // уничтожаться, в this все свежие поля из rhs. Копирования нигде не происходит
  template < class Key, class Value, class Hasher, class Equal >
  HashTable< Key, Value, Hasher, Equal > &
  HashTable< Key, Value, Hasher, Equal >::operator=(HashTable< Key, Value, Hasher, Equal > &&rhs) noexcept
  {
    if (this != &rhs)
    {
      swap(rhs);
    }
    return *this;
  }

  // Функция swap
  // для корректной работый ADL
  template < class Key, class Value, class Hasher, class Equal >
  void swap(HashTable< Key, Value, Hasher, Equal > &lhs, HashTable< Key, Value, Hasher, Equal > &rhs) noexcept
  {
    lhs.swap(rhs);
  }
}

#endif
