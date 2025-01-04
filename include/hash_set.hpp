#pragma once

#include <concepts>
#include <gtl/phmap.hpp>

namespace lerw {

template <class T> using hash_set = gtl::flat_hash_set<T>;

template <class T>
concept hashable = std::equality_comparable<T> && requires(T t) {
  { std::hash<T>{}(t) } -> std::same_as<std::size_t>;
};

} // namespace lerw
