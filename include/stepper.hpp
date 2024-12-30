#pragma once

#include <boost/math/distributions/pareto.hpp>
#include <boost/math/special_functions/round.hpp>
#include <boost/random/uniform_on_sphere.hpp>
#include <boost/random/variate_generator.hpp>
#include <cmath>
#include <random>

#include "concepts.hpp" // IWYU pragma: keep

namespace lerw {

// TODO: This is technically broken, the point is not requried to
// have Directions
template <point P> struct NearestNeighborStepper {
  using Point = P;
  std::uniform_int_distribution<std::size_t> distribution{
      0, P::Directions().size() - 1};

  template <std::uniform_random_bit_generator RNG>
  auto operator()(const Point &p, RNG &rng) -> Point {
    return p + P::Directions()[distribution(rng)];
  }
};

} // namespace lerw
