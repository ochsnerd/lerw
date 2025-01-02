#pragma once

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
    auto dir_int = std::vector<int>{};
    std::transform(dir.begin(), dir.end(), std::back_inserter(dir_int),
                   [](auto x) { return std::round(x); });
    return constructor<Point>{}(dir_int.cbegin(), dir_int.cend());
  }
};

} // namespace lerw
