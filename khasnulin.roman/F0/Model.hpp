#ifndef MODEL_HPP
#define MODEL_HPP

#include "Entities.hpp"
#include <cstdint>
#include <string>

namespace khasnulin
{

  constexpr std::uint64_t offsetBasisFNV1a = 0xcbf29ce484222325;

  // Шаг fnv1a для 1 байта
  std::uint64_t fnv1aStep(std::uint64_t hash, unsigned char byte) noexcept;

  // хэширует строку продолжая переданный хэш. По дефолту стандартный базис fnv1a
  std::uint64_t fnv1aHash(const std::string &str, std::uint64_t hash = offsetBasisFNV1a) noexcept;

  // двигает state на константу и возвращает перемешанное новое значение
  std::uint64_t splitmix64(std::uint64_t &state) noexcept;

  // перевод из 64-битного числа в double от 0 до 1 [0, 1)
  double unitFromBits(std::uint64_t bits) noexcept;

  // Получить latent вектор из name пользователя
  Latent initialLatent(const UserName &name);

  // Получить latent вектор из name article
  Latent initialLatent(const ArticleName &name);
}

#endif
