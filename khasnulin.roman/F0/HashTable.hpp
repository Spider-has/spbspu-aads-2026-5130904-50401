#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "LIter.hpp"
#include "List.hpp"
#include "Vector.hpp"
#include <functional>
#include <stdexcept>
#include <utility>

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
    using public_value_type = std::pair< const Key, Value >;

    // Тип размерности для массива
    using size_type = std::size_t;

    class HIter;
    class CHIter;

    // Явный констркутор с необязательным параметром изначального количества слотов
    explicit HashTable(size_type initial_buckets = 16);

    // rule of 5
    ~HashTable() = default;

    HashTable(const HashTable &rhs);
    HashTable &operator=(const HashTable &rhs);

    HashTable(HashTable &&rhs) noexcept;
    HashTable &operator=(HashTable &&rhs) noexcept;

    // Добавить по ключу K значение V.
    template < class V > void add(const Key &key, V &&value);

    // Удалить значение K
    Value drop(const Key &key);

    // Проверить, если ли по ключу K значение
    bool has(const Key &key) const;

    // Перехэшировать таблицу с новым числом слотов
    void rehash(size_type buckets);

    // Перегрузка оператора квадратные скобки для обращения к значению по ключу
    Value &operator[](const Key &key);

    // итераторы (Forwarding iter, так как внутри у нас связный список)
    HIter begin() noexcept;
    HIter end() noexcept;
    CHIter cbegin() const noexcept;
    CHIter cend() const noexcept;

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
    using value_type = std::pair< Key, Value >;

    // Константа для вместимости каждого бакета
    static constexpr size_type BUCKET_CAPACITY = 4;

    static constexpr size_type DEFAULT_OVERFLOW_LIMIT = 10;

    // структура одного бакета
    struct Bucket
    {
      size_type count;
      value_type data[BUCKET_CAPACITY];
      Bucket():
          count(0)
      {
      }
    };

    // swap метод для copy and swap идиомы.
    void swap(HashTable &rhs) noexcept;

    // вспомогательная функция для поиска значения с ключом внутри бакета и в overflow
    const value_type *findInBucketOrOverflow(const Bucket &bucket, const Key &key) const;
    value_type *findInBucketOrOverflow(Bucket &bucket, const Key &key);

    template < class V > value_type &insertIntoBucketOrOverflow(Bucket &bucket, const Key &key, V &&value);

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
      elements_count_(0),
      max_overflow_limit_(DEFAULT_OVERFLOW_LIMIT),
      hasher_(),
      equal_(),
      buckets_(initial_buckets),
      overflow_()
  {
    if (initial_buckets == 0)
    {
      throw std::runtime_error("HashTable initialization error: buckets count can't be zero");
    }
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

  // Функция Add вставки в hashTable значения value
  // Вычисляем позицую ключа в массиве bucket с помощью хэш функции hasher.
  // 1) Если ключ key уже есть в hashTable, находим его в нужном bucket(или в overflow, если bucket
  // переполнен) и обновляем значение на value с помощью perfect forwarding, количество элементов в бакете не
  // меняется

  // 2) Если ключа key не было в hastTable, вставляем в оставшееся место в его bucket, если там есть место и
  // задаем значение Value по perfect forwading, инкрементируем кол-во элеметах в бакете и общее кол-во
  // элементов

  // 3) Если ключа key не было и bucket с ключами переполнен, то
  // проверяем, переполнен ли буфер overfow
  // Если список overfow достиг своего максимума, генерируем исколючение std::overflow_error,
  // Если всё ок, добавляем элемент value с ключом key в конец списка overflow, инкрементируем  общее кол-во
  // элементов.

  template < class Key, class Value, class Hasher, class Equal >
  template < class V >
  void HashTable< Key, Value, Hasher, Equal >::add(const Key &key, V &&value)
  {
    // вычисляем номер бакета с хэш функцией
    size_t bucket_i = (hasher_(key) % buckets_.getSize());
    Bucket &currBucket = buckets_[bucket_i];
    // Ищем в нужном бакете наш элемент, если бакет переполнен и мы не нашли, идем искать в overflow
    value_type *existedValue = findInBucketOrOverflow(currBucket, key);
    if (existedValue)
    {
      // элемент найден, нужно обновить его value
      existedValue->second = std::forward< V >(value);
    }
    else
    {
      // элемент не найден, нужно понять, где мы должны его вставить
      insertIntoBucketOrOverflow(currBucket, key, std::forward< V >(value));
    }
  }

  // Метод has
  // Проверяет, есть ли такой ключ в hashTable
  // 1) Считаем хэш с помощью хэшера и определяем индекс бакета при текущем количестве бакетов
  // 2) Проходимся в цикле до bucket.count элемента и проверяем, есть ли совпадения с ключом.
  // 3) Если совпадение есть -> true
  // 4) Совпадений нет и bucket.count == BUCKET CAPACITY -> идем искать совпадения с ключом в overflow
  // 5) Нашли в overflow -> true
  // 6) Во всех остальных случаях -> false
  template < class Key, class Value, class Hasher, class Equal >
  bool HashTable< Key, Value, Hasher, Equal >::has(const Key &key) const
  {
    size_t bucket_i = (hasher_(key) % buckets_.getSize());
    const Bucket &currBucket = buckets_[bucket_i];
    return findInBucketOrOverflow(currBucket, key) != nullptr;
  }

  // Впомогательный приватный метод findInBucketOrOverflow
  // Ищет в уже переданном бакете элемент, если находит, то возвращает указатель на него
  // Если не находит и бакет заполнен, идет искать в overflow, если находит, возвращает указатель
  // Иначе возврат nullptr
  template < class Key, class Value, class Hasher, class Equal >
  const typename HashTable< Key, Value, Hasher, Equal >::value_type *
  HashTable< Key, Value, Hasher, Equal >::findInBucketOrOverflow(const Bucket &bucket, const Key &key) const
  {
    size_type bucket_size = bucket.count;
    for (size_type i = 0; i < bucket_size; ++i)
    {
      if (equal_(bucket.data[i].first, key))
      {
        return bucket.data + i;
      }
    }

    // тут специально в любом случае будем искать в overflow, так как элемент с ключом в бакете может
    // удалиться, размер бакета уменьшится, а элементы с таким же хэшом в overflow так и останутся в overflow,
    // мы их должны там найти
    for (const value_type &item : overflow_)
    {
      if (equal_(item.first, key))
      {
        return (&item);
      }
    }
    return nullptr;
  }

  template < class Key, class Value, class Hasher, class Equal >
  typename HashTable< Key, Value, Hasher, Equal >::value_type *
  HashTable< Key, Value, Hasher, Equal >::findInBucketOrOverflow(Bucket &bucket, const Key &key)
  {
    return const_cast< value_type * >(
        static_cast< const HashTable * >(this)->findInBucketOrOverflow(bucket, key));
  }

  // Метод drop
  // Удаляет элемент с ключом key, если он есть, возвращает
  // Если мы нашли элемент с ключем key, то нам важно понять, где именно он находится
  // 1) если он в bucket, то необходимо его удалить с его позиции, сдвинуть все остальные элементы, уменьшить
  // count 2) если он в overflow, то необходимо сделать erase по его позиции
  // если элемента нет, бросаем исключение на попытку удалить несуществующий ключ
  template < class Key, class Value, class Hasher, class Equal >
  Value HashTable< Key, Value, Hasher, Equal >::drop(const Key &key)
  {
    size_t bucket_i = (hasher_(key) % buckets_.getSize());
    Bucket &currBucket = buckets_[bucket_i];
    value_type *existedValue = findInBucketOrOverflow(currBucket, key);
    // ищем key в бакете и в overflow
    if (existedValue)
    {
      // нужно определить где именно хранится наш ключ, в bucket или в overflow
      if ((existedValue >= currBucket.data) && existedValue < (currBucket.data + BUCKET_CAPACITY))
      {
        // если адрес нашего value находится между нижней границей массива bucket и верхней границей, то
        // гарантированно в bucket. Вычисляем индекс по арифметике указателей
        size_type val_i = existedValue - currBucket.data;

        // копируем с move семантикой, чтобы сразу на этом этапе все потенциальные исключения пробросились, а
        // дальше exception safety
        Value cp = std::move(existedValue->second);

        for (size_type i = val_i; i < currBucket.count - 1; ++i)
        {
          // Ставим на место нашего key все остальные элементы дальше нашего. Наш элемент уносим в конец.
          std::swap(currBucket.data[i], currBucket.data[i + 1]);
        }
        // исключения не пробросились -> можем декрементировать счетчик и вернуть value
        --currBucket.count;
        --elements_count_;
        return cp;
      }
      // ключ не в бакете, а в overflow, значит ищем там.
      LIter< value_type > iter = overflow_.begin();
      while (!equal_(key, iter->first))
      {
        ++iter;
      }
      // нашли iter на наш key, нужно убрать элемент из списка и вернуть копию значения
      Value cp = std::move(iter->second);
      overflow_.erase(iter);
      --elements_count_;
      return cp;
    }
    throw std::out_of_range("can't drop element from hash map: such key not found");
  }

  // Метод rehash
  // Задача -- перехешировать HashTable с новым количеством бакетов
  // Нужно проверить, поместиться ли текущее количество элементов в новое количестов бакетов + лимит overflow
  // Если поместиться, создаем пустую HastTable с buckets количеством бакетов и начинаем её заполнять с 0,
  // проходясь по основной HashTable итераторами. Все value копируем, значение индекса
  // вычисляем с помощью Hasher и % buckets. Как только все элементы скопировали, делаем swap временного
  // объекта и нашего для гарантии exception safety
  template < class Key, class Value, class Hasher, class Equal >
  void HashTable< Key, Value, Hasher, Equal >::rehash(size_type buckets)
  {
    // Считаем, сколько максимум элементов поместиться при рехэшировании с такими кол-вом buckets;
    size_type newMaxCount = (buckets * BUCKET_CAPACITY) + max_overflow_limit_;
    // Если максимум меньше, чем текущее количество элементов, пробрасываем исключение
    if (elements_count_ > newMaxCount)
    {
      throw std::overflow_error("can't rehash maps to this count of buckets, elements can't be placed there, "
                                "overflow will happened");
    }
    // Создаем временную HashTable с buckets кол-вом bucket и max_overflow_limit равным текущему.
    HashTable< Key, Value, Hasher, Equal > temp(buckets);
    temp.max_overflow_limit_ = max_overflow_limit_;
    // копируем все пары ключ-значение в нашу новую hashMap, учитывая рехэширование.
    for (const value_type &item : (*this))
    {
      temp.add(item.first, item.second);
    }
    // Если все скопировалось успешно и исключений не было, то смело делаем swap Hash Table
    swap(temp);
  }

  // Перегрузка оператора квадратные скобки
  // Сначала находим существующий или вставляем с помощью add
  template < class Key, class Value, class Hasher, class Equal >
  Value &HashTable< Key, Value, Hasher, Equal >::operator[](const Key &key)
  {
    // вычисляем номер бакета с хэш функцией
    size_t bucket_i = (hasher_(key) % buckets_.getSize());
    Bucket &currBucket = buckets_[bucket_i];
    // Ищем в нужном бакете наш элемент
    value_type *existedValue = findInBucketOrOverflow(currBucket, key);
    if (existedValue)
    {
      // элемент найден, нужно вернуть second
      return existedValue->second;
    }
    // элемент не найден, нужно понять, где мы должны его вставить и вернуть его second
    return insertIntoBucketOrOverflow(currBucket, key, Value{}).second;
  }

  template < class Key, class Value, class Hasher, class Equal >
  template < class V >
  typename HashTable< Key, Value, Hasher, Equal >::value_type &
  HashTable< Key, Value, Hasher, Equal >::insertIntoBucketOrOverflow(Bucket &bucket, const Key &key,
                                                                     V &&value)
  {
    if (bucket.count < BUCKET_CAPACITY)
    {
      // бакет не до конца заполнен, можем добавить в этот бакет наш value
      bucket.data[bucket.count] = {key, std::forward< V >(value)};
      // если вставка прошла успешно без исключений, можем инкрементировать счетчик
      ++elements_count_;
      ++bucket.count;
      return bucket.data[bucket.count - 1];
    }

    // бакет заполнен, мы должны вставить в конец overflow, если он не заполнен до конца
    if (overflow_.size() < max_overflow_limit_)
    {
      // место в overflow осталось, можем вставить с помощью empace и perfect forwarding
      overflow_.emplace_back(key, std::forward< V >(value));
      // вставка прошла успешно, инкрементируем общее число элементов
      ++elements_count_;
      return overflow_.back();
    }
    // нельзя вставить, overflow переполнен, возвращаем исключение
    throw std::overflow_error("Bucket is overflow and overflow buffer reached the limit");
  }

  template < class Key, class Value, class Hasher, class Equal >
  typename HashTable< Key, Value, Hasher, Equal >::size_type
  HashTable< Key, Value, Hasher, Equal >::size() const noexcept
  {
    return elements_count_;
  }

  template < class Key, class Value, class Hasher, class Equal >
  bool HashTable< Key, Value, Hasher, Equal >::empty() const noexcept
  {
    return elements_count_ == 0;
  }

  template < class Key, class Value, class Hasher, class Equal >
  typename HashTable< Key, Value, Hasher, Equal >::size_type
  HashTable< Key, Value, Hasher, Equal >::overflow_size() const noexcept
  {
    return overflow_.size();
  }

  template < class Key, class Value, class Hasher, class Equal >
  double HashTable< Key, Value, Hasher, Equal >::average_bucket_size() const noexcept
  {
    if (buckets_.getSize() == 0)
    {
      return 0.0;
    }

    size_type in_buckets_count = elements_count_ - overflow_.size();

    return static_cast< double >(in_buckets_count) / buckets_.getSize();
  }

  // Поставить новый лимит для списка overflow значений
  template < class Key, class Value, class Hasher, class Equal >
  void HashTable< Key, Value, Hasher, Equal >::set_overflow_limit(size_type limit) noexcept
  {
    max_overflow_limit_ = limit;
  }
}

#endif
