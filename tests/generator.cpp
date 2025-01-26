#include <catch2/catch_test_macros.hpp>
#include <random>
#include <vector>

#include "directions.hpp"
#include "distributions.hpp"
#include "generator.hpp"
#include "ldstepper.hpp"
#include "point.hpp"
#include "stepper.hpp"
#include "stopper.hpp"

using namespace lerw;

// Stops after a fixed number of calls (so that we can stop even in the presence
// of deterministic loops)
struct MockStopper {
  size_t max_steps;

  mutable int step_count = 0;

  explicit MockStopper(size_t steps) : max_steps{steps} {}

  bool operator()(const std::vector<int> &_) const {
    return ++step_count > max_steps;
  }
};

struct MockStepper {
  using Point = int;

  // Keeps track of how many steps have been taken
  mutable int step_count = 0;

  template <std::uniform_random_bit_generator RNG>
  auto operator()(const Point &prev, RNG &) -> Point const {
    step_count++;
    return prev + 1;
  }
};

struct LoopingMockStepper {
  using Point = int;
  mutable int step_count = 0;

  template <std::uniform_random_bit_generator RNG>
  auto operator()(const Point &prev, RNG &) -> Point const {
    step_count++;
    return (prev + 1) % 4;
  }
};

TEST_CASE("RandomWalkGenerator basic functionality") {
  auto rng = std::mt19937{42};
  SECTION("Walk with zero steps") {
    RandomWalkGenerator generator{ MockStepper{}, MockStopper{0}};
    const auto walk = generator(rng);

    REQUIRE(walk.size() == 1);
    REQUIRE(walk[0] == 0);
  }

  SECTION("Walk with one step") {
    RandomWalkGenerator generator{ MockStepper{}, MockStopper{1}};
    const auto walk = generator(rng);

    REQUIRE(walk.size() == 2);
    REQUIRE(walk[0] == 0);
    REQUIRE(walk[1] == 1);
  }

  SECTION("Walk with multiple steps") {
    const size_t num_steps = 5;
    RandomWalkGenerator generator{ MockStepper{}, MockStopper{num_steps}};
    const std::vector<int> expected = {0, 1, 2, 3, 4, 5};

    const auto walk = generator(rng);

    REQUIRE(walk == expected);
  }

  SECTION("Verify stepper is called correct number of times") {
    const size_t num_steps = 4;
    RandomWalkGenerator generator{ MockStepper{}, MockStopper{num_steps}};
    const auto walk = generator(rng);

    REQUIRE(generator.stepper.step_count == num_steps);
  }

  SECTION("Large number of steps") {
    const size_t large_steps = 1000;
    RandomWalkGenerator generator{
        MockStepper{},
        MockStopper{large_steps}
    };
    const auto walk = generator(rng);

    REQUIRE(walk.size() == large_steps + 1);
    REQUIRE(walk.back() == large_steps);
  }
}

TEST_CASE("LoopErasedRandomWalkGenerator") {
  const auto max_steps = 5;
  MockStopper stopper{max_steps};
  auto rng = std::mt19937{42};

  SECTION("Generates walk of correct length") {
    LoopErasedRandomWalkGenerator generator{MockStepper{}, stopper};
    const std::vector<int> expected = {0, 1, 2, 3, 4, 5};

    const auto walk = generator(rng);

    REQUIRE(walk == expected);
  }

  SECTION("Simple loop detection and erasure") {
    LoopErasedRandomWalkGenerator generator{LoopingMockStepper{}, stopper};
    auto walk = generator(rng);

    // Check that the loop was erased (stepper did 1 -> 2 -> 3 -> 0 -> 1)
    std::vector<int> expected = {0, 1};
    REQUIRE(walk == expected);
  }

  SECTION("Stops at maximum steps") {
    LoopErasedRandomWalkGenerator generator{MockStepper{}, stopper};
    auto walk = generator(rng);

    REQUIRE(walk.size() == max_steps + 1);
  }

  SECTION("Handles zero steps") {
    LoopErasedRandomWalkGenerator generator{MockStepper{}, MockStopper{0}};
    auto walk = generator(rng);

    REQUIRE(walk.size() == 1); // Should still contain start point
    REQUIRE(walk[0] == 0);
  }
}

TEST_CASE("ConstructDifferentCombinations") {
  // TODO: I want these gone, they should be LDSteppers with L1Direction
  const auto a = LoopErasedRandomWalkGenerator{
      NearestNeighborStepper<Point1D>{}, DistanceStopper<Norm::L1>{10.0}};
  const auto b = LoopErasedRandomWalkGenerator{
      NearestNeighborStepper<Point2D>{}, DistanceStopper<Norm::L1>{10.0}};
  const auto c = LoopErasedRandomWalkGenerator{
      NearestNeighborStepper<Point3D>{}, DistanceStopper<Norm::L1>{10.0}};

  const auto d = LoopErasedRandomWalkGenerator{LDStepper{
                                                   Pareto{2.0},
                                                   L2Direction<Point1D>{},
                                               },
                                               DistanceStopper<Norm::L1>{10.0}};
  const auto e = LoopErasedRandomWalkGenerator{LDStepper{
                                                   Pareto{2.0},
                                                   L2Direction<Point2D>{},
                                               },
                                               DistanceStopper<Norm::L1>{10.0}};
  const auto f = LoopErasedRandomWalkGenerator{LDStepper{
                                                   Pareto{2.0},
                                                   L2Direction<Point3D>{},
                                               },
                                               DistanceStopper<Norm::L1>{10.0}};
}
