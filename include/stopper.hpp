#pragma once

#include <cstddef>
#include <vector>

#include "concepts.hpp"
#include "utils.hpp"

namespace lerw {

struct LengthStopper {
  size_t length;

  template <class P>
  constexpr auto operator()(const std::vector<P> &walk) const -> bool {
    return walk.size() > length;
  }
};

template <Norm N> struct DistanceStopper {
  double distance;

  template <point Point>
  constexpr auto operator()(const std::vector<Point> &walk) const -> bool {
    return norm<N>(walk.back()) > distance;
  }
};

} // namespace lerw
