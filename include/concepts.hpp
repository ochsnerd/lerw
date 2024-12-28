#pragma once

#include "utils.hpp"
#include <concepts>
#include <gtl/phmap.hpp>
#include <utility>
#include <vector>

namespace lerw {

// int is a point
template <class T> inline constexpr auto zero() -> T {};
template <> inline constexpr auto zero<int>() -> int { return 0; }

template <class P>
concept point = std::equality_comparable<P> && requires(P p1, P p2) {
  { p1 + p2 } -> std::same_as<P>;
  { norm<Norm::L1>(p1) } -> std::floating_point;
  { norm<Norm::L2>(p1) } -> std::floating_point;
  { norm<Norm::LINFTY>(p1) } -> std::floating_point;
  { zero<P>() } -> std::same_as<P>;
  typename gtl::flat_hash_set<P>;
};

template <class S>
concept stepper = requires(S s) {
  typename S::Point;
  { s(std::declval<typename S::Point>()) } -> std::same_as<typename S::Point>;
};

template <class S>
concept stopper = requires(S s) {
  // I would like to have declval for a concept. Alas, I use a specific Point
  { s(std::vector<int>{}) } -> std::same_as<bool>;
};

} // namespace lerw
