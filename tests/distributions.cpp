#include <algorithm>
#include <cmath>
#include <iterator>
#include <numeric>
#include <random>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <vector>

#include "distributions.hpp"

using Catch::Matchers::WithinRel;

TEST_CASE("Zipf") {
  const auto a = GENERATE(2.5, 3, 3.5);

  // The mean of the Zipf (Zeta) distribution is Z(a - 1) / Z(a) (if a > 2)
  const auto mean_expected = std::riemann_zeta(a - 1) / std::riemann_zeta(a);

  auto rng = std::mt19937{};
  auto zipf = lerw::Zipf{a};

  const auto N = 1 << 16;
  auto v = std::vector<decltype(zipf)::result_type>{};
  std::generate_n(std::back_inserter(v), N, [&] { return zipf(rng); });
  const auto mean = std::accumulate(v.begin(), v.end(), 0.0) / N;

  // Note that the variance is quite high with this many samples
  REQUIRE_THAT(mean, WithinRel(mean_expected, 0.01));
}
