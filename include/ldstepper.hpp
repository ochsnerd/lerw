#pragma once

#include <algorithm>
#include <boost/math/distributions/pareto.hpp>
#include <boost/random/uniform_on_sphere.hpp>
#include <concepts> // IWYU pragma: keep // std::uniform_random_bit_generator
#include <random>

#include "concepts.hpp"

namespace lerw {

template <distribution Length, direction Direction> struct LDStepper {
  using Point = Direction::result_type;

  Length length;
  Direction direction;

  explicit LDStepper(Length &&step_length, Direction &&step_direction)
      : length{std::move(step_length)}, direction{std::move(step_direction)} {}

  template <std::uniform_random_bit_generator RNG>
  auto operator()(const Point &p, RNG &rng) -> Point {
    return p + direction(length(rng), rng);
  }
};

// generate Pareto-distributed doubles
struct ParetoDistribution {
  using result_type = double;

  std::uniform_real_distribution<> uniform{};
  boost::math::pareto_distribution<> pareto;

  explicit ParetoDistribution(double alpha) : pareto{1.0, alpha} {};

  template <std::uniform_random_bit_generator RNG>
  auto operator()(RNG &rng) -> double {
    // boost does ~ U^{-1/a}
    return boost::math::quantile(pareto, uniform(rng));
  }
};

// uniformly pick direction from sphere, round to nearest integer lattice point
template <point Point> struct L2Direction {
  using result_type = Point;

  boost::random::uniform_on_sphere<> direction{dim<Point>()};

  template <std::uniform_random_bit_generator RNG>
  constexpr auto operator()(double r, RNG &rng) -> Point {
    auto dir = direction(rng);
    auto dir_int = std::vector<int>{};
    std::transform(dir.begin(), dir.end(), dir.begin(),
                   [r](auto x) { return r * x; });
    std::transform(dir.begin(), dir.end(), std::back_inserter(dir_int),
                   [](auto x) { return std::round(x); });
    return constructor<Point>{}(dir_int.cbegin(), dir_int.cend());
  }
};

} // namespace lerw
