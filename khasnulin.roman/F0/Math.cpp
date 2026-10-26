#include "Math.hpp"
double khasnulin::vecModule(const Latent &vec) noexcept
{
  (void)vec;
  return 0;
}

// скалярное произведение двух векторов
double khasnulin::scalarProduct(const Latent &v1, const Latent &v2) noexcept
{
  (void)v1;
  (void)v2;
  return 0;
}

// косинус угла между векторами от [-1; 1]. Если один из векторов нулевой -- возвращает 0.
double khasnulin::cosine(const Latent &v1, const Latent &v2) noexcept
{
  (void)v1;
  (void)v2;
  return 0;
}

// Сходство Жаккара для двух наборов тегов. Возвращает [0; 1], если одно из переданных множеств пустое --
// возвращает 0.
double khasnulin::jaccard(const TagSet &t1, const TagSet &t2) noexcept
{
  (void)t1;
  (void)t2;
  return 0;
}

// если val выходит за low или high -- возвращает val приведенный к границе. low <= high контракт
double khasnulin::clip(double val, double low, double high) noexcept
{
  (void)val;
  (void)low;
  (void)high;
  return 0;
}

// переводит val к интервалу между newLow и newHigh вместо oldLow и oldHigh. oldLow < oldHigh и newLow <
// newHigh контракт и oldLow <= val <= oldHigh.
double khasnulin::toInterval(double val, double oldLow, double oldHigh, double newLow,
                             double newHigh) noexcept
{
  (void)val;
  (void)oldHigh;
  (void)oldLow;
  (void)newHigh;
  (void)newLow;
  return 0;
}
