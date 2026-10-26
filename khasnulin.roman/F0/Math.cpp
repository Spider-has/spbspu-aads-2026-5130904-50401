#include "Math.hpp"
#include "Entities.hpp"
#include <cassert>
#include <cmath>
#include <cstddef>

double khasnulin::vecModule(const Latent &vec) noexcept
{
  static constexpr std::size_t N = latentDimensionCount;
  double module = 0;
  for (size_t i = 0; i < N; ++i)
  {
    module += vec.v[i] * vec.v[i];
  }
  if (module < 0)
  {
    module *= -1;
  }
  return std::sqrt(module);
}

double khasnulin::scalarProduct(const Latent &v1, const Latent &v2) noexcept
{
  static constexpr std::size_t N = latentDimensionCount;
  double prod = 0;
  for (size_t i = 0; i < N; ++i)
  {
    prod += v1.v[i] * v2.v[i];
  }
  return prod;
}

double khasnulin::cosine(const Latent &v1, const Latent &v2) noexcept
{
  double prod = scalarProduct(v1, v2);
  double v1_len = vecModule(v1);
  double v2_len = vecModule(v2);
  if (v1_len == 0 || v2_len == 0)
  {
    return 0;
  }
  return clip((prod / v1_len) / v2_len, -1, 1);
}

// Сходство Жаккара для двух наборов тегов. Возвращает [0; 1], если одно из переданных множеств пустое --
// возвращает 0.
double khasnulin::jaccard(const TagSet &t1, const TagSet &t2) noexcept
{
  int common = 0;
  int total = 0;
  auto it1 = t1.begin();
  auto it2 = t2.begin();
  auto it1_end = t1.end();
  auto it2_end = t2.end();
  while (it1 != it1_end && it2 != it2_end)
  {
    if ((*it1).first == (*it2).first)
    {
      ++common;
      ++it1;
      ++it2;
    }
    else if ((*it1).first < (*it2).first)
    {
      ++it1;
    }
    else
    {
      ++it2;
    }
    ++total;
  }
  for (; it1 != it1_end; ++it1)
  {
    ++total;
  }
  for (; it2 != it2_end; ++it2)
  {
    ++total;
  }
  if (total == 0)
  {
    return 0;
  }
  return clip(static_cast< double >(common) / static_cast< double >(total), 0, 1);
}

double khasnulin::clip(double val, double low, double high) noexcept
{
  assert(low <= high);
  if (val > high)
  {
    return high;
  }
  if (val < low)
  {
    return low;
  }
  return val;
}

double khasnulin::toInterval(double val, double oldLow, double oldHigh, double newLow,
                             double newHigh) noexcept
{
  assert(val >= oldLow && val <= oldHigh);
  assert(oldLow < oldHigh);
  assert(newLow < newHigh);
  double lamda = (newHigh - newLow) / (oldHigh - oldLow);
  double beta = newLow - (lamda * oldLow);
  double result = (lamda * val) + beta;
  return clip(result, newLow, newHigh);
}
