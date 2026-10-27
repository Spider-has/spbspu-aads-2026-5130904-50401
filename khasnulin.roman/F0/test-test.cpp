#include "Math.hpp"
#include "Model.hpp"
#include <boost/test/unit_test.hpp>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

using namespace khasnulin;

namespace
{
  const double tolerance = 1e-12;

  bool isClose(double lhs, double rhs)
  {
    return std::fabs(lhs - rhs) < tolerance;
  }

  bool sameLatent(const Latent &lhs, const Latent &rhs)
  {
    for (std::size_t i = 0; i < latentDimensionCount; ++i)
    {
      if (lhs.v[i] != rhs.v[i])
      {
        return false;
      }
    }
    return true;
  }

  bool withinAmplitude(const Latent &latent)
  {
    for (std::size_t i = 0; i < latentDimensionCount; ++i)
    {
      if (latent.v[i] < -0.1 || latent.v[i] > 0.1)
      {
        return false;
      }
    }
    return true;
  }

  bool matchesReference(const Latent &latent, const double (&reference)[8])
  {
    for (std::size_t i = 0; i < latentDimensionCount; ++i)
    {
      if (!isClose(latent.v[i], reference[i]))
      {
        return false;
      }
    }
    return true;
  }
}

BOOST_AUTO_TEST_SUITE(fnv1a_tests)

BOOST_AUTO_TEST_CASE(empty_string_gives_offset_basis)
{
  BOOST_CHECK(fnv1aHash("") == offsetBasisFNV1a);
  BOOST_CHECK(offsetBasisFNV1a == 0xcbf29ce484222325ULL);
}

BOOST_AUTO_TEST_CASE(matches_reference_values)
{
  BOOST_CHECK(fnv1aHash("a") == 0xaf63dc4c8601ec8cULL);
  BOOST_CHECK(fnv1aHash("foobar") == 0x85944171f73967e8ULL);
}

BOOST_AUTO_TEST_CASE(step_matches_single_byte_hash)
{
  BOOST_CHECK(fnv1aStep(offsetBasisFNV1a, 'a') == fnv1aHash("a"));
}

BOOST_AUTO_TEST_CASE(continues_from_given_hash)
{
  BOOST_CHECK(fnv1aHash("bar", fnv1aHash("foo")) == fnv1aHash("foobar"));
}

BOOST_AUTO_TEST_CASE(prefix_step_then_name_equals_prefixed_string)
{
  BOOST_CHECK(fnv1aHash("alice", fnv1aStep(offsetBasisFNV1a, 'u')) == fnv1aHash("ualice"));
  BOOST_CHECK(fnv1aHash("alice", fnv1aStep(offsetBasisFNV1a, 'u')) == 0x2341ac0552eb9658ULL);
}

BOOST_AUTO_TEST_CASE(bytes_above_127_are_hashed_as_unsigned)
{
  BOOST_CHECK(fnv1aHash(std::string(1, '\xFF')) == 0xaf64724c8602eb6eULL);
  BOOST_CHECK(fnv1aStep(offsetBasisFNV1a, 0xFF) == 0xaf64724c8602eb6eULL);
  BOOST_CHECK(fnv1aHash("\xD0\x96") == 0x0af5f207b744ebbbULL);
}

