#pragma once

#include <gtl/phmap.hpp>

namespace lerw {

template <class Stopper, class Stepper> struct RandomWalkGenerator {
  Stopper stopper;
  Stepper stepper;

  constexpr auto operator()() -> auto {
    using Point = Stepper::Point;
    std::vector<Point> walk{Point::Zero()};

    while (not stopper(walk))
      walk.emplace_back(stepper(walk.back()));

    return walk;
  }
};

template <class Stopper, class Stepper>
struct LoopErasedRandomWalkGenerator {
  Stopper stopper;
  Stepper stepper;

  constexpr auto operator()() -> auto {
    using Point = Stepper::Point;
    gtl::flat_hash_set<Point> visited{Point::Zero()};
    std::vector<Point> walk{Point::Zero()};

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
