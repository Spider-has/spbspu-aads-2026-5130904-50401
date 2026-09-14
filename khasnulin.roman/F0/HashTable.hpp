#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "List.hpp"
#include "Vector.hpp"
#include <functional>

namespace khasnulin
{
  template < class Key, class Value, class Hash = std::hash< Key >, class Equal = std::equal_to< Key > >
  class HashTable
  {
  public:
    using key_type = Key;
    using mapped_type = Value;
    using value_type = std::pair< const Key, Value >;
    using size_type = std::size_t;

    class iterator;
    class const_iterator;

    explicit HashTable(size_type initial_buckets = 16);
    ~HashTable();

    HashTable(const HashTable &other);
    HashTable &operator=(const HashTable &other);

    HashTable(HashTable &&other) noexcept;
    HashTable &operator=(HashTable &&other) noexcept;

    void add(Key k, Value v);

    Value drop(Key k);

    bool has(Key k);

    void rehash(size_type slots);

    Value &operator[](const Key &k);

    iterator begin() noexcept;
    iterator end() noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator cend() const noexcept;

    size_type size() const noexcept;
    bool empty() const noexcept;

    size_type overflow_size() const noexcept;
    double average_bucket_size() const noexcept;

    void set_overflow_limit(size_type limit) noexcept;

  private:
    static constexpr size_type BUCKET_CAPACITY = 4;

    struct Bucket
    {
      size_type count;
      value_type data[BUCKET_CAPACITY];
    };

    size_type elements_count_;

    size_type max_overflow_limit_;

    size_type current_overflow_count_;

    Hash hasher_;
    Equal equal_;

    Vector< Bucket > buckets_;
    BiList< Bucket > overflow_;
  };
}

#endif
