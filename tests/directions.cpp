#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <iostream>
#include <random>
#include <unordered_map>

#include "array_point.hpp"
#include "directions.hpp"

using namespace lerw;

template <typename T>
auto is_approximately_uniform(const std::vector<T> &vec,
                              double tolerance = 0.1) -> bool {
  std::unordered_map<T, std::size_t> frequency;
  for (const T &element : vec) {
    frequency[element]++;
  }

  double expected_frequency =
      static_cast<double>(vec.size()) / frequency.size();

  double max_allowed_deviation = expected_frequency * tolerance;

  for (const auto &pair : frequency) {
    double deviation = std::abs(pair.second - expected_frequency);
    if (deviation > max_allowed_deviation) {
      return false;
    }
  }

  return true;
}

template <std::size_t Dim> void check_uniform(int r, std::size_t N = 100'000) {
  using P = ArrayPoint<Dim>;
  auto rng = std::mt19937{};
  auto points = std::vector<P>{N};
  std::transform(points.cbegin(), points.cend(), points.begin(),
                 [&rng](auto) mutable { return LinftyDirection<P>{}(1, rng); });
  REQUIRE(is_approximately_uniform(points));
}

TEST_CASE("LINF") {
  using D2 = LinftyDirection<ArrayPoint<2>>;
  using D3 = LinftyDirection<ArrayPoint<3>>;
  SECTION("A_k") {
    // A square of sidelength 4 has 3 points per side
    // and 1 point per vertex
    REQUIRE(D2::A_k(1, 2) == 12);
    REQUIRE(D2::A_k(2, 2) == 4);
    // A cube of sidelength 2 has 1 point per side, 1 point
    // per edge and 1 point per vertex
    REQUIRE(D3::A_k(1, 1) == 6);
    REQUIRE(D3::A_k(2, 1) == 12);
    REQUIRE(D3::A_k(3, 1) == 8);
  }
  SECTION("A_k sums") {
    REQUIRE(D2::A_k_sums(2) == std::array{0.0, 12.0});
    REQUIRE(D3::A_k_sums(1) == std::array{0.0, 6.0, 18.0});
  }
  SECTION("uniform") {
    const auto r = GENERATE(1, 2, 3, 10, 50, 100, 1000);
    check_uniform<2>(r);
    check_uniform<3>(r);
    check_uniform<4>(r);
  }
}
