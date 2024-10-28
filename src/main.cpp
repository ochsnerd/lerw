#include <print>

#include "lerw.hpp"

auto main() -> int {
  lerw::Stepper stepper{42};
  for (const auto &p : lerw::generate_random_walk(10, stepper)) {
    std::println("{}, {}", p[0], p[1]);
  }
}
