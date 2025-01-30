#include <fstream>
#include <iostream>
#include <print>
#include <random>
#include <stdexcept>

#include "config.hpp"
#include "lerw.hpp"
#include "utils.hpp"

using namespace lerw;

auto main(int argc, char *argv[]) -> int {
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

  auto seed_rng = std::mt19937{config.seed};
  auto computer =
      DistanceLerwComputer{[&seed_rng] { return std::mt19937{seed_rng()}; },
                   config.num_samples, config.alpha, config.distance};

  const auto lengths = [&] {
    switch (switch_pair(config.dimension, config.norm)) {
    case switch_pair(1, Norm::L1):
      return computer.compute<1, Norm::L1>(
          [](auto walk) { return walk.size(); });
    case switch_pair(2, Norm::L1):
      return computer.compute<2, Norm::L1>(
          [](auto walk) { return walk.size(); });
    case switch_pair(3, Norm::L1):
      return computer.compute<3, Norm::L1>(
          [](auto walk) { return walk.size(); });
    case switch_pair(4, Norm::L1):
      return computer.compute<4, Norm::L1>(
          [](auto walk) { return walk.size(); });
    case switch_pair(5, Norm::L1):
      return computer.compute<5, Norm::L1>(
          [](auto walk) { return walk.size(); });
    case switch_pair(1, Norm::L2):
      return computer.compute<1, Norm::L2>(
          [](auto walk) { return walk.size(); });
    case switch_pair(2, Norm::L2):
      return computer.compute<2, Norm::L2>(
          [](auto walk) { return walk.size(); });
    case switch_pair(3, Norm::L2):
      return computer.compute<3, Norm::L2>(
          [](auto walk) { return walk.size(); });
    case switch_pair(4, Norm::L2):
      return computer.compute<4, Norm::L2>(
          [](auto walk) { return walk.size(); });
    case switch_pair(5, Norm::L2):
      return computer.compute<5, Norm::L2>(
          [](auto walk) { return walk.size(); });
      // TODO: LINF with d=1 is broken
    // case switch_pair(1, Norm::LINF):
    //   return computer.compute<1, Norm::LINF>();
    case switch_pair(2, Norm::LINF):
      return computer.compute<2, Norm::LINF>(
          [](auto walk) { return walk.size(); });
    case switch_pair(3, Norm::LINF):
      return computer.compute<3, Norm::LINF>(
          [](auto walk) { return walk.size(); });
    case switch_pair(4, Norm::LINF):
      return computer.compute<4, Norm::LINF>(
          [](auto walk) { return walk.size(); });
    case switch_pair(5, Norm::LINF):
      return computer.compute<5, Norm::LINF>(
          [](auto walk) { return walk.size(); });
    default:
      throw std::invalid_argument("Unsupported dimension/norm choice");
    }
  }();

  std::println(*out, "# D={}, R={}, N={}, α={}, Norm={}, seed={}",
               config.dimension, config.distance, config.num_samples,
               config.alpha, norm_to_string(config.norm), config.seed);

  for (auto l : lengths) {
    std::println(*out, "{}", l);
  }
}
