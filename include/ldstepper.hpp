#pragma once

#include <algorithm>
#include <concepts> // IWYU pragma: keep // std::uniform_random_bit_generator
#include <random>

#include "concepts.hpp"

namespace lerw {

template <distribution Length, direction Direction> struct LDStepper {
  using Point = Direction::result_type;

  Length length;
  Direction direction;

  explicit LDStepper(Length &&step_length, Direction &&step_direction)
      : length{std::move(step_length)}, direction{std::move(step_direction)} {}

  template <std::uniform_random_bit_generator RNG>
  auto operator()(const Point &p, RNG &rng) -> Point {
    return p + direction(length(rng), rng);
  }
};

} // namespace lerw
