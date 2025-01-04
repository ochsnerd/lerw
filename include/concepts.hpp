#pragma once

#include <concepts>
#include <random>
#include <utility>
#include <vector>

#include "hash_set.hpp"
#include "utils.hpp"

namespace lerw {

// basically
// https://en.cppreference.com/w/cpp/named_req/RandomNumberDistribution
template <class D>
concept distribution = requires(D d) {
  typename D::result_type;
  // I would like to have declval for a concept. Alas, I use a specific RNG
  {
    d(std::declval<std::mt19937 &>())
  } -> std::same_as<typename D::result_type>;
};

template <class T> constexpr auto zero() -> T;
template <class T> constexpr auto dim() -> std::size_t;
template <class T> struct field;
// TODO: Is there a more elegant way to do this?
template <class T> struct constructor {
  template <class InputIt>
  auto operator()(InputIt first, InputIt last) const -> T;
};
// int is a point
template <> constexpr auto zero<int>() -> int { return 0; }
template <> constexpr auto dim<int>() -> std::size_t { return 1; }
template <> struct field<int> {
  using type = int;
};
template <> struct constructor<int> {
  template <class InputIt>
  auto operator()(InputIt first, InputIt last) const -> int {
    if (std::distance(first, last) != 1) {
      throw std::invalid_argument(
          "Integer constructor requires exactly 1 element");
    }
    return *first;
  };
};

// TODO: Use this
// template <std::default_initializable F,
//           auto result = std::bool_constant<(F{}(), true)>()>
// consteval auto is_constexpr(F) {
//   return result;
// }

template <class P>
concept point = hashable<P> && requires(P p1, P p2) {
  { p1 + p2 } -> std::same_as<P>;
  { norm<Norm::L1>(p1) } -> std::floating_point;
  { norm<Norm::L2>(p1) } -> std::floating_point;
  { norm<Norm::LINF>(p1) } -> std::floating_point;
  { zero<P>() } -> std::same_as<P>;
  { dim<P>() } -> std::unsigned_integral;
  typename field<P>::type;
  requires std::signed_integral<typename field<P>::type>;
  {
    constructor<P>{}(
        std::declval<typename std::vector<typename field<P>::type>::iterator>(),
        std::declval<typename std::vector<typename field<P>::type>::iterator>())
  } -> std::same_as<P>;
};

template <class D>
concept direction = requires(D d) {
  typename D::result_type;
  {
    d(std::declval<int>(), std::declval<std::mt19937 &>())
  } -> std::same_as<typename D::result_type>;
};

template <class S>
concept stepper = requires(S s) {
  typename S::Point;
  {
    s(std::declval<typename S::Point>(), std::declval<std::mt19937 &>())
  } -> std::same_as<typename S::Point>;
};

template <class S>
concept stopper = requires(S s) {
  // I would like to have declval for a concept. Alas, I use a specific Point
  { s(std::vector<int>{}) } -> std::same_as<bool>;
};

} // namespace lerw
