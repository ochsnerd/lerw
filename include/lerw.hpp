#include <algorithm>
#include <execution>
#include <functional>
#include <numeric>
#include <random>
#include <vector>

#include "generator.hpp"
#include "stepper.hpp"
#include "stopper.hpp"

namespace lerw {

template <class GeneratorFactory>
auto compute_average_length(GeneratorFactory generator_factory, size_t N)
    -> double {
  std::vector<decltype(generator_factory())> generators{};

  // This has to stay std::exectution::seq to prevent a race condition on the
  // seed rng
  std::generate_n(std::execution::seq, std::back_inserter(generators), N,
                  generator_factory);

  return std::transform_reduce(
             std::execution::par_unseq, generators.begin(), generators.end(),
             0.0, std::plus{},
             [](auto &generator) { return generator().size(); }) /
         static_cast<double>(N);
}

template <class StepperFactory>
auto compute_lerw_average_lengths(StepperFactory &&stepper_factory,
                                  const std::vector<double> &distances,
                                  std::size_t n_samples) -> auto {
  auto results = std::vector<std::pair<double, double>>{};
  for (const auto &d : distances) {
    auto stopper_factory = [d] { return L2DistanceStopper{d}; };
    auto generator_factory = [&stopper_factory, &stepper_factory] {
      return LoopErasedRandomWalkGenerator{stopper_factory(),
                                           stepper_factory()};
    };
    auto l = compute_average_length(generator_factory, n_samples);
    results.emplace_back(d, l);
  }
  return results;
}

} // namespace lerw
