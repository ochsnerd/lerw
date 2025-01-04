#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
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

template <std::uniform_random_bit_generator RNG>
constexpr auto random_sign(RNG &rng) -> std::int8_t {
  using i = std::int8_t;
  auto d = std::uniform_int_distribution<i>{0, 1};
  return static_cast<i>(2 * d(rng) - 1);
}

// uniformly pick direction from surface of L1-ball
template <point Point> struct L1Direction {
  // find an integer solution to
  // sum |x_i| = r
  // by using the 'stars and bars' method.
  // Example:
  // For r=5, d=3
  // |****|*
  // corresponds to (0, 4, 1)
  // After this, the sign of each coordinate is randomized.
  // Because (-1 * 0 = 1 * 0 = 0), this oversamples points
  // by a factor of 2 for every 0-coordinate. To combat
  // this, reject a point with a probability of 0.5 for
  // every 0-coordinate.

  using result_type = Point;

  using int_t = field<Point>::type;

  static constexpr std::size_t d = dim<Point>();
  static_assert(d > 0, "Out-of-bounds stuff happens when d = 0.");

  template <std::uniform_random_bit_generator RNG>
  constexpr auto operator()(int_t r, RNG &rng) -> Point {
    auto proposed = get_integer_solution(r, rng);
    while (reject_for_zeros(proposed.cbegin(), proposed.cend(), rng)) {
      proposed = get_integer_solution(r, rng);
    }
    std::transform(proposed.cbegin(), proposed.cend(), proposed.begin(),
                   [&rng](auto i) { return random_sign(rng) * i; });
    return constructor<Point>{}(proposed.cbegin(), proposed.cend());
  }

  template <std::uniform_random_bit_generator RNG>
  constexpr static auto get_integer_solution(int_t r,
                                      RNG &rng) -> std::array<int_t, d> {
    auto bars = sample_iota(r, rng);
    bars[d - 1] = r + d;
    std::sort(bars.begin(), bars.end());
    auto stars = std::array<int_t, d>{};
    std::adjacent_difference(bars.cbegin(), bars.cend(), stars.begin());
    std::transform(stars.cbegin(), stars.cend(), stars.begin(),
                   [&rng](auto s) { return (s - 1); });
    return stars;
  }

  template <class InputIt, std::uniform_random_bit_generator RNG>
  constexpr static auto reject_for_zeros(InputIt first, InputIt last,
                                  RNG &rng) -> bool {
    auto d = std::uniform_int_distribution<std::int8_t>{0, 1};
    return std::transform_reduce(
        first, last, false, std::logical_or{},
        [&d, &rng](auto c) { return c == 0 && d(rng) == 0; });
  }

  template <std::uniform_random_bit_generator RNG>
  constexpr static auto sample_iota(int_t r, RNG &rng) -> std::array<std::size_t, d> {
    // equivalent to
    // auto indices = std::vector<std::size_t>(r + d - 1);
    // std::iota(indices.begin(), indices.end(), 1);
    // auto result = std::array<std::size_t, d>{};
    // std::sample(indices.cbegin(), indices.cend(), result.begin(), d - 1,
    // rng);
    // return result;
    // but does this without materializing the indices-vector.
    // This works fine for smallish d.
    // See  Algorithm 3.4.2S of Seminumeric Algorithms (Knuth) for a less
    // quadratic solution
    auto dist = std::uniform_int_distribution<std::size_t>{1, r + d - 1};
    auto result = std::array<std::size_t, d>{};
    for (auto it = result.begin(); it + 1 < result.end(); ++it) {
      auto next = dist(rng);
      while (std::find(result.begin(), it, next) != it) {
        next = dist(rng);
      }
      *it = next;
    }
    return result;
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
    const auto k = choose_k(r, rng);

    auto coordinates = std::array<int_t, d>{};
    auto last = std::transform(
        coordinates.cbegin(), coordinates.cbegin() + k, coordinates.begin(),
        [r, &rng](auto) { return r * random_sign(rng); });
    constexpr int_t one{1}; // stupid typecasting
    auto dist = std::uniform_int_distribution<int_t>{one - r, r - one};
    std::transform(last, coordinates.end(), last,
                   [&dist, &rng](auto) { return dist(rng); });
    std::shuffle(coordinates.begin(), coordinates.end(), rng);
    return constructor<Point>{}(coordinates.cbegin(), coordinates.cend());
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

  constexpr static auto A_k(std::uint16_t k, int_t r) -> float_t {
    // boost forces floating point value return type, keep it throughout since
    // for large r the number of points on faces dominate all others
    const auto combinations = boost::math::binomial_coefficient<float_t>(d, k);
    return combinations * std::pow(2, k) * std::pow(2 * r - 1, d - k);
  }


};

} // namespace lerw
