#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include <random>

#include "ldstepper.hpp"
#include "distributions.hpp"
#include "directions.hpp"
#include "point.hpp"

using namespace lerw;
using Catch::Matchers::WithinRel;

// TODO: Think of more meaningful tests
TEST_CASE("LDStepper performs valid steps", "[stepper]") {
  SECTION("Basic stepping behavior") {
    auto rng = std::mt19937{42};
    auto stepper = LDStepper{
        Pareto{2.0},
        L2Direction<Point2D>{},
    };

    const auto start = Point2D{0, 0};
    const auto next = stepper(start, rng);

    // Verify that the point has moved
    REQUIRE(next != start);

    // Verify that multiple steps produce different results
    const auto another_step = stepper(next, rng);
    REQUIRE(another_step != next);
  }
}
