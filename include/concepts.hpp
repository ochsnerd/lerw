#pragma once

#include <vector>

namespace lerw {

template <class T>
concept NumberGenerator = requires(T t) {
  { t() } -> std::same_as<unsigned long>;
};

// TODO: Use this?
template <class F, class U>
concept Factory = requires(F f) {
  { f() } -> std::same_as<U>;
};

template <class C, class T>
concept Indexable = requires(C container, std::size_t i) {
  { container.size() } -> std::convertible_to<std::size_t>;
  // {container[i]} -> std::same_as<T>;
  // does not work because operator[] can also return a reference
  requires std::same_as<std::remove_cvref_t<decltype(container[i])>, T>;
};

template <class P>
concept Point = std::equality_comparable<P> && requires(P p1, P p2) {
  { p1 + p2 } -> std::same_as<P>;
  { p1.L2Sq() } -> std::convertible_to<double>;
  { p1.L1() } -> std::convertible_to<double>;
  // TODO: Hashable
};

template <class L>
concept Lattice = requires(L) {
  typename L::Point;
  requires Point<typename L::Point>;
  { L::Zero() } -> std::same_as<typename L::Point>;
  requires Indexable<decltype(L::Directions()), typename L::Point>;
};

template <class T, class L>
concept Stepper = Lattice<L> && requires(T stepper, L::Point p) {
  { stepper(p) } -> std::same_as<typename L::Point>;
};

template <class T, class P>
concept Stopper = Point<P> && requires(T stopper, std::vector<P> walk) {
  { stopper(walk) } -> std::same_as<bool>;
};

} // namespace lerw
