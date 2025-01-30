#include <fstream>
#include <iostream>
#include <print>
#include <random>
#include <stdexcept>

#include "config.hpp"
#include "lerw.hpp"
#include "point.hpp"
#include "utils.hpp"

using namespace lerw;

auto main(int argc, char *argv[]) -> int {
  // NOTE: We only do one walk. We ignore the distance-parameter
  // and use the num_samples-parameter to mean the length of the walk
  auto config = parse_command_line(argc, argv);

  if (config.help_requested) {
    return 0;
  }

  std::ofstream output_file;
  std::ostream *out = &std::cout; // Default to cout

  if (!config.output_path.empty()) {
    output_file.open(config.output_path);
    if (!output_file) {
      std::cerr << "Error: Could not open output file: " << config.output_path
                << "\n";
      return 1;
    }
    out = &output_file;
  }

  auto rng = std::mt19937{config.seed};
  auto computer = LengthLerwComputer{config.alpha, config.num_samples};

  const auto points = [&] {
    switch (switch_pair(config.dimension, config.norm)) {
    case switch_pair(2, Norm::L1):
      return computer.compute<2, Norm::L1>(rng);
    case switch_pair(2, Norm::L2):
      return computer.compute<2, Norm::L2>(rng);
    case switch_pair(2, Norm::LINF):
      return computer.compute<2, Norm::LINF>(rng);
    default:
      throw std::invalid_argument("Unsupported dimension/norm choice");
    }
  }();

  std::println(*out, "# D={}, R={}, N={}, α={}, Norm={}, seed={}",
               config.dimension, config.distance, config.num_samples,
               config.alpha, norm_to_string(config.norm), config.seed);

  for (auto p : points) {
    std::println(*out, "{}, {}", p.x, p.y);
  }
}
