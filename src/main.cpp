#include <boost/program_options.hpp>
#include <iostream>
#include <random>
#include <ranges>

#include "lerw.hpp"
#include "point.hpp"
#include "stepper.hpp"

#include <map>

using namespace lerw;

namespace po = boost::program_options;

// Print a histogram of the pareto distribution as a sanity check
// https://en.wikipedia.org/wiki/Pareto_distribution
// https://www.boost.org/doc/libs/1_86_0/libs/math/doc/html/math_toolkit/dist_ref/dists/pareto.html
void show_pareto() {
  const auto n = 10000;
  auto stepper = LongRangeStepper1D{std::mt19937{}, 0.5};

  std::map<long, std::size_t> occurences{};
  for (auto t :
       std::views::iota(1, n) | std::views::transform([&stepper](auto) {
         return stepper.pareto();
       })) {
    occurences[t]++;
  }

  for (const auto &[value, number] : occurences) {
    std::cout << value << std::string(number, '*') << '\n';
  }
}

auto main(int argc, char *argv[]) -> int {
  // Default values
  std::size_t n_samples = 1000;  // number of samples for averaging
  std::size_t max_exponent = 10; // maximum exponent of distance (2^10 = 1024)
  std::size_t N = 8;             // number of distances
  double alpha = 0.5;            // shape parameter

  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "samples,s", po::value<size_t>(&n_samples)->default_value(n_samples),
      "number of samples for averaging")(
      "max-exponent,e",
      po::value<size_t>(&max_exponent)->default_value(max_exponent),
      "maximum exponent e for distance sampling (will sample from 2^(e-N+1) to "
      "2^e)")("intervals,n", po::value<size_t>(&N)->default_value(N),
              "number of distance intervals to sample")(
      "alpha,a", po::value<double>(&alpha)->default_value(alpha),
      "shape parameter (must be > 0)");

  po::variables_map vm;
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

  if (max_exponent < N - 1) {
    std::cerr << "Error: maximum exponent must be at least " << N - 1
              << " to accommodate " << N << " intervals\n";
    return 1;
  }

  if (alpha <= 0) {
    std::cerr << "Error: alpha must be greater than 0\n";
    return 1;
  }

  // Generate powers of 2 starting from 2^(max_exponent-N+1) up to
  // 2^max_exponent, dropping anything lower than 500 (too short)
  auto distances =
      std::ranges::iota_view(max_exponent - N + 1, max_exponent + 1) |
      std::views::transform([](auto exp) { return std::pow(2.0, exp); }) |
      std::views::drop_while([](auto d) { return d < 500; }) |
      std::ranges::to<std::vector<double>>();

  auto seed_rng = std::mt19937{42};
  // auto stepper_factory = [&seed_rng] {
  //   return NearestNeighborStepper<std::mt19937, Point3D>{
  //       std::mt19937{seed_rng()}};
  // };
  auto stepper_factory = [&seed_rng, alpha] {
    return LongRangeStepper3D{std::mt19937{seed_rng()}, alpha};
  };

  for (auto [d, l] :
       compute_lerw_average_lengths(stepper_factory, distances, n_samples)) {
    std::cout << d << ", " << l << '\n';
  }
}
