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

using int_t = field<ArrayPoint<2>>::type;

template <class T>
void check_approximately_uniform(const std::vector<T> &vec, double tolerance) {
  std::unordered_map<T, std::size_t> frequency;
  for (const T &element : vec) {
    frequency[element]++;
  }

  double expected_frequency =
      static_cast<double>(vec.size()) / frequency.size();

  int max_allowed_deviation = expected_frequency * tolerance;

  for (const auto &pair : frequency) {
    int deviation = std::abs(pair.second - expected_frequency);

    CHECK(deviation < max_allowed_deviation);
  }
}

template <direction Direction>
void check_uniform(Direction d, int_t r, std::size_t N) {
  auto rng = std::mt19937{};
  auto points = std::vector<typename Direction::result_type>{N};
  std::transform(points.cbegin(), points.cend(), points.begin(),
                 [r, &rng, &d](auto) { return d(r, rng); });
  check_approximately_uniform(points, 0.1);
}

template <direction Direction, Norm n>
void require_length(int_t r, std::size_t N) {
  auto rng = std::mt19937{};
  auto points = std::vector<typename Direction::result_type>{N};

  std::ranges::for_each(
      std::ranges::iota_view(0) | std::views::take(N),
      [r](auto p) { REQUIRE(norm<n>(p) == r); },
      [r, &rng](auto) { return Direction{}(r, rng); });
}

TEST_CASE("L1") {
  using D1 = L1Direction<ArrayPoint<1>>;
  using D2 = L1Direction<ArrayPoint<2>>;
  using D3 = L1Direction<ArrayPoint<3>>;
  const std::size_t N = 100'000;

  SECTION("uniform") {
    check_uniform(D1{}, 1, N);
    check_uniform(D1{}, 100, N);
    check_uniform(D2{}, 1, N);
    check_uniform(D2{}, 5, N);
    check_uniform(D3{}, 1, N);
    check_uniform(D3{}, 2, N);
    check_uniform(L1Direction<ArrayPoint<4>>{}, 1, N);
  }

  SECTION("max r") {
    const int_t r =
        GENERATE(1, 2, 5, 100); // , std::numeric_limits<int_t>::max());
    require_length<D1, Norm::L1>(r, N);
    require_length<D2, Norm::L1>(r, N);
    require_length<D3, Norm::L1>(r, N);
    require_length<L1Direction<ArrayPoint<4>>, Norm::L1>(r, N);
  }
}

// TODO: Broken for d=1, why?
TEST_CASE("LINF") {
  using D2 = LinfDirection<ArrayPoint<2>>;
  using D3 = LinfDirection<ArrayPoint<3>>;
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
    // check_uniform(LinfDirection<ArrayPoint<1>>{}, 1, N);
    // require_uniform(LinfDirection<ArrayPoint<1>>{}, 100, N);
    check_uniform(D2{}, 1, N);
    check_uniform(D2{}, 5, N);
    check_uniform(D3{}, 1, N);
    check_uniform(D3{}, 2, N);
    check_uniform(LinfDirection<ArrayPoint<4>>{}, 1, N);
  }

  SECTION("max r") {
    const int_t r = GENERATE(1, 2, 5, 100, std::numeric_limits<int_t>::max());
    // require_length<LinfDirection<ArrayPoint<1>>, Norm::LINF>(r, N);
    require_length<D2, Norm::LINF>(r, N);
    require_length<D3, Norm::LINF>(r, N);
    require_length<LinfDirection<ArrayPoint<4>>, Norm::LINF>(r, N);
  }
}
