#include "config.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop
#include <iostream>
#include <stdexcept>

namespace po = boost::program_options;

namespace lerw {

Configuration parse_command_line(int argc, char *argv[]) {
  Configuration config{.norm = Norm::L2,
                       .dimension = 2,
                       .num_samples = 1000,
                       .distance = 1000,
                       .alpha = 0.5,
                       .output_path = "",
                       .seed = 42,
                       .help_requested = false};

  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "norm,n",
      po::value<std::string>()->default_value("L2")->notifier(
          [&config](const std::string &n) { config.norm = parse_norm(n); }),
      "norm (L1, L2, or LINF)")(
      "dimension,D",
      po::value<size_t>(&config.dimension)->default_value(config.dimension),
      "dimension of the lattice")(
      "number_of_walks,N",
      po::value<size_t>(&config.num_samples)->default_value(config.num_samples),
      "number of walks")(
      "distance,R",
      po::value<double>(&config.distance)->default_value(config.distance),
      "distance from the origin when the walk is stopped")(
      "alpha,a", po::value<double>(&config.alpha)->default_value(config.alpha),
      "shape parameter (must be > 0)")(
      "seed,s",
      po::value<std::size_t>(&config.seed)->default_value(config.seed),
      "random number generator seed")(
      "output,o", po::value<std::string>(&config.output_path),
      "path to output file (if not specified, writes to stdout)");

  try {
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    config.help_requested = vm.count("help") > 0;
    if (config.help_requested) {
      std::cout << desc << "\n";
    }

    if (config.alpha <= 0) {
      throw std::runtime_error("alpha must be greater than 0");
    }

  } catch (const std::exception &e) {
    throw std::runtime_error(std::string("Error parsing command line: ") +
                             e.what());
  }

  return config;
}

} // namespace lerw
