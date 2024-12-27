#pragma once

#include <boost/math/distributions/pareto.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/variate_generator.hpp>
#include <cmath>
#include <cstdint>
#include <limits>
#include <random>

#include "concepts.hpp" // IWYU pragma: keep
#include "point.hpp"

namespace lerw {

// refactor: DI length and direction, sampled seperately

template <class Rng, point P> struct NearestNeighborStepper {
  using Point = P;
  std::uniform_int_distribution<std::size_t> distribution{
      0, P::Directions().size() - 1};
  Rng rng;

  explicit NearestNeighborStepper(Rng &&gen) : rng{std::move(gen)} {};

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
    return static_cast<Point::Field_t>(std::round(clamped));
  }

  auto direction() -> Point::Field_t { return direction_(rng_) * 2 - 1; }
};

template <class Rng> struct LongRangeStepper2D {
  using Point = Point2D;

  std::uniform_real_distribution<> uniform_{0.0, 1.0};
  boost::math::pareto_distribution<> pareto_;
  boost::random::uniform_on_sphere<> direction_{2};
  Rng rng_;

  explicit LongRangeStepper2D(Rng &&rng, double alpha)
      : pareto_{1.0, alpha}, rng_{std::move(rng)} {};

  auto operator()(const Point &p) -> Point {
    const auto d = direction();
    const auto r = pareto();
    const auto step = Point{std::round(d[0] * r), std::round(d[1] * r)};

    return p + step;
  }

  auto pareto() -> double {
    const auto sampled = boost::math::quantile(pareto_, uniform_(rng_));
    // clamp to prevent overflow when rounding - this is obviously not a nice
    // thing to do to a distribution
    const auto clamped = std::clamp(
        sampled,
        static_cast<double>(std::numeric_limits<Point::Field_t>::min()),
        static_cast<double>(std::numeric_limits<Point::Field_t>::max()));
    return clamped;
  }

  auto direction() -> std::vector<double> { return direction_(rng_); }
};

template <class Rng> struct LongRangeStepper3D {
  using Point = Point3D;

  std::uniform_real_distribution<> uniform_{0.0, 1.0};
  boost::math::pareto_distribution<> pareto_;
  boost::random::uniform_on_sphere<> direction_{3};
  Rng rng_;

  explicit LongRangeStepper3D(Rng &&rng, double alpha)
      : pareto_{1.0, alpha}, rng_{std::move(rng)} {};

  auto operator()(const Point &p) -> Point {
    const auto d = direction();
    const auto r = pareto();
    const auto step = Point{static_cast<Point::Field_t>(std::round(d[0] * r)),
                            static_cast<Point::Field_t>(std::round(d[1] * r)),
                            static_cast<Point::Field_t>(std::round(d[2] * r))};

    return p + step;
  }

  auto pareto() -> double {
    const auto sampled = boost::math::quantile(pareto_, uniform_(rng_));
    // clamp to prevent overflow when rounding - this is obviously not a nice
    // thing to do to a distribution
    const auto clamped = std::clamp(
        sampled,
        static_cast<double>(std::numeric_limits<Point::Field_t>::min()),
        static_cast<double>(std::numeric_limits<Point::Field_t>::max()));
    return clamped;
  }

  auto direction() -> std::vector<double> { return direction_(rng_); }
};

} // namespace lerw
