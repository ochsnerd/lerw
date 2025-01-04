#include <algorithm>
#include <bits/ranges_algo.h>
#include <cassert>
#include <execution>
#include <functional>
#include <iterator>
#include <vector>

#include "array_point.hpp"
#include "directions.hpp"
#include "distributions.hpp"
#include "generator.hpp"
#include "ldstepper.hpp"
#include "point.hpp"
#include "stopper.hpp"
#include "utils.hpp"

namespace lerw {

template <std::size_t Dim>
  requires(Dim > 0)
struct PointTypeSelector {
  using type = ArrayPoint<Dim>;
};

template <> struct PointTypeSelector<1> {
  using type = Point1D;
};

template <> struct PointTypeSelector<2> {
  using type = Point2D;
};

template <> struct PointTypeSelector<3> {
  using type = Point3D;
};

template <std::size_t dim>
using PointType = typename PointTypeSelector<dim>::type;

template <point P, Norm n> struct DirectionSelector;

template <point P> struct DirectionSelector<P, Norm::L2> {
  using type = L2Direction<P>;
};

template <point P> struct DirectionSelector<P, Norm::LINF> {
  using type = LinfDirection<P>;
};

template <point P, Norm norm>
using DirectionType = typename DirectionSelector<P, norm>::type;

// Length depends on Point because of the type used to store coordinates
template <point P, Norm n> struct LengthSelector;

template <point P> struct LengthSelector<P, Norm::L2> {
  using type = Pareto;
};

template <point P> struct LengthSelector<P, Norm::LINF> {
  using type = Zipf<typename field<P>::type>;
};

template <point P, Norm n>
using LengthType = typename LengthSelector<P, n>::type;

struct LERWComputer {
  std::function<std::mt19937()> rng_factory;
  std::size_t N;
  double alpha;
  double distance;
  template <std::size_t dim, Norm norm> auto compute() const {
    using point_t = PointType<dim>;
    return compute_lerw_lengths(
        [alpha = alpha]() {
          return LDStepper{LengthType<point_t, norm>{alpha}, DirectionType<point_t, norm>{}};
        },
        [distance = distance]() { return DistanceStopper<norm>{distance}; },
        rng_factory, N);
  }
};

template <class GeneratorFactory, class RNGFactory>
auto compute_lengths(GeneratorFactory &&generator_factory,
                     RNGFactory &&rng_factory,
                     size_t N) -> std::vector<size_t> {
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
                            RNGFactory &&rng_factory, size_t N) -> double {
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
