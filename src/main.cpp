#include <print>
#include <cmath>

#include "lerw.hpp"

auto main() -> int {
  lerw::Stepper stepper{42};

  // samples for average
  const auto& N = 1000;

  auto make_generator = [&stepper](double distance) {
    return lerw::LoopErasedRandomWalkGenerator{
        lerw::L2DistanceStopper{distance}, stepper};
  };

  for (size_t i = 3; i < 9; ++i) {
    const auto &d = std::pow(2, i);
    std::println("{}, {}", d, lerw::compute_average_length( make_generator(d), N));
  }
}
