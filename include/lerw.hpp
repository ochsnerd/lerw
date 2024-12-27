#include <algorithm>
#include <bits/ranges_algo.h>
#include <cassert>
#include <execution>
#include <functional>
#include <iterator>
#include <vector>

#include "generator.hpp"
#include "stopper.hpp"
#include "utils.hpp"

namespace lerw {

template <class GeneratorFactory>
auto compute_lengths(GeneratorFactory generator_factory,
                     size_t N) -> std::vector<size_t> {
  std::vector<decltype(generator_factory())> generators{};

  // This has to stay std::exectution::seq to prevent a race condition on the
  // seed rng
  std::generate_n(std::execution::seq, std::back_inserter(generators), N,
                  generator_factory);

  std::vector<size_t> lengths(N);

  std::transform(std::execution::par_unseq, generators.begin(),
                 generators.end(), lengths.begin(),
                 [](auto &generator) { return generator().size(); });

  return lengths;
}

template <class StepperFactory>
auto compute_lerw_lengths(StepperFactory &&stepper_factory, double distance,
                          std::size_t n_samples) -> auto {
  // TODO: Seems strange to build the stopper here and the stepper not (will have to refactor anyway when norm)
  auto stopper_factory = [distance] { return DistanceStopper<Norm::L2>{distance}; };
  auto generator_factory = [&stopper_factory, &stepper_factory] {
    return LoopErasedRandomWalkGenerator{stopper_factory(), stepper_factory()};
  };
  return compute_lengths(generator_factory, n_samples);
}


template <class GeneratorFactory>
auto compute_average_length(GeneratorFactory generator_factory,
                            size_t N) -> double {
  assert(N != 0);
  return std::ranges::fold_left_first(compute_lengths(generator_factory, N),
                                      std::plus{}) /
         static_cast<double>(N);
}

template <class StepperFactory>
auto compute_lerw_average_lengths(StepperFactory &&stepper_factory,
                                  const std::vector<double> &distances,
                                  std::size_t n_samples) -> auto {
  auto results = std::vector<std::pair<double, double>>{};
  for (const auto &d : distances) {
    auto stopper_factory = [d] { return DistanceStopper<Norm::L2>{d}; };
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
