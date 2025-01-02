#include <catch2/catch_test_macros.hpp>
#include <cmath>

#include "array_point.hpp"
#include "concepts.hpp"
#include "hash_set.hpp"
#include "point.hpp"
#include "utils.hpp"

using namespace lerw;

template <point P> void check1D() {
  static_assert(dim<P>() == 1);

  CHECK(P{1} + P{1} == P{2});
  CHECK(P{1} + P{-1} == P{0});

  CHECK(zero<P>() == P{0});

  CHECK(norm<Norm::L1>(P{1}) == 1);
  CHECK(norm<Norm::L1>(P{-1}) == 1);
  CHECK(norm<Norm::L1>(P{-5}) == 5);

  CHECK(norm<Norm::L2>(P{1}) == 1);
  CHECK(norm<Norm::L2>(P{-1}) == 1);
  CHECK(norm<Norm::L2>(P{-5}) == 5);

  CHECK(norm<Norm::LINFTY>(P{1}) == 1);
  CHECK(norm<Norm::LINFTY>(P{-1}) == 1);
  CHECK(norm<Norm::LINFTY>(P{-5}) == 5);

  const auto v = std::vector<int>{1};
  CHECK(constructor<P>{}(v.cbegin(), v.cend()) == P{1});

  hash_set<P> set{};
  set.insert(P{1});
  set.insert(P{2});
  CHECK(set.size() == 2);
  set.insert(P{2});
  CHECK(set.size() == 2);
  set.insert(P{3});
  CHECK(set.size() == 3);
}

template <point P> void check2D() {
  static_assert(dim<P>() == 2);

  CHECK(P{1, 1} + P{1, 2} == P{2, 3});
  CHECK(P{1, 1} + P{-1, -1} == P{0, 0});
  CHECK(zero<P>() == P{0, 0});

  CHECK(norm<Norm::L1>(P{1, 1}) == 2);
  CHECK(norm<Norm::L1>(P{-1, -1}) == 2);
  CHECK(norm<Norm::L1>(P{-5, 3}) == 8);

  CHECK(norm<Norm::L2>(P{1, 0}) == 1);
  CHECK(norm<Norm::L2>(P{-1, 0}) == 1);
  CHECK(norm<Norm::L2>(P{3, 4}) == 5);

  CHECK(norm<Norm::LINFTY>(P{1, 1}) == 1);
  CHECK(norm<Norm::LINFTY>(P{-1, -2}) == 2);
  CHECK(norm<Norm::LINFTY>(P{-5, 3}) == 5);

  // Vector construction
  const auto v = std::vector<int>{1, 2};
  CHECK(constructor<P>{}(v.cbegin(), v.cend()) == P{1, 2});

  // Set operations
  gtl::flat_hash_set<P> set{};
  set.insert(P{1, 1});
  set.insert(P{2, 2});
  CHECK(set.size() == 2);
  set.insert(P{2, 2}); // Duplicate insertion
  CHECK(set.size() == 2);
  set.insert(P{1, 2}); // New unique point
  CHECK(set.size() == 3);
}

template <point P> void check3D() {
  static_assert(dim<P>() == 3);

  CHECK(P{1, 1, 1} + P{1, 2, 3} == P{2, 3, 4});
  CHECK(P{1, 1, 1} + P{-1, -1, -1} == P{0, 0, 0});
  CHECK(zero<P>() == P{0, 0, 0});

  CHECK(norm<Norm::L1>(P{1, 1, 1}) == 3);
  CHECK(norm<Norm::L1>(P{-1, -1, -1}) == 3);
  CHECK(norm<Norm::L1>(P{-5, 3, 2}) == 10);

  CHECK(norm<Norm::L2>(P{1, 0, 0}) == 1);
  CHECK(norm<Norm::L2>(P{-1, 0, 0}) == 1);
  CHECK(norm<Norm::L2>(P{3, 4, 0}) == 5);
  CHECK(norm<Norm::L2>(P{2, 3, 6}) == 7);

  CHECK(norm<Norm::LINFTY>(P{1, 1, 1}) == 1);
  CHECK(norm<Norm::LINFTY>(P{-1, -2, -3}) == 3);
  CHECK(norm<Norm::LINFTY>(P{-5, 3, 4}) == 5);

  const auto v = std::vector<int>{1, 2, 3};
  CHECK(constructor<P>{}(v.cbegin(), v.cend()) == P{1, 2, 3});

  gtl::flat_hash_set<P> set{};
  set.insert(P{1, 1, 1});
  set.insert(P{2, 2, 2});
  CHECK(set.size() == 2);
  set.insert(P{2, 2, 2});
  CHECK(set.size() == 2);
  set.insert(P{1, 2, 3});
  CHECK(set.size() == 3);
}

template <point P> void check4D() {
  static_assert(dim<P>() == 4);

  CHECK(P{1, 1, 1, 1} + P{1, 2, 3, 4} == P{2, 3, 4, 5});
  CHECK(P{1, 1, 1, 1} + P{-1, -1, -1, -1} == P{0, 0, 0, 0});
  CHECK(zero<P>() == P{0, 0, 0, 0});

  CHECK(norm<Norm::L1>(P{1, 1, 1, 1}) == 4);
  CHECK(norm<Norm::L1>(P{-1, -1, -1, -1}) == 4);
  CHECK(norm<Norm::L1>(P{-5, 3, 2, 2}) == 12);

  CHECK(norm<Norm::L2>(P{1, 0, 0, 0}) == 1);
  CHECK(norm<Norm::L2>(P{-1, 0, 0, 0}) == 1);
  CHECK(norm<Norm::L2>(P{3, 4, 0, 0}) == 5);
  CHECK(norm<Norm::L2>(P{2, 3, 6, 0}) == 7);
  CHECK(norm<Norm::L2>(P{1, 2, 2, 4}) == 5);

  CHECK(norm<Norm::LINFTY>(P{1, 1, 1, 1}) == 1);
  CHECK(norm<Norm::LINFTY>(P{-1, -2, -3, -4}) == 4);
  CHECK(norm<Norm::LINFTY>(P{-5, 3, 4, 2}) == 5);

  const auto v = std::vector<int>{1, 2, 3, 4};
  CHECK(constructor<P>{}(v.cbegin(), v.cend()) == P{1, 2, 3, 4});

  gtl::flat_hash_set<P> set{};
  set.insert(P{1, 1, 1, 1});
  set.insert(P{2, 2, 2, 2});
  CHECK(set.size() == 2);
  set.insert(P{2, 2, 2, 2});
  CHECK(set.size() == 2);
  set.insert(P{1, 2, 3, 4});
  CHECK(set.size() == 3);
}

TEST_CASE("Points") {
  check1D<int>();
  check1D<Point1D>();
  check1D<ArrayPoint<1>>();

  check2D<Point2D>();
  check2D<ArrayPoint<2>>();

  check3D<Point3D>();
  check3D<ArrayPoint<3>>();

  check4D<ArrayPoint<4>>();
}
