#pragma once

#include <concepts>
#include <gtl/phmap.hpp>
#include <utility>
#include <vector>

namespace lerw {

template <class P>
concept point = std::equality_comparable<P> && requires(P p1, P p2) {
  { p1 + p2 } -> std::same_as<P>;
  { p1.l2sq() } -> std::floating_point;
  { P::Zero() } -> std::same_as<P>;
  typename gtl::flat_hash_set<P>;
};

template <class S>
concept stepper = requires(S s) {
  typename S::Point;
  { s(std::declval<typename S::Point>()) } -> std::same_as<typename S::Point>;
};

template <class S>
concept stopper = requires(S s) {
  { s(std::vector<int>{}) } -> std::same_as<bool>;
};

} // namespace lerw
