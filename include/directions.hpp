#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <numeric>
#include <random>

#include <boost/math/special_functions.hpp>
#include <boost/random/uniform_on_sphere.hpp>

#include "concepts.hpp"

namespace lerw {

// uniformly pick direction from sphere, round to nearest integer lattice point
template <point Point> struct L2Direction {
  using result_type = Point;

  boost::random::uniform_on_sphere<> direction{static_cast<int>(dim<Point>())};

  template <std::uniform_random_bit_generator RNG>
  constexpr auto operator()(double r, RNG &rng) -> Point {
    auto dir = direction(rng);
    std::transform(dir.begin(), dir.end(), dir.begin(),
                   [r](auto x) { return r * x; });
    auto dir_int = std::vector<typename field<Point>::type>{};
    std::transform(dir.begin(), dir.end(), std::back_inserter(dir_int),
                   [](auto x) { return std::round(x); });
    return constructor<Point>{}(dir_int.cbegin(), dir_int.cend());
  }
};

// uniformly pick direction from surface of linf-ball (cube centered at the
// origin)
template <point Point> struct LinfDirection {
  // Sample a point on the radius r linf-ball by
  // - choosing how many coordinates will be equal to r (e.g. vertex, edge, face
  //   in 3D). For this choice, the options have to be weighed according to the
  //   number of points on that type of face (e.g. in 2D there are 4 vertices).
  //   Call this choice k (e.g. in 3D, when k=2 we will choose a point on an
  //   edge of the cube, excluding the vertices).
  // - Then, choose k coordinates and assign them either +r or -r.
  // - Then, assign a number from [-r+1, ..., r-1] to each of the remaining
  //   coordinates.

  using result_type = Point;

  using int_t = field<Point>::type;
  using float_t = double;

  static constexpr std::size_t d = dim<Point>();

  template <std::uniform_random_bit_generator RNG>
  constexpr auto operator()(int_t r, RNG &rng) -> Point {
    r = std::abs(r);
    const auto k = choose_k(r, rng);

    auto coordinates = std::array<int_t, d>{};
    auto last = std::transform(
        coordinates.cbegin(), coordinates.cbegin() + k, coordinates.begin(),
        [r, &rng](auto) mutable { return r * random_sign(rng); });
    constexpr int_t one{1}; // stupid typecasting
    auto dist = std::uniform_int_distribution<int_t>{one - r, r - one};
    std::transform(last, coordinates.end(), last,
                   [&dist, &rng](auto) mutable { return dist(rng); });
    std::shuffle(coordinates.begin(), coordinates.end(), rng);
    return constructor<Point>{}(coordinates.cbegin(), coordinates.cend());
  }

  constexpr static auto A_k(std::uint16_t k, int_t r) -> float_t {
    // boost forces floating point value return type, keep it throughout since
    // for large r the number of points on faces dominate all others
    const auto combinations = boost::math::binomial_coefficient<float_t>(d, k);
    return combinations * std::pow(2, k) * std::pow(2 * r - 1, d - k);
  }

  constexpr static auto A_k_sums(int_t r) -> std::array<float_t, d> {
    auto ak_sums = std::array<float_t, d>{};
    std::iota(ak_sums.begin(), ak_sums.end(), 1);
    std::transform_exclusive_scan(ak_sums.cbegin(), ak_sums.cend(),
                                  ak_sums.begin(), 0, std::plus{}, [r](auto k) {
                                    // this cast is fine because k <= d << 65535
                                    const auto k_ =
                                        static_cast<std::uint16_t>(k);
                                    return A_k(k_, r);
                                  });
    return ak_sums;
  }

  template <std::uniform_random_bit_generator RNG>
  constexpr static auto choose_k(int_t r, RNG &rng) -> std::uint16_t {
    const auto ak_sums = A_k_sums(r);
    const auto i =
        std::uniform_real_distribution<float_t>{0, ak_sums[d - 1]}(rng);
    return static_cast<std::uint16_t>(
        std::distance(ak_sums.cbegin(),
                      std::lower_bound(ak_sums.cbegin(), ak_sums.cend(), i)));
  }

  template <std::uniform_random_bit_generator RNG>
  constexpr static auto random_sign(RNG &rng) -> int {
    return 2 * std::uniform_int_distribution{0, 1}(rng)-1;
  }
};

} // namespace lerw
