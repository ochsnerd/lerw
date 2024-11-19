#pragma once

#include <optional>

#include "concepts.hpp"
#include "gtl/phmap.hpp"

namespace lerw {

template <Lattice L, Stopper<typename L::Point> Stop, Stepper<L> Step>
struct RandomWalkGenerator {
  Stop stopper;
  Step stepper;
  std::optional<size_t> expected_size = std::nullopt;

  constexpr auto operator()() -> auto {
    std::vector<typename L::Point> walk{L::Zero()};

    if (expected_size)
      walk.reserve(*expected_size);

    while (not stopper(walk))
      walk.emplace_back(stepper(walk.back()));

    return walk;
  }
};

template <Lattice G, Stopper<typename G::Point> Stop, Stepper<G> Step>
struct LoopErasedRandomWalkGenerator {
  Stop stopper;
  Step stepper;

  constexpr auto operator()() -> auto {
    using Point = G::Point;
    gtl::flat_hash_set<Point, typename Point::Hash> visited{G::Zero()};
    std::vector<Point> walk{G::Zero()};

    while (not stopper(walk)) {
      auto proposed = stepper(walk.back());
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