BOOST_AUTO_TEST_CASE(order_of_bytes_matters)
{
  BOOST_CHECK(fnv1aHash("ab") != fnv1aHash("ba"));
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(splitmix64_tests)

BOOST_AUTO_TEST_CASE(matches_reference_sequence_from_zero)
{
  std::uint64_t state = 0;
  BOOST_CHECK(splitmix64(state) == 0xe220a8397b1dcdafULL);
  BOOST_CHECK(splitmix64(state) == 0x6e789e6aa1b965f4ULL);
  BOOST_CHECK(splitmix64(state) == 0x06c45d188009454fULL);
}

BOOST_AUTO_TEST_CASE(advances_state_by_gamma)
{
  std::uint64_t state = 0;
  splitmix64(state);
  BOOST_CHECK(state == 0x9e3779b97f4a7c15ULL);
  splitmix64(state);
  BOOST_CHECK(state == 0x3c6ef372fe94f82aULL);
}

BOOST_AUTO_TEST_CASE(same_state_gives_same_value)
{
  std::uint64_t first = 12345;
  std::uint64_t second = 12345;
  BOOST_CHECK(splitmix64(first) == splitmix64(second));
  BOOST_CHECK(first == second);
}

BOOST_AUTO_TEST_CASE(consecutive_values_differ)
{
  std::uint64_t state = 42;
  std::uint64_t first = splitmix64(state);
  std::uint64_t second = splitmix64(state);
  BOOST_CHECK(first != second);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(unit_from_bits_tests)

BOOST_AUTO_TEST_CASE(zero_gives_zero)
{
  BOOST_CHECK(unitFromBits(0) == 0.0);
}

BOOST_AUTO_TEST_CASE(maximum_stays_strictly_below_one)
{
  double value = unitFromBits(std::numeric_limits< std::uint64_t >::max());
  BOOST_CHECK(value < 1.0);
  BOOST_CHECK(value == 1.0 - std::ldexp(1.0, -53));
}

BOOST_AUTO_TEST_CASE(highest_bit_gives_half)
{
  BOOST_CHECK(unitFromBits(1ULL << 63) == 0.5);
}

BOOST_AUTO_TEST_CASE(low_eleven_bits_are_ignored)
{
  BOOST_CHECK(unitFromBits(0x7FFULL) == 0.0);
  BOOST_CHECK(unitFromBits((1ULL << 63) | 0x7FFULL) == 0.5);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(initial_latent_tests)

BOOST_AUTO_TEST_CASE(user_matches_reference_vector)
{
  const double reference[8] = {0.058368528226680566, -0.07039031273869019, 0.0709408206449437,
                               -0.08524817123965558, 0.02792848819596777,  -0.062912342008928,
                               -0.08431351853723418, 0.06711034768609475};
  BOOST_CHECK(matchesReference(initialLatent(UserName("alice")), reference));
}

BOOST_AUTO_TEST_CASE(article_matches_reference_vector)
{
  const double reference[8] = {0.09520513202736208,  0.036143239716413034, -0.09761525777759723,
                               -0.05730237065542494, -0.06833917649221177, -0.08180708017777638,
                               0.023130054692718532, -0.07400048912366558};
  BOOST_CHECK(matchesReference(initialLatent(ArticleName("alice")), reference));
}

BOOST_AUTO_TEST_CASE(is_deterministic)
{
  BOOST_CHECK(sameLatent(initialLatent(UserName("bob")), initialLatent(UserName("bob"))));
  BOOST_CHECK(sameLatent(initialLatent(ArticleName("bob")), initialLatent(ArticleName("bob"))));
}

BOOST_AUTO_TEST_CASE(user_and_article_with_same_name_differ)
{
  BOOST_CHECK(!sameLatent(initialLatent(UserName("rust")), initialLatent(ArticleName("rust"))));
}

BOOST_AUTO_TEST_CASE(similar_names_give_different_vectors)
{
  BOOST_CHECK(!sameLatent(initialLatent(UserName("alice")), initialLatent(UserName("alicf"))));
  BOOST_CHECK(!sameLatent(initialLatent(UserName("alice")), initialLatent(UserName("Alice"))));
}

BOOST_AUTO_TEST_CASE(components_stay_within_amplitude)
{
  const std::string letters = "abcdefghijklmnopqrstuvwxyz";
  for (char letter : letters)
  {
    std::string name = std::string("item") + letter;
    BOOST_CHECK(withinAmplitude(initialLatent(UserName(name))));
    BOOST_CHECK(withinAmplitude(initialLatent(ArticleName(name))));
  }
}

BOOST_AUTO_TEST_CASE(vector_is_not_zero)
{
  BOOST_CHECK(vecModule(initialLatent(UserName("alice"))) > 0.0);
  BOOST_CHECK(vecModule(initialLatent(ArticleName("alice"))) > 0.0);
}

BOOST_AUTO_TEST_CASE(components_use_whole_range_with_both_signs)
{
  const std::string letters = "abcdefghijklmnopqrstuvwxyz";
  double minimum = 1.0;
  double maximum = -1.0;
  std::size_t negativeCount = 0;
  std::size_t totalCount = 0;
  for (char letter : letters)
  {
    Latent latent = initialLatent(UserName(std::string("item") + letter));
    for (std::size_t i = 0; i < latentDimensionCount; ++i)
    {
      minimum = std::fmin(minimum, latent.v[i]);
      maximum = std::fmax(maximum, latent.v[i]);
      if (latent.v[i] < 0.0)
      {
        ++negativeCount;
      }
      ++totalCount;
    }
  }
  BOOST_CHECK(minimum < -0.09);
  BOOST_CHECK(maximum > 0.09);
  BOOST_CHECK(negativeCount > totalCount / 3);
  BOOST_CHECK(negativeCount < 2 * totalCount / 3);
}

BOOST_AUTO_TEST_SUITE_END()
