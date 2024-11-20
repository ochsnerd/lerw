#pragma once

#include <gtl/phmap.hpp>
#include <vector>

namespace lerw {

// TODO: Use this?
template <class F, class U>
concept Factory = requires(F f) {
  { f() } -> std::same_as<U>;
};

template <class T>
concept NumberGenerator = requires(T t) {
  { t() } -> std::same_as<unsigned long>;
};

template <class C, class T>
concept Indexable = requires(C container, std::size_t i) {
  { container.size() } -> std::convertible_to<std::size_t>;
  // { container[i] } -> std::same_as<T>;
  // does not work because operator[] can also return a reference
  requires std::same_as<std::remove_cvref_t<decltype(container[i])>, T>;
};

template <class T> consteval auto zero() -> T;
template <class T> constexpr auto directions() -> std::vector<T>;

template <class P>
concept Point = std::equality_comparable<P> && requires(P p1, P p2) {
  { add(p1, p2) } -> std::same_as<P>;
  { l2sq(p1) } -> std::convertible_to<double>;
  { l1(p1) } -> std::convertible_to<double>;
  { zero<P>() } -> std::same_as<P>;
  requires Indexable<decltype(directions<P>()), P>;
  typename gtl::flat_hash_set<P>;
};

template <class T, class P>
concept Stepper = Point<P> && requires(T stepper, P p) {
  { stepper(p) } -> std::same_as<P>;
};

template <class T, class P>
concept Stopper = Point<P> && requires(T stopper, std::vector<P> walk) {
  { stopper(walk) } -> std::same_as<bool>;
};

} // namespace lerw
