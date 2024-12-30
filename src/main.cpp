#include <functional>
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

#include "ldstepper.hpp"
#include "lerw.hpp"
#include "point.hpp"
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
      "norm (L1, L2, or LINFTY)")(
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
  auto rng_factory = [&seed_rng] { return std::mt19937{seed_rng()}; };
  std::println(*out, "# D={}, R={}, N={}, α={}, Norm={}, seed={}", dimension,
               distance, N, alpha, norm_to_string(norm), seed);

  auto compute_with = [&rng_factory, N](auto stepper, auto stopper) {
    return compute_lerw_lengths(std::move(stepper), std::move(stopper),
                                rng_factory, N);
  };

  const auto lengths = [&] {
    switch (switch_pair(dimension, norm)) {
    case switch_pair(1, Norm::L2): {

      auto stepper_factory = [alpha] -> decltype(auto) {
        return LDStepper{ParetoDistribution{alpha}, L2Direction<Point1D>{}};
      };
      auto stopper_factory = [distance] {
        return DistanceStopper<Norm::L2>{distance};
      };
      return compute_with(stepper_factory, stopper_factory);
    }
    case switch_pair(2, Norm::L2): {
      auto stepper_factory = [alpha] -> decltype(auto) {
        return LDStepper{ParetoDistribution{alpha}, L2Direction<Point2D>{}};
      };
      auto stopper_factory = [distance] {
        return DistanceStopper<Norm::L2>{distance};
      };
      return compute_with(stepper_factory, stopper_factory);
    }
    case switch_pair(3, Norm::L2): {
      auto stepper_factory = [alpha] -> decltype(auto) {
        return LDStepper{ParetoDistribution{alpha}, L2Direction<Point3D>{}};
      };
      auto stopper_factory = [distance] {
        return DistanceStopper<Norm::L2>{distance};
      };
      return compute_with(stepper_factory, stopper_factory);
    }
    default:
      throw std::invalid_argument("Unsupported dimension/norm choice");
    }
  }();

  for (auto l : lengths) {
    std::println(*out, "{}", l);
  }
}
