#include <print>

#include "lerw.hpp"

auto main() -> int {
  lerw::Stepper stepper{42};
  std::println("{}", lerw::format_walk(
                         lerw::generate_fixed_length_random_walk(10, stepper)));
  std::println("{}", lerw::format_walk(
                         lerw::generate_fixed_distance_L1_random_walk(10, stepper)));
  std::println("{}", lerw::format_walk(
				       lerw::generate_fixed_distance_L2Sq_random_walk(10, stepper)));
}
