#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "point.hpp"
#include "stopper.hpp"
#include "utils.hpp"

using namespace lerw;
using Catch::Matchers::WithinRel;

TEST_CASE("Norms") {
  CHECK(norm<Norm::L1>(1) == 1);
  CHECK(norm<Norm::L1>(1, -3) == 4);
  CHECK(norm<Norm::L2>(1) == 1);
  CHECK(norm<Norm::L2>(3, -4) == 5);
  CHECK(norm<Norm::LINFTY>(1) == 1);
  CHECK(norm<Norm::LINFTY>(3, -4) == 4);
  CHECK(norm<Norm::LINFTY>(4, -4) == 4);

  REQUIRE_THAT(norm<Norm::L2>(1, 1), WithinRel(1.4142135623730951));    // √2
  REQUIRE_THAT(norm<Norm::L2>(2, 2), WithinRel(2.8284271247461903));    // 2√2
  REQUIRE_THAT(norm<Norm::L2>(1, 1, 1), WithinRel(1.7320508075688772)); // √3
  REQUIRE_THAT(norm<Norm::L2>(2, 3), WithinRel(3.605551275463989));     // √13
}

TEST_CASE("LengthStopper tests") {
  LengthStopper stopper{5};

  SECTION("Empty walk") {
    std::vector<int> walk;
    REQUIRE_FALSE(stopper(walk));
  }

  SECTION("Walk shorter than limit") {
    std::vector<int> walk{0, 1, 2};
    REQUIRE_FALSE(stopper(walk));
  }

  SECTION("Walk exactly at limit") {
    std::vector<int> walk{0, 1, 2, 3, 4};
    REQUIRE_FALSE(stopper(walk));
  }

  SECTION("Walk longer than limit") {
    std::vector<int> walk{0, 1, 2, 3, 4, 5};
    REQUIRE(stopper(walk));
  }
}

TEST_CASE("DistanceStopper tests for different norms") {
  SECTION("L2 norm tests") {
    DistanceStopper<Norm::L2> stopper{3.0};

    SECTION("Point at origin") {
      std::vector<Point2D> walk{{0, 0}};
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point below distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {2, 2}};
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point exactly at distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {3, 0}};
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point beyond distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {2, 2}, {4, 0}};
      REQUIRE(stopper(walk));
    }
  }

  SECTION("L1 norm tests") {
    DistanceStopper<Norm::L1> stopper{4.0};

    SECTION("Point at origin") {
      std::vector<Point2D> walk{{0, 0}};
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point below distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {2, 1}}; // L1 norm = 3
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point exactly at distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {2, 2}}; // L1 norm = 4
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point beyond distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {3, 2}}; // L1 norm = 5
      REQUIRE(stopper(walk));
    }
  }

  SECTION("L-infinity norm tests") {
    DistanceStopper<Norm::LINFTY> stopper{2.0};

    SECTION("Point at origin") {
      std::vector<Point2D> walk{{0, 0}};
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point below distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {1, 1}}; // L∞ norm = 1
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point exactly at distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {2, 1}}; // L∞ norm = 2
      REQUIRE_FALSE(stopper(walk));
    }

    SECTION("Point beyond distance threshold") {
      std::vector<Point2D> walk{{0, 0}, {3, 2}}; // L∞ norm = 3
      REQUIRE(stopper(walk));
    }
  }
}
