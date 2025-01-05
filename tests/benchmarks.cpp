#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <format>
#include <random>

#include "generator.hpp"
#include "ldstepper.hpp"
#include "lerw.hpp"
#include "stopper.hpp"
#include "utils.hpp"

using namespace lerw;

template <std::size_t dim, Norm norm>
constexpr auto make_generator(double distance, double alpha) {
  using point_t = PointType<dim>;
  return LoopErasedRandomWalkGenerator{
      DistanceStopper<norm>{distance},
      LDStepper{LengthType<point_t, norm>{alpha},
                DirectionType<point_t, norm>{}}};
}

template <std::size_t dim, Norm norm, double alpha, int distance>
constexpr auto bench() {
  BENCHMARK(std::format("D={}, N={}, α={}, R={}", dim, norm_to_string(norm),
                        alpha, distance)) {
    auto rng = std::mt19937{};
    return make_generator<2, Norm::L2>(alpha, distance)(rng).size();
  };
}

TEST_CASE("Walk-Generation Benchmarks", "[benchmark]") {
  bench<2, Norm::L2, 0.5, 1000>();
  bench<2, Norm::L2, 3.0, 1000>();
  bench<3, Norm::L2, 0.5, 1000>();
  bench<3, Norm::L2, 3.0, 1000>();
  bench<2, Norm::LINF, 0.5, 1000>();
  bench<2, Norm::LINF, 3.0, 1000>();
  bench<3, Norm::LINF, 0.5, 1000>();
  bench<3, Norm::LINF, 3.0, 1000>();
}
