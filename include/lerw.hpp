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

template <class GeneratorFactory, class RNGFactory>
auto compute_lengths(GeneratorFactory &&generator_factory,
                     RNGFactory &&rng_factory,
                     size_t N) -> std::vector<size_t> {
  // TODO: Can we use std::ranges::generate_n to generate a vector of pairs or
  // sth?

  auto generators = std::vector<decltype(generator_factory())>{};
  auto rngs = std::vector<decltype(rng_factory())>{};

  // This has to stay std::exectution::seq to prevent a race condition on the
  // seed rng
  std::generate_n(std::execution::seq, std::back_inserter(rngs), N,
                  rng_factory);
  std::generate_n(std::execution::seq, std::back_inserter(generators), N,
                  generator_factory);

  std::vector<size_t> lengths(N);

  std::transform(
      std::execution::par_unseq, generators.begin(), generators.end(),
      rngs.begin(), lengths.begin(),
      [](auto generator, auto rng) { return generator(rng).size(); });

  return lengths;
}

template <class StepperFactory, class StopperFactory, class RNGFactory>
auto compute_lerw_lengths(StepperFactory &&stepper_factory,
                          StopperFactory &&stopper_factory,
                          RNGFactory &&rng_factory,
                          std::size_t n_samples) -> auto {
  auto generator_factory = [&stopper_factory, &stepper_factory] {
    return LoopErasedRandomWalkGenerator{stopper_factory(), stepper_factory()};
  };
  return compute_lengths(generator_factory, rng_factory, n_samples);
}

template <class GeneratorFactory, class RNGFactory>
auto compute_average_length(GeneratorFactory &&generator_factory,
                            RNGFactory &&rng_factory,

                            size_t N) -> double {
  assert(N != 0);
  return std::ranges::fold_left_first(
             compute_lengths(std::move(generator_factory),
                             std::move(rng_factory), N),
             std::plus{}) /
         static_cast<double>(N);
}

template <class StepperFactory, class RNGFactory>
auto compute_lerw_average_lengths(StepperFactory &&stepper_factory,
                                  RNGFactory &&rng_factory,
                                  const std::vector<double> &distances,
                                  std::size_t n_samples) -> auto {
  auto results = std::vector<std::pair<double, double>>{};
  for (const auto &d : distances) {
    auto stopper_factory = [d] { return DistanceStopper<Norm::L2>{d}; };
    auto l = compute_average_length(
        [&stopper_factory, &stepper_factory] {
          return LoopErasedRandomWalkGenerator{stopper_factory(),
                                               stepper_factory()};
        },
        rng_factory, n_samples);
    results.emplace_back(d, l);
  }
  return results;
}
} // namespace lerw
