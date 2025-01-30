#pragma once

#include <cstddef>
#include <string>

#include "utils.hpp"

namespace lerw {

struct Configuration {
  Norm norm;
  std::size_t dimension;
  std::size_t num_samples;
  double distance;
  double alpha;
  std::string output_path;
  std::size_t seed;
  bool help_requested;
};

Configuration parse_command_line(int argc, char *argv[]);

} // namespace lerw
