#pragma once

#include <optional>

#include "concepts.hpp" // IWYU pragma: keep
#include <gtl/phmap.hpp>

namespace lerw {

template <Point P, Stopper<P> Stop, Stepper<P> Step>
struct RandomWalkGenerator {
  Stop stopper;
  Step stepper;
  std::optional<size_t> expected_size = std::nullopt;

  constexpr auto operator()() -> auto {
    std::vector<P> walk{zero<P>()};

    if (expected_size)
      walk.reserve(*expected_size);

    while (not stopper(walk))
      walk.emplace_back(stepper(walk.back()));

    return walk;
  }
};

template <Point P, Stopper<P> Stop, Stepper<P> Step>
struct LoopErasedRandomWalkGenerator {
  Stop stopper;
  Step stepper;

  constexpr auto operator()() -> auto {
    gtl::flat_hash_set<P> visited{zero<P>()};
    std::vector<P> walk{zero<P>()};

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
