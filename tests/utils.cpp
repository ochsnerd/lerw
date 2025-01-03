#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "utils.hpp"

using namespace lerw;
using Catch::Matchers::WithinRel;

TEST_CASE("Norms") {
  // I could do that. Do I want to do that?
  // static_assert(norm<Norm::L1>(1) == 1);

  CHECK(norm<Norm::L1>(1) == 1);
  CHECK(norm<Norm::L1>(1, -3) == 4);
  CHECK(norm<Norm::L2>(1) == 1);
  CHECK(norm<Norm::L2>(3, -4) == 5);
  CHECK(norm<Norm::LINF>(1) == 1);
  CHECK(norm<Norm::LINF>(3, -4) == 4);
  CHECK(norm<Norm::LINF>(4, -4) == 4);

  REQUIRE_THAT(norm<Norm::L2>(1, 1), WithinRel(1.4142135623730951));    // √2
  REQUIRE_THAT(norm<Norm::L2>(2, 2), WithinRel(2.8284271247461903));    // 2√2
  REQUIRE_THAT(norm<Norm::L2>(1, 1, 1), WithinRel(1.7320508075688772)); // √3
  REQUIRE_THAT(norm<Norm::L2>(2, 3), WithinRel(3.605551275463989));     // √13
}
