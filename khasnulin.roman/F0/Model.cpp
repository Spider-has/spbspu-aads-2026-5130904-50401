#include "Model.hpp"
#include "Entities.hpp"
#include "Math.hpp"
#include <cstdint>

namespace
{
  constexpr std::uint64_t primeFNV1a = 1099511628211;

  constexpr std::uint64_t splitmixIncrement = 0x9E3779B97F4A7C15;

  constexpr std::uint64_t primeSplitmixConst1 = 0xBF58476D1CE4E5B9;
  constexpr std::uint64_t primeSplitmixConst2 = 0x94D049BB133111EB;
};

// Шаг fnv1a для 1 байта
std::uint64_t khasnulin::fnv1aStep(std::uint64_t hash, unsigned char byte) noexcept
{
  return (hash ^ byte) * primeFNV1a;
}

// хэширует строку продолжая переданный хэш. По дефолту стандартный базис fnv1a
std::uint64_t khasnulin::fnv1aHash(const std::string &str, std::uint64_t hash) noexcept
{
  std::uint64_t result = hash;
  for (unsigned char ch : str)
  {
    result = fnv1aStep(result, ch);
  }
  return result;
}

// двигает state на константу и возвращает перемешанное новое значение
std::uint64_t khasnulin::splitmix64(std::uint64_t &seed) noexcept
{
  seed = seed + splitmixIncrement;
  std::uint64_t result = seed;
  result = (result ^ (result >> 30)) * primeSplitmixConst1;
  result = (result ^ (result >> 27)) * primeSplitmixConst2;
  result = result ^ (result >> 31);
  return result;
}

// перевод из 64-битного числа в double от 0 до 1 [0, 1)
double khasnulin::unitFromBits(std::uint64_t bits) noexcept
{
  bits = bits >> 11;
  return double(bits) / double(2ULL << 52);
}

// Получить latent вектор из name пользователя
khasnulin::Latent khasnulin::initialLatent(const UserName &name)
{
  std::uint64_t hash = fnv1aHash("u");
  hash = fnv1aHash(name.value(), hash);
  Latent ans;
  for (std::size_t i = 0; i < latentDimensionCount; ++i)
  {
    std::uint64_t bits = splitmix64(hash);
    double unit = unitFromBits(bits);
    ans.v[i] = toInterval(unit, 0, 1, -latentInitAmplitude, latentInitAmplitude);
  }

  return ans;
}

// Получить latent вектор из name article
khasnulin::Latent khasnulin::initialLatent(const ArticleName &name)
{
  std::uint64_t hash = fnv1aHash("a");
  hash = fnv1aHash(name.value(), hash);
  Latent ans;
  for (std::size_t i = 0; i < latentDimensionCount; ++i)
  {
    std::uint64_t bits = splitmix64(hash);
    double unit = unitFromBits(bits);
    ans.v[i] = toInterval(unit, 0, 1, -latentInitAmplitude, latentInitAmplitude);
  }

  return ans;
}
