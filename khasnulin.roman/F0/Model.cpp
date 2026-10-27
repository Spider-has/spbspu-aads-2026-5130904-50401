#include "Model.hpp"
#include "Entities.hpp"

// Шаг fnv1a для 1 байта
std::uint64_t khasnulin::fnv1aStep(std::uint64_t hash, unsigned char byte) noexcept
{
  (void)byte;
  (void)hash;
  return 0;
}

// хэширует строку продолжая переданный хэш. По дефолту стандартный базис fnv1a
std::uint64_t khasnulin::fnv1aHash(const std::string &str, std::uint64_t hash) noexcept
{
  (void)str;
  (void)hash;
  return 0;
}

// двигает state на константу и возвращает перемешанное новое значение
std::uint64_t khasnulin::splitmix64(std::uint64_t &seed) noexcept
{
  (void)seed;
  return 0;
}

// перевод из 64-битного числа в double от 0 до 1 [0, 1)
double khasnulin::unitFromBits(std::uint64_t bits) noexcept
{
  (void)bits;

  return 0;
}

// Получить latent вектор из name пользователя
khasnulin::Latent khasnulin::initialLatent(const UserName &name)
{
  (void)name;

  return Latent{};
}

// Получить latent вектор из name article
khasnulin::Latent khasnulin::initialLatent(const ArticleName &name)
{
  (void)name;

  return Latent{};
}
