#pragma once

#include <vector>

#include "utils.hpp"

namespace lerw {

template <class T> consteval auto zero() -> T;
template <class T> constexpr auto directions() -> std::vector<T>;

struct Point3D {
  int x;
  int y;
  int z;

  auto operator==(const Point3D &) const -> bool = default;
};

template <> consteval auto zero() -> Point3D { return {0, 0, 0}; }

template <> constexpr auto directions() -> std::vector<Point3D> {
  return {Point3D{1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
          {0, -1, 0},       {0, 0, 1},  {0, 0, -1}};
}

constexpr auto add(const Point3D &p1, const Point3D &p2) -> Point3D {
  return {p1.x + p2.x, p1.y + p2.y, p1.z + p2.z};
}

constexpr auto l2sq(const Point3D &p) -> double {
  return p.x * p.x + p.y * p.y + p.z * p.z;
}

constexpr auto l1(const Point3D &p) -> double {
  constexpr auto a = abs{};
  return a(p.x) + a(p.y) + a(p.z);
}

} // namespace lerw

namespace std {
template <> struct hash<lerw::Point3D> {
  constexpr auto operator()(const lerw::Point3D &p) const -> std::size_t {
    return std::hash<int>()(p.x) ^ std::hash<int>()(p.y << 16) ^
           std::hash<int>()(p.z << 8);
  }
};

} // namespace std
