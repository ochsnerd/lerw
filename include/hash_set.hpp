#pragma once

#include <concepts>
#include <gtl/phmap.hpp>
#include <robin_hood.h>
#include "absl/container/flat_hash_set.h"

namespace lerw {

// template <class T> using hash_set = gtl::flat_hash_set<T>;
template <class T> using hash_set = absl::flat_hash_set<T>;
// template <class T> using hash_set = robin_hood::unordered_set<T>;


template <class T>
concept hashable = std::equality_comparable<T> && requires(T t) {
  { std::hash<T>{}(t) } -> std::same_as<std::size_t>;
};

} // namespace lerw
