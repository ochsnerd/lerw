#include <cmath>
#include <print>

#include "lerw.hpp"

auto main() -> int {
  // samples for average
  const auto &N = 1000;

  auto make_generator_factory = [](double distance) {
    return [distance](auto seed) {
      using namespace lerw;
      return LoopErasedRandomWalkGenerator{L2DistanceStopper{distance},
                                           Stepper{seed}};
    };
  };

  for (size_t i = 3; i < 12; ++i) {
    const auto &d = std::pow(2, i);
    std::println("{}, {}", d,
                 lerw::compute_average_length(make_generator_factory(d), N));
  }
}
