#pragma once

#include <array>
#include <numeric>
#include <vector>

#include "utils.hpp"

namespace lerw {

template <class T> consteval auto zero() -> T;
template <class T> constexpr auto directions() -> std::vector<T>;

// TODO: This does not work (cannot specialize a template while introducing
// a new template argument
template <std::size_t D> using APoint = std::array<int, D>;

template <std::size_t D> consteval auto zero() -> APoint<D> { return {}; }

template <std::size_t D> constexpr auto directions() -> std::vector<APoint<D>> {
  auto directions = std::vector<APoint<D>>(2 * D);
  for (size_t i = 0; i < 2 * D; ++i) {
    directions[i][i / 2] = i % 2 ? -1 : 1;
  }
  return directions;
}

template <std::size_t D>
constexpr auto add(const APoint<D> &p1, const APoint<D> &p2) -> APoint<D> {
  APoint<D> res{};
  for (size_t i = 0; i < D; ++i)
    res[i] = p1[i] + p2[i];
  return res;
}

template <size_t D> constexpr auto l2sq(const APoint<D> &p) -> double {
  return std::transform_reduce(p.cbegin(), p.cend(), 0, std::plus{}, square{});
}

template <size_t D> constexpr auto l1(const APoint<D> &p) -> double {
  return std::transform_reduce(p.cbegin(), p.cend(), 0, std::plus{}, abs{});
}

template <class P> struct Hash {
  constexpr std::size_t operator()(const P &p) const {
    std::size_t hashValue = 0;
    for (const auto &elem : p) {
      hashValue ^= std::hash<unsigned int>()(elem) + 0x9e3779b9 +
                   (hashValue << 6) + (hashValue >> 2);
    }
    return hashValue;
  }
};

} // namespace lerw
