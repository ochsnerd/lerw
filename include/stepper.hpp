#pragma once

#include <boost/math/distributions/pareto.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/random/variate_generator.hpp>
#include <cstdint>
#include <limits>
#include <random>

#include "point.hpp"

namespace lerw {

template <class Rng, class P> struct SimpleStepper {
  using Point = P;
  std::uniform_int_distribution<std::size_t> distribution{
      0, P::Directions().size() - 1};
  Rng rng;

  explicit SimpleStepper(Rng &&gen) : rng{std::move(gen)} {};

  auto operator()(const Point &p) -> Point {
    return p + P::Directions()[distribution(rng)];
  }
};

template <class Rng> struct LongRangeStepper1D {
  using Point = Point1D;

  std::uniform_int_distribution<uint8_t> direction_{0, 1};
  std::uniform_real_distribution<> uniform_{0.0, 1.0};
  boost::math::pareto_distribution<> pareto_;
  Rng rng_;

  explicit LongRangeStepper1D(Rng &&rng, double alpha)
      : pareto_{1.0, alpha}, rng_{std::move(rng)} {};

  auto operator()(const Point &p) -> Point {
    const auto step = pareto() * direction();

    return p + Point{step};
  }

  auto pareto() -> Point::Field_t {
    const auto sampled = boost::math::quantile(pareto_, uniform_(rng_));
    // clamp to prevent overflow when rounding - this is obviously not a nice
    // thing to do to a distribution
    const auto clamped = std::clamp(
        sampled,
        static_cast<double>(std::numeric_limits<Point::Field_t>::min()),
        static_cast<double>(std::numeric_limits<Point::Field_t>::max()));
    return std::llround(clamped);
  }

  auto direction() -> Point::Field_t { return direction_(rng_) * 2 - 1; }
};

// maybe helpful?
// https://www.boost.org/doc/libs/1_86_0/doc/html/doxygen/headers/classboost_1_1random_1_1uniform__on__sphere.html
} // namespace lerw
