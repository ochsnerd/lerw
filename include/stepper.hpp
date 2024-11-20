#pragma once

#include <random>

#include "concepts.hpp" // IWYU pragma: keep

namespace lerw {

template <NumberGenerator RNG, Point P> struct SimpleStepper {
  std::uniform_int_distribution<std::size_t> distribution{
    0, directions<P>().size() - 1};
  RNG rng;

  explicit SimpleStepper(RNG &&gen) : rng{std::move(gen)} {};

  auto operator()(const P &p) -> P {
    // TODO: we could do the distribution here, then the whole class does not have to be templated
    return add(p, directions<P>()[distribution(rng)]);
  }
};

} // namespace lerw
