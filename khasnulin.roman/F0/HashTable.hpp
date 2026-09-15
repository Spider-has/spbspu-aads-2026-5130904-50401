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
    ~HashTable();

    HashTable(const HashTable &other);
    HashTable &operator=(const HashTable &other);

    HashTable(HashTable &&other) noexcept;
    HashTable &operator=(HashTable &&other) noexcept;

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
    // Константа для вместимости каждого бакета
    static constexpr size_type BUCKET_CAPACITY = 4;

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
}

#endif
