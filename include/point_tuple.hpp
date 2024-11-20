#pragma once

#include <tuple>
#include <vector>

#include "utils.hpp"

namespace lerw {

template <class T> consteval auto zero() -> T;
template <class T> constexpr auto directions() -> std::vector<T>;

using TPoint3D = std::tuple<int, int, int>;

template <> consteval auto zero() -> TPoint3D { return {0, 0, 0}; }

template <> constexpr auto directions() -> std::vector<TPoint3D> {
  return {TPoint3D{1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
          {0, -1, 0},        {0, 0, 1},  {0, 0, -1}};
}

constexpr auto add(const TPoint3D &p1, const TPoint3D &p2) -> TPoint3D {
  const auto &[x1, y1, z1] = p1;
  const auto &[x2, y2, z2] = p2;
  return {x1 + x2, y1 + y2, z1 + z2};
}

constexpr auto l2sq(const TPoint3D &p) -> double {
  const auto &[x, y, z] = p;
  return x * x + y * y + z * z;
}

constexpr auto l1(const TPoint3D &p) -> double {
  constexpr auto a = abs{};
  const auto &[x, y, z] = p;
  return a(x) + a(y) + a(z);
}
} // namespace lerw
