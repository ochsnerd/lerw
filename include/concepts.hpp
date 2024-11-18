#pragma once

#include <vector>

namespace lerw {

template <class T>
concept NumberGenerator = requires(T t) {
  { t() } -> std::same_as<unsigned long>;
};

template <class Generator, class U>
concept WalkGenerator = requires(Generator t) {
  { t() } -> std::same_as<std::vector<U>>;
};

template <class GeneratorFactory, class SeedGenerator, class U>
concept WalkGeneratorFactory = NumberGenerator<SeedGenerator> &&
                               requires(GeneratorFactory t, SeedGenerator n) {
                                 { t(std::move(n)) } -> WalkGenerator<U>;
                               };

template <class C, class T>
concept Indexable = requires(C container, std::size_t i) {
  { container.size() } -> std::convertible_to<std::size_t>;
  // {container[i]} -> std::same_as<T>;
  // does not work because operator[] can also return a reference
  requires std::same_as<std::remove_cvref_t<decltype(container[i])>, T>;
};

template <class T>
concept Point = std::equality_comparable<T> && requires(T p1, T p2) {
  { p1 + p2 } -> std::same_as<T>;
  { p1.L2Sq() } -> std::convertible_to<double>;
  { p1.L1() } -> std::convertible_to<double>;
  // TODO: Hashable
};

template <class T>
concept Graph = requires(T) {
  typename T::Point;
  requires Point<typename T::Point>;
  { T::Zero() } -> std::same_as<typename T::Point>;
  requires Indexable<decltype(T::Directions()), typename T::Point>;
};

template <class T, class G>
concept Stepper = Graph<G> && requires(T stepper, G::Point p) {
  { stepper(p) } -> std::same_as<typename G::Point>;
};

template <class T, class P>
concept Stopper = Point<P> && requires(T t, std::vector<P> walk) {
  { t(walk) } -> std::same_as<bool>;
};

} // namespace lerw
