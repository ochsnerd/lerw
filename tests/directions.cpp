#include <algorithm>
#include <array>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <limits>
#include <random>
#include <ranges>
#include <unordered_map>

#include "array_point.hpp"
#include "directions.hpp"
#include "utils.hpp"

using namespace lerw;

using D2 = LinfDirection<ArrayPoint<2>>;
using D3 = LinfDirection<ArrayPoint<3>>;
using int_t = field<ArrayPoint<2>>::type;

template <typename T>
auto is_approximately_uniform(const std::vector<T> &vec,
                              double tolerance) -> bool {
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

template <std::size_t Dim> void require_uniform(int_t r, std::size_t N) {
  using P = ArrayPoint<Dim>;
  auto rng = std::mt19937{};
  auto points = std::vector<P>{N};
  std::transform(points.cbegin(), points.cend(), points.begin(),
                 [r, &rng](auto) { return LinfDirection<P>{}(r, rng); });
  REQUIRE(is_approximately_uniform(points, 0.1));
}

template <std::size_t Dim> void check_distance(int_t r, std::size_t N) {
  using P = ArrayPoint<Dim>;
  auto rng = std::mt19937{};
  auto points = std::vector<P>{N};

  std::ranges::for_each(
      std::ranges::iota_view(0) | std::views::take(N),
      [r](auto p) { REQUIRE(norm<Norm::LINF>(p) == r); },
      [r, &rng](auto) { return LinfDirection<ArrayPoint<Dim>>{}(r, rng); });
}

TEST_CASE("LINF") {
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

  const std::size_t N = 100'000;
  SECTION("uniform") {
    require_uniform<2>(1, N);
    require_uniform<2>(5, N);
    require_uniform<3>(1, N);
    require_uniform<3>(2, N);
    require_uniform<4>(1, N);
  }

  SECTION("max r") {
    const int_t r = GENERATE(1, 2, 5, 100, std::numeric_limits<int_t>::max());
    check_distance<2>(r, N);
    check_distance<3>(r, N);
    check_distance<4>(r, N);
  }
}
