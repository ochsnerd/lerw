#pragma once

#include <cmath>
#include <stdexcept>
#include <string>

namespace lerw {

struct square {
  template <class T> constexpr auto operator()(T t) const -> T { return t * t; }
};

struct abs {
  template <class T> constexpr auto operator()(T t) const -> T {
    return std::abs(t);
  }
};

enum class Norm { L1, L2, LINFTY };

inline auto norm_to_string(Norm norm) -> std::string {
  switch (norm) {
  case Norm::L1:
    return "L1";
  case Norm::L2:
    return "L2";
  case Norm::LINFTY:
    return "LINFTY";
  default:
    throw std::invalid_argument("Invalid norm value");
  }
}

inline auto parse_norm(const std::string &normStr) -> Norm {
  if (normStr == "L1")
    return Norm::L1;
  if (normStr == "L2")
    return Norm::L2;
  if (normStr == "LINFTY")
    return Norm::LINFTY;
  throw std::invalid_argument("Invalid norm type. Must be L1, L2, or LINFTY");
}

template <class... T> constexpr auto l1_norm(T... args) -> double {
  return (std::abs(args) + ...);
}

template <class... T> constexpr auto l2_norm(T... args) -> double {
  return std::sqrt((square{}(args) + ...));
}

template <class... T> constexpr auto linfty_norm(T... args) -> double {
  return std::max({std::abs(args)...});
}

template <Norm N> struct norm_selector;

template <> struct norm_selector<Norm::L1> {
  template <class... T> constexpr auto operator()(T... args) -> auto {
    return l1_norm(args...);
  }
};

template <> struct norm_selector<Norm::L2> {
  template <class... T> constexpr auto operator()(T... args) -> auto {
    return l2_norm(args...);
  }
};

template <> struct norm_selector<Norm::LINFTY> {
  template <class... T> constexpr auto operator()(T... args) -> auto {
    return linfty_norm(args...);
  }
};

template <Norm N, class... T> constexpr auto norm(T... args) -> auto {
  return norm_selector<N>{}(args...);
}
} // namespace lerw
