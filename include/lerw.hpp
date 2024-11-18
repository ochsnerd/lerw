#include <algorithm>
#include <execution>
#include <functional>
#include <numeric>
#include <vector>

#include "concepts.hpp"

namespace lerw {

template <NumberGenerator RNG, Graph G,
          WalkGeneratorFactory<RNG, typename G::Point> GeneratorFactory>
auto compute_average_length(RNG seedRNG, GeneratorFactory generator_factory,
                            size_t N) -> double {
  // sadly, ranges do not support parallel execution (yet)
  // we could write for example
  // auto total_lengths = std::views::iota(1, N)
  //   | std::views::transform([&generator_factory, &seedRNG](auto) {
  //       return generator_factory(RNG{seedRNG()});
  //     })
  //   | std::views::transform(policy, [](auto& generator){
  //       return generator().size();
  //     })
  //   | std::ranges::fold_left_first(policy, std::plus{});

  std::vector<decltype(generator_factory(RNG{seedRNG()}))> generators;
  // This has to stay std::exectution::seq to prevent a race condition on the
  // seed rng
  std::generate_n(std::execution::seq, std::back_inserter(generators), N,
                  [&generator_factory, &seedRNG] {
                    return generator_factory(RNG{seedRNG()});
                  });

  return std::transform_reduce(
             std::execution::par_unseq, generators.begin(), generators.end(),
             0.0, std::plus{},
             [](auto &generator) { return generator().size(); }) /
         N;
}

} // namespace lerw
