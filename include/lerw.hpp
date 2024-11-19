#include <algorithm>
#include <execution>
#include <functional>
#include <numeric>
#include <vector>

#include "concepts.hpp" // IWYU pragma: keep

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

} // namespace lerw
