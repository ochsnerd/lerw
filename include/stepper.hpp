#pragma once

#include <random>

namespace lerw {

template <class RNG, class Point> struct SimpleStepper {
  using Point_t = Point;
  std::uniform_int_distribution<std::size_t> distribution{
      0, Point::Directions().size() - 1};
  RNG rng;

  explicit SimpleStepper(RNG &&gen) : rng{std::move(gen)} {};

  auto operator()(const Point &p) -> Point {
    return p + Point::Directions()[distribution(rng)];
  }
};

} // namespace lerw
