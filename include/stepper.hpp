#pragma once

#include <random>

#include "concepts.hpp"

namespace lerw {

template <NumberGenerator RNG, Graph G> struct SimpleStepper {
  std::uniform_int_distribution<uint8_t> distribution{
      0, G::Directions().size() - 1};
  RNG rng;

  explicit SimpleStepper(RNG &&rng) : rng{std::move(rng)} {};

  auto operator()(const G::Point &p) -> G::Point {
    return p + G::Directions()[distribution(rng)];
  }
};

} // namespace lerw
