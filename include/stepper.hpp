#pragma once

#include <random>

namespace lerw {

template <class T> constexpr auto directions() -> std::vector<T>;

template <class RNG, class Point> struct SimpleStepper {
  std::uniform_int_distribution<std::size_t> distribution{
      0, directions<Point>().size() - 1};
  RNG rng;

  explicit SimpleStepper(RNG &&gen) : rng{std::move(gen)} {};

  auto operator()(const Point &p) -> Point {
    return add(p, directions<Point>()[distribution(rng)]);
  }
};

} // namespace lerw
