#pragma once

#include <random>

#include "concepts.hpp" // IWYU pragma: keep

namespace lerw {

template <NumberGenerator RNG, Lattice G> struct SimpleStepper {
  std::uniform_int_distribution<uint8_t> distribution{
      0, G::Directions().size() - 1};
  RNG rng;

  explicit SimpleStepper(RNG &&gen) : rng{std::move(gen)} {};

  auto operator()(const G::Point &p) -> G::Point {
    return p + G::Directions()[distribution(rng)];
  }
};

} // namespace lerw
