#pragma once

#include <gtl/phmap.hpp>
#include <random>

#include "concepts.hpp" // IWYU pragma: keep

namespace lerw {

template <stopper Stopper, stepper Stepper> struct RandomWalkGenerator {
  Stopper stopper;
  Stepper stepper;

  template <std::uniform_random_bit_generator RNG>
  constexpr auto operator()(RNG& rng) -> auto {
    std::vector walk{zero<typename Stepper::Point>()};

    while (not stopper(walk))
      walk.emplace_back(stepper(walk.back(), rng));

    return walk;
  }
};

template <stopper Stopper, stepper Stepper>
struct LoopErasedRandomWalkGenerator {
  Stopper stopper;
  Stepper stepper;

  template <std::uniform_random_bit_generator RNG>
  constexpr auto operator()(RNG& rng) -> auto {
    using Point = Stepper::Point;
    const auto start = zero<Point>();
    gtl::flat_hash_set<Point> visited{start};
    std::vector walk{start};

    while (not stopper(walk)) {
      auto proposed = stepper(walk.back(), rng);
      auto [_, inserted] = visited.insert(proposed);

      if (inserted) [[likely]] {
        walk.emplace_back(std::move(proposed));
        continue;
      }

      while (walk.back() != proposed) {
        visited.erase(walk.back());
        walk.pop_back();
      }
    }

    return walk;
  }
};

} // namespace lerw
