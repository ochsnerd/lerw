#pragma once

#include <concepts>
#include <gtl/phmap.hpp>
#include <random>
#include <utility>
#include <vector>

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

template <class T> inline constexpr auto zero() -> T;
template <class T> inline constexpr auto dim() -> unsigned;
// TODO: Get rid of this
template <class T> struct constructor {
  template <class InputIt>
  auto operator()(InputIt first, InputIt last) const -> T;
};
// int is a point
template <> inline constexpr auto zero<int>() -> int { return 0; }
template <> inline constexpr auto dim<int>() -> unsigned { return 1; }
template <> struct constructor<int> {
  template <class InputIt>
  auto operator()(InputIt first, InputIt last) const -> int {
    const auto ret = *first++;
    assert(first == last);
    return ret;
  };
};

template <class P>
concept point = std::equality_comparable<P> && requires(P p1, P p2) {
  { p1 + p2 } -> std::same_as<P>;
  { norm<Norm::L1>(p1) } -> std::floating_point;
  { norm<Norm::L2>(p1) } -> std::floating_point;
  { norm<Norm::LINFTY>(p1) } -> std::floating_point;
  { zero<P>() } -> std::same_as<P>;
  {
    constructor<P>{}(std::declval<std::vector<int>::iterator>(),
                     std::declval<std::vector<int>::iterator>())
  } -> std::same_as<P>;
  { dim<P>() };
  typename gtl::flat_hash_set<P>;
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
