#include <stdexcept>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnull-dereference"
#include <boost/program_options.hpp>
#pragma GCC diagnostic pop
#include <fstream>
#include <iostream>
#include <map>
#include <print>
#include <random>

#include "lerw.hpp"
#include "utils.hpp"

using namespace lerw;
namespace po = boost::program_options;

// helper for switch
constexpr auto switch_pair(std::size_t dimension, Norm norm) -> std::size_t {
  return (dimension << 2) + static_cast<size_t>(norm);
}

auto main(int argc, char *argv[]) -> int {
  Norm norm = Norm::L2;
  std::size_t dimension = 2;
  std::size_t N = 1000;   // number of samples
  double distance = 1000; // distance
  double alpha = 0.5;     // shape parameter
  std::string output_path;
  std::size_t seed = 42; // default seed value

  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "norm,n",
      po::value<std::string>()->default_value("L2")->notifier(
          [&norm](const std::string &n) { norm = parse_norm(n); }),
      "norm (L1, L2, or LINF)")(
      "dimension,D", po::value<size_t>(&dimension)->default_value(dimension),
      "dimension of the lattice")("number_of_walks,N",
                                  po::value<size_t>(&N)->default_value(N),
                                  "number of walks")(
      "distance,R", po::value<double>(&distance)->default_value(distance),
      "distance from the origin when the walk is stopped")(
      "alpha,a", po::value<double>(&alpha)->default_value(alpha),
      "shape parameter (must be > 0)")(
      "seed,s", po::value<std::size_t>(&seed)->default_value(seed),
      "random number generator seed")(
      "output,o", po::value<std::string>(&output_path),
      "path to output file (if not specified, writes to stdout)");

  boost::program_options::variables_map vm;
  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 0;
  }

  if (alpha <= 0) {
    std::cerr << "Error: alpha must be greater than 0\n";
    return 1;
  }

  std::ofstream output_file;
  std::ostream *out = &std::cout; // Default to cout
  if (vm.count("output")) {
    output_file.open(output_path);
    if (!output_file) {
      std::cerr << "Error: Could not open output file: " << output_path << "\n";
      return 1;
    }
    out = &output_file;
  }

  auto seed_rng = std::mt19937{seed};
  auto computer = LERWComputer{[&seed_rng] { return std::mt19937{seed_rng()}; },
                               N, alpha, distance};

  const auto lengths = [&] {
    switch (switch_pair(dimension, norm)) {
    case switch_pair(1, Norm::L1):
      return computer.compute<1, Norm::L1>();
    case switch_pair(2, Norm::L1):
      return computer.compute<2, Norm::L1>();
    case switch_pair(3, Norm::L1):
      return computer.compute<3, Norm::L1>();
    case switch_pair(4, Norm::L1):
      return computer.compute<4, Norm::L1>();
    case switch_pair(5, Norm::L1):
      return computer.compute<5, Norm::L1>();
    case switch_pair(1, Norm::L2):
      return computer.compute<1, Norm::L2>();
    case switch_pair(2, Norm::L2):
      return computer.compute<2, Norm::L2>();
    case switch_pair(3, Norm::L2):
      return computer.compute<3, Norm::L2>();
    case switch_pair(4, Norm::L2):
      return computer.compute<4, Norm::L2>();
    case switch_pair(5, Norm::L2):
      return computer.compute<5, Norm::L2>();
      // TODO: LINF with d=1 is broken
    // case switch_pair(1, Norm::LINF):
    //   return computer.compute<1, Norm::LINF>();
    case switch_pair(2, Norm::LINF):
      return computer.compute<2, Norm::LINF>();
    case switch_pair(3, Norm::LINF):
      return computer.compute<3, Norm::LINF>();
    case switch_pair(4, Norm::LINF):
      return computer.compute<4, Norm::LINF>();
    case switch_pair(5, Norm::LINF):
      return computer.compute<5, Norm::LINF>();
    default:
      throw std::invalid_argument("Unsupported dimension/norm choice");
    }
  }();

  std::println(*out, "# D={}, R={}, N={}, α={}, Norm={}, seed={}", dimension,
               distance, N, alpha, norm_to_string(norm), seed);

  for (auto l : lengths) {
    std::println(*out, "{}", l);
  }
}
