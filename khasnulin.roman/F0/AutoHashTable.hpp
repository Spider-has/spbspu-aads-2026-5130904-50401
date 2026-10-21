#ifndef AUTO_HASH_TABLE_HPP
#define AUTO_HASH_TABLE_HPP

#include "HashTable.hpp"

namespace khasnulin
{
  template < class Key, class Value, class Hash = std::hash< Key >, class Equal = std::equal_to< Key > >
  class AutoHashTable
  {
  public:
    using key_type = Key;

    using mapped_type = Value;

    using public_value_type = std::pair< const Key, Value >;

    using size_type = std::size_t;

    using iterator = HIter< Key, Value, Hash, Equal >;
    using const_iterator = CHIter< Key, Value, Hash, Equal >;

    explicit AutoHashTable(size_type initial_buckets = 16);

    // default rule of 5 == rule of zero
    AutoHashTable(const AutoHashTable &rhs) = default;
    AutoHashTable(AutoHashTable &&rhs) noexcept = default;

    AutoHashTable &operator=(const AutoHashTable &rhs) = default;
    AutoHashTable &operator=(AutoHashTable &&rhs) noexcept = default;

    ~AutoHashTable() noexcept = default;

    // Вставить элемент, если уже существует -- перезаписать
    template < class V > void add(const Key &key, V &&value);

    // Получить элемент по ключу. Если элемента нет -- создать Value{} и вернуть
    Value &operator[](const Key &key);

    // Найти по ключу и вернуть указатель на значение. Если нет ключа -- возврат nullptr. (Колхоз, но для
    // быстрого решения подойдет)
    Value *find(const Key &key);
    const Value *find(const Key &key) const;

    bool has(const Key &key) const;

    // Если удалось удалить -- возвращаем true, иначе false. Без исключений
    bool erase(const Key &key);

    size_type size() const noexcept;
    bool empty() const noexcept;

    size_type bucketSize() const noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator begin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    void clear() noexcept;

  private:
    static constexpr size_type MAX_GROW_ATTEMPTS = 8;
    void grow();

    HashTable< Key, Value, Hash, Equal > hashTable_;
  };
}

#endif
