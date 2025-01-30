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

template <point P> struct DirectionSelector<P, Norm::L1> {
  using type = L2Direction<P>;
};

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

template <point P> struct LengthSelector<P, Norm::L1> {
  using type = Zipf<typename field<P>::type>;
};

template <point P> struct LengthSelector<P, Norm::L2> {
  using type = Pareto;
};

template <point P> struct LengthSelector<P, Norm::LINF> {
  using type = Zipf<typename field<P>::type>;
};

template <point P, Norm n>
using LengthType = typename LengthSelector<P, n>::type;

struct DistanceLerwComputer {
  std::function<std::mt19937()> rng_factory;
  std::size_t N;
  double alpha;
  double distance;
  template <std::size_t dim, Norm norm, class Projection = std::identity>
  auto compute(Projection projection = {}) const {
    using point_t = PointType<dim>;
    return compute_lengths(
        [alpha = alpha, distance = distance] {
          return LoopErasedRandomWalkGenerator{
              LDStepper{LengthType<point_t, norm>{alpha},
                        DirectionType<point_t, norm>{}},
              DistanceStopper<norm>{distance}};
        },
        rng_factory, projection, N);
  }
};

struct LengthLerwComputer {
  double alpha;
  size_t length;

  template <std::size_t dim, Norm norm, std::uniform_random_bit_generator RNG>
  auto compute(RNG &rng) const {
    using point_t = PointType<dim>;
    return LoopErasedRandomWalkGenerator{
        LDStepper{LengthType<point_t, norm>{alpha},
                  DirectionType<point_t, norm>{}},
        LengthStopper{length}}(rng);
  }
};

template <class GeneratorFactory, class RNGFactory, class Projection>
auto compute_lengths(GeneratorFactory &&generator_factory,
                     RNGFactory &&rng_factory, Projection projection,
                     size_t N) {
  auto generators = std::vector<decltype(generator_factory())>{};
  auto rngs = std::vector<decltype(rng_factory())>{};

  // This has to stay std::exectution::seq to prevent a race condition on the
  // seed rng
  std::generate_n(std::execution::seq, std::back_inserter(rngs), N,
                  rng_factory);
  std::generate_n(std::execution::seq, std::back_inserter(generators), N,
                  generator_factory);

  std::vector<size_t> lengths(N);

  std::transform(std::execution::par_unseq, generators.begin(),
                 generators.end(), rngs.begin(), lengths.begin(),
                 [projection](auto generator, auto rng) {
                   return projection(generator(rng));
                 });

  return lengths;
}

} // namespace lerw
