#ifndef MODEL_HPP
#define MODEL_HPP

#include "Entities.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace khasnulin
{

  constexpr std::uint64_t offsetBasisFNV1a = 0xcbf29ce484222325;

  constexpr double targetScaleMax = 1.0;

  constexpr double targetScaleMin = -1.0;

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

  class Learner
  {
  public:
    static constexpr double defaultLearningRate = 0.1;
    static constexpr double defaultRegularization = 0.02;
    static constexpr std::size_t defaultStepCount = 10;

    // Передаем lr -- размер шага после каждого обучения модели. Насколько далеко векторы сдвигаются за шаг.
    // regularization -- штраф за длину векторов при вычислении силы смещения.
    // stepCount -- количество повторений вызовов перевычислений  векторов при обучении модели
    explicit Learner(double learningRate = defaultLearningRate, double regularization = defaultRegularization,
                     std::size_t stepCount = defaultStepCount);

    // делает один шаг обновления векторов для одной оценки. Target -- имеющаяся оценка пользователя в целевом
    // виде [−1, 1]
    void sgdStep(Latent &user, Latent &article, double target) const noexcept;

    // learn фукнция для прокидывания объекта в rate у storage
    void operator()(Latent &user, Latent &article, const Rating &rating) const noexcept;

  private:
    double learningRate_;
    double regularization_;
    std::size_t steps_;
  };
}

#endif
