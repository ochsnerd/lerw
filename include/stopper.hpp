#pragma once

#include <cstddef>
#include <vector>

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

  template <class Point>
  constexpr auto operator()(const std::vector<Point> &walk) const -> bool {
    return l1(walk.back()) > distance;
  }
};

struct L2DistanceStopper {
  double distance_sq;

  L2DistanceStopper(double distance) : distance_sq{distance * distance} {}

  template <class Point>
  constexpr auto operator()(const std::vector<Point> &walk) const -> bool {
    return walk.back().l2sq() > distance_sq;
  }
};

} // namespace lerw
