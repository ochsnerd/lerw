#include <boost/program_options.hpp>
#include <execution>
#include <iostream>
#include <random>
#include <ranges>
#include <utility>

#include "lerw.hpp"

using namespace lerw;

namespace po = boost::program_options;

auto main(int argc, char *argv[]) -> int {
  constexpr static int Dimension = 3;
  // Default values
  size_t n_samples = 1000;  // number of samples for averaging
  size_t max_exponent = 11; // maximum exponent of distance (2^10 = 1024)
  size_t N = 8;             // number of distances

  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")(
      "samples,s", po::value<size_t>(&n_samples)->default_value(n_samples),
      "number of samples for averaging")(
      "max-exponent,e",
      po::value<size_t>(&max_exponent)->default_value(max_exponent),
      "maximum exponent e for distance sampling (will sample from 2^(e-N+1) to "
      "2^e)")("intervals,n", po::value<size_t>(&N)->default_value(N),
              "number of distance intervals to sample");

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

  auto make_generator_factory = [](double distance) {
    using namespace lerw;
    return [distance]<NumberGenerator RNG>(RNG &&rng) {
      return LoopErasedRandomWalkGenerator{
          L2DistanceStopper{distance},
          Stepper<Lattice<Dimension>, RNG>{std::move(rng)}};
    };
  };

  // Generate powers of 2 starting from 2^(max_exponent-N+1) up to
  // 2^max_exponent
  for (auto [d, l] :
       std::ranges::iota_view(max_exponent - N + 1, max_exponent + 1) |
           std::views::transform([](auto exp) { return std::pow(2.0, exp); }) |
           std::views::drop_while([](auto d) { return d < 500; }) |
           std::views::transform([=](auto d) {
             return std::make_pair(
                 d, lerw::compute_average_length(std::execution::par_unseq,
                                                 std::mt19937{(unsigned long)d},
                                                 make_generator_factory(d),
                                                 n_samples));
           })) {
    std::cout << d << ", " << l << '\n';
  }
}
