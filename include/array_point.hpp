#pragma once

#include <array>

#include <concepts>
#include <cstdint>
#include <functional>
#include <unordered_set> // IWYU pragma: keep // std::hash

#include "concepts.hpp" // IWYU pragma: keep // zero<T>(), etc
#include "utils.hpp"

namespace lerw {

using int_t = int32_t;

// TODO:
// template <std::size_t Dim> using ArrayPoint = std::Array<int_t, Dim>;

// For 2- and 3-dimension, this is as fast as struct{int x; int y; int z;}
template <std::size_t Dim> struct ArrayPoint {
  std::array<int_t, Dim> values;

  constexpr auto operator+=(const ArrayPoint<Dim> &rhs) -> ArrayPoint<Dim> & {
    std::transform(values.cbegin(), values.cend(), rhs.values.cbegin(),
                   values.begin(), std::plus{});
    return *this;
  }

  constexpr friend auto
  operator+(ArrayPoint<Dim> lhs,
            const ArrayPoint<Dim> &rhs) -> ArrayPoint<Dim> {
    lhs += rhs;
    return lhs;
  }

  constexpr auto operator==(const ArrayPoint<Dim> &) const -> bool = default;
};

template <Norm N, std::size_t Dim>
constexpr auto norm(const ArrayPoint<Dim> &p) -> double {
  return std::apply(
      [](const auto &...args) {
        return norm<N, std::decay_t<decltype(args)>...>(args...);
      },
      p.values);
}

template <class T>
concept array_point = requires(T t) {
  t.values;
  // an array has a value_type and a tuple_size
  requires std::same_as<typename decltype(t.values)::value_type,
                        int_t>; // TODO: this is outdated
  std::tuple_size_v<decltype(t.values)>;
};

template <array_point T> constexpr auto zero() -> T { return T{}; }

template <array_point T> constexpr auto dim() -> std::size_t {
  return std::tuple_size_v<decltype(T::values)>;
}

template <array_point T> struct field<T> {
  using type = int_t;
};

template <array_point T> struct constructor<T> {
  template <class InputIt>
  auto operator()(InputIt first, InputIt last) const -> T {
    if (std::distance(first, last) != dim<T>()) {
      throw std::invalid_argument(
          "ArrayPoint constructor requires correct number of elements");
    }
    auto p = zero<T>();
    std::copy(first, last, p.values.begin());
    return p;
  };
};

} // namespace lerw

namespace std {

template <std::size_t Dim> struct hash<lerw::ArrayPoint<Dim>> {
  std::size_t operator()(const lerw::ArrayPoint<Dim> &point) const {
    std::size_t seed = 0;

    for (const auto &value : point.values) {
      // Boost's hash_combine formula
      seed ^= std::hash<lerw::int_t>{}(value) + 0x9e3779b9 + (seed << 6) +
              (seed >> 2);
    }

    return seed;
  }
};

} // namespace std
