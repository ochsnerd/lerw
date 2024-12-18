#pragma once

#include <array>
#include <unordered_set> // IWYU pragma: keep // std::hash

#include "utils.hpp"

namespace lerw {

struct Point1D {
  using Field_t = int64_t;
  Field_t x;

  constexpr auto operator+=(const Point1D &rhs) -> Point1D & {
    x += rhs.x;
    return *this;
  }

  constexpr friend auto operator+(Point1D lhs, const Point1D &rhs) -> Point1D {
    lhs += rhs;
    return lhs;
  }

  constexpr auto l2sq() const -> double { return static_cast<double>(x * x); }
  constexpr auto l1() const -> double { return static_cast<double>(abs{}(x)); }

  constexpr auto operator==(const Point1D &) const -> bool = default;

  consteval static auto Zero() -> Point1D { return {0}; }

  consteval static auto Directions() -> std::array<Point1D, 2> {
    return {Point1D{1}, {-1}};
  }
};

struct Point3D {
  int x;
  int y;
  int z;

  constexpr auto operator+=(const Point3D &rhs) -> Point3D & {
    x += rhs.x;
    y += rhs.y;
    z += rhs.z;
    return *this;
  }

  constexpr friend auto operator+(Point3D lhs, const Point3D &rhs) -> Point3D {
    lhs += rhs;
    return lhs;
  }

  constexpr auto l2sq() const -> double {
    return static_cast<double>(x * x + y * y + z * z);
  }

  constexpr auto l1() const -> double {
    constexpr auto a = abs{};
    return static_cast<double>(a(x) + a(y) + a(z));
  }

  constexpr auto operator==(const Point3D &) const -> bool = default;

  consteval static auto Zero() -> Point3D { return {0, 0, 0}; }

  consteval static auto Directions() -> std::array<Point3D, 8> {
    return {Point3D{1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
            {0, -1, 0},       {0, 0, 1},  {0, 0, -1}};
  }
};

} // namespace lerw

namespace std {

template <> struct hash<lerw::Point1D> {
  constexpr auto operator()(const lerw::Point1D &p) const -> std::size_t {
    return std::hash<lerw::Point1D::Field_t>{}(p.x);
  }
};

template <> struct hash<lerw::Point3D> {
  constexpr auto operator()(const lerw::Point3D &p) const -> std::size_t {
    return std::hash<int>{}(p.x) ^ std::hash<int>{}(p.y << 16) ^
           std::hash<int>{}(p.z << 8);
  }
};

} // namespace std
