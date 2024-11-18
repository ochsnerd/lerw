#pragma once

#include <functional>
#include <numeric>

#include "utils.hpp"

namespace lerw {

struct Lattice3D {
  struct Point {
    int x;
    int y;
    int z;

    constexpr auto operator+(const Point &other) const -> Point {
      return {x + other.x, y + other.y, z + other.z};
    }
    constexpr auto L2Sq() const -> double { return x * x + y * y + z * z; }
    constexpr auto L1() const -> double {
      const auto a = abs{};
      return a(x) + a(y) + a(z);
    }

    struct Hash {
      constexpr std::size_t operator()(const Point &vec) const {
        return std::hash<int>()(vec.x) ^ std::hash<int>()(vec.y << 16) ^
               std::hash<int>()(vec.z << 8);
      }
    };

    bool operator==(const Point &) const = default;
  };
  consteval static auto Zero() -> Point { return {0, 0, 0}; }

  consteval static auto Directions() -> std::array<Point, 8> {
    return {Point{1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
            {0, -1, 0},     {0, 0, 1},  {0, 0, -1}};
  }
};

template <size_t Dimension> struct Lattice {

  struct Point {
    std::array<int, Dimension> values;

    constexpr auto operator+(const Point &other) const -> Point {
      Point res{};
      for (size_t i = 0; i < Dimension; ++i)
        res.values[i] = values[i] + other.values[i];
      return res;
    }

    constexpr auto L2Sq() const -> double {
      // Don't compute the sqrt because its expensive and unnecessary
      return std::transform_reduce(values.cbegin(), values.cend(), 0,
                                   std::plus{}, square{});
    }

    constexpr auto L1() const -> double {
      return std::transform_reduce(values.cbegin(), values.cend(), 0,
                                   std::plus{}, abs{});
    }

    // TODO: specialize std::hash
    struct Hash {
      constexpr std::size_t operator()(const Point &vec) const {
        std::size_t hashValue = 0;
        for (const auto &elem : vec.values) {
          hashValue ^= std::hash<unsigned int>()(elem) + 0x9e3779b9 +
                       (hashValue << 6) + (hashValue >> 2);
        }
        return hashValue;
      }
    };

    bool operator==(const Point &) const = default;
  };

  consteval static auto Zero() -> Point {
    // TODO: std::views::repeat?
    Point res{};
    for (size_t i = 0; i < Dimension; ++i)
      res.values[i] = 0;
    return res;
  }

  consteval static auto Directions() -> std::array<Point, 2 * Dimension> {
    auto directions = std::array<Point, 2 * Dimension>{};
    for (size_t i = 0; i < 2 * Dimension; ++i) {
      directions[i].values[i / 2] = i % 2 ? -1 : 1;
    }
    return directions;
  }
};

} // namespace lerw
