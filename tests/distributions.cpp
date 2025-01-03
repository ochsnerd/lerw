#include <cmath>
#include <numeric>
#include <random>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "distributions.hpp"

using Catch::Matchers::WithinRel;

TEST_CASE("Zipf") {
  const auto a = GENERATE(1.5, 2, 2.5);

  // The mean of the Zipf (Zeta) distribution is Z(a) / Z(a + 1) (if a > 1)
  // (our alpha is s + 1 from wiki)
  const auto mean_expected = std::riemann_zeta(a) / std::riemann_zeta(a + 1);

  auto rng = std::mt19937{};
  auto zipf = lerw::Zipf{a};

  const auto N = 1 << 16;
  auto v = std::vector<decltype(zipf)::result_type>{};
  std::generate_n(std::back_inserter(v), N, [&] { return zipf(rng); });
  const auto mean = std::accumulate(v.begin(), v.end(), 0.0) / N;

  // Note that the variance is quite high with this many samples
  REQUIRE_THAT(mean, WithinRel(mean_expected, 0.01));
}
