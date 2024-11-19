#pragma once

#include <cstddef>
#include <vector>

#include "concepts.hpp" // IWYU pragma: keep

namespace lerw {

struct LengthStopper {
  size_t length;

  template <class P>
  constexpr auto operator()(const std::vector<P> &walk) const -> bool {
    return walk.size() > length;
  }
};

struct L1DistanceStopper {
  double distance;

  template <Point P>
  constexpr auto operator()(const std::vector<P> &walk) const -> bool {
    return walk.back().L1() > distance;
  }
};

struct L2DistanceStopper {
  double distance_sq;

  L2DistanceStopper(double distance) : distance_sq{distance * distance} {}

  template <Point P>
  constexpr auto operator()(const std::vector<P> &walk) const -> bool {
    return walk.back().L2Sq() > distance_sq;
  }
};

} // namespace lerw
