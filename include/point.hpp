#pragma once

#include <array>
#include <cmath>
#include <unordered_set> // IWYU pragma: keep // std::hash

#include "utils.hpp"
#include "concepts.hpp" // IWYU pragma: keep // zero<T>()

namespace lerw {

using int_t = int32_t;

// TODO: constexpr?
// https://stackoverflow.com/questions/55382543/is-it-possible-to-write-a-constexpr-rounding-function-for-avr
// TODO: compile-time decide which round based on int_t
template <class T> inline auto round(T t) -> int_t {
  return static_cast<int_t>(std::lround(t));
}

struct Point1D {
  using Field_t = int_t;

  Field_t x;

  constexpr auto operator+=(const Point1D &rhs) -> Point1D & {
    x += rhs.x;
    return *this;
  }

  constexpr friend auto operator+(Point1D lhs, const Point1D &rhs) -> Point1D {
    lhs += rhs;
    return lhs;
  }

  constexpr auto operator*=(double rhs) -> Point1D & {
    x = round(static_cast<double>(x) * rhs);
    return *this;
  }

  constexpr friend auto operator*(Point1D lhs, double rhs) -> Point1D {
    lhs *= rhs;
    return lhs;
  }

  constexpr friend auto operator*(double lhs, Point1D rhs) -> Point1D {
    rhs *= lhs;
    return rhs;
  }

  constexpr auto operator==(const Point1D &) const -> bool = default;

  consteval static auto Directions() -> std::array<Point1D, 2> {
    return {Point1D{1}, {-1}};
  }
};

struct Point2D {
  using Field_t = int_t;

  Field_t x;
  Field_t y;

  constexpr auto operator+=(const Point2D &rhs) -> Point2D & {
    x += rhs.x;
    y += rhs.y;
    return *this;
  }

  constexpr friend auto operator+(Point2D lhs, const Point2D &rhs) -> Point2D {
    lhs += rhs;
    return lhs;
  }

  constexpr auto operator*=(double rhs) -> Point2D & {
    x = round(static_cast<double>(x) * rhs);
    y = round(static_cast<double>(y) * rhs);
    return *this;
  }

  constexpr friend auto operator*(Point2D lhs, double rhs) -> Point2D {
    lhs *= rhs;
    return lhs;
  }

  constexpr friend auto operator*(double lhs, Point2D rhs) -> Point2D {
    rhs *= lhs;
    return rhs;
  }

  constexpr auto operator==(const Point2D &) const -> bool = default;

  consteval static auto Directions() -> std::array<Point2D, 8> {
    return {Point2D{
                1,
                0,
            },
            {
                -1,
                0,
            },
            {
                0,
                1,
            },
            {
                0,
                -1,
            }};
  }
};

struct Point3D {
  using Field_t = int_t;

  Field_t x;
  Field_t y;
  Field_t z;

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

  constexpr auto operator*=(double rhs) -> Point3D & {
    x = round(static_cast<double>(x) * rhs);
    y = round(static_cast<double>(y) * rhs);
    z = round(static_cast<double>(z) * rhs);
    return *this;
  }

  constexpr friend auto operator*(Point3D lhs, double rhs) -> Point3D {
    lhs *= rhs;
    return lhs;
  }

  constexpr friend auto operator*(double lhs, Point3D rhs) -> Point3D {
    rhs *= lhs;
    return rhs;
  }

  constexpr auto operator==(const Point3D &) const -> bool = default;

  consteval static auto Directions() -> std::array<Point3D, 8> {
    return {Point3D{1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
            {0, -1, 0},       {0, 0, 1},  {0, 0, -1}};
  }
};

template <Norm N> constexpr auto norm(Point1D p) -> double {
  return norm<N>(p.x);
}

template <Norm N> constexpr auto norm(Point2D p) -> double {
  return norm<N>(p.x, p.y);
}

template <Norm N> constexpr auto norm(Point3D p) -> double {
  return norm<N>(p.x, p.y, p.z);
}

template <> inline constexpr auto zero<Point1D>() -> Point1D { return {0}; }
template <> inline constexpr auto zero<Point2D>() -> Point2D { return {0, 0}; }
template <> inline constexpr auto zero<Point3D>() -> Point3D {
  return {0, 0, 0};
}

} // namespace lerw

namespace std {

template <> struct hash<lerw::Point1D> {
  constexpr auto operator()(const lerw::Point1D &p) const -> std::size_t {
    return std::hash<lerw::Point1D::Field_t>{}(p.x);
  }
};

template <> struct hash<lerw::Point2D> {
  constexpr auto operator()(const lerw::Point2D &p) const -> std::size_t {
    return std::hash<lerw::Point2D::Field_t>{}(p.x) ^
           std::hash<lerw::Point2D::Field_t>{}(p.y << 8);
  }
};

template <> struct hash<lerw::Point3D> {
  constexpr auto operator()(const lerw::Point3D &p) const -> std::size_t {
    return std::hash<lerw::Point3D::Field_t>{}(p.x) ^
           std::hash<lerw::Point3D::Field_t>{}(p.y << 16) ^
           std::hash<lerw::Point3D::Field_t>{}(p.z << 8);
  }
};
} // namespace std
