#include <array>
#include <cmath>
#include <format>
#include <cstdint>
#include <functional>
#include <numeric>
#include <random>
#include <string>
#include <vector>

namespace lerw {

using field_t = int;

struct square {
  template <class T> constexpr auto operator()(T t) const -> T { return t * t; }
};
struct abs {
  template <class T> constexpr auto operator()(T t) const -> T {
    return std::abs(t);
  }
};

template <size_t Dimension> struct Vec {
  std::array<field_t, Dimension> values;

  constexpr auto operator+(const Vec<Dimension> &other) const
      -> Vec<Dimension> {
    Vec<Dimension> res{};
    for (size_t i = 0; i < Dimension; ++i)
      res.values[i] = values[i] + other.values[i];
    return res;
  }

  constexpr auto L2Sq() const -> double {
    // Don't compute the sqrt because its expensive and unnecessary
    return std::transform_reduce(values.cbegin(), values.cend(), 0, std::plus{},
                                 square{});
  }

  constexpr auto L1() const -> double {
    return std::transform_reduce(values.cbegin(), values.cend(), 0, std::plus{},
                                 abs{});
  }

  constexpr static auto Zero() -> Vec<Dimension> {
    Vec<Dimension> res{};
    for (size_t i = 0; i < Dimension; ++i)
      res.values[i] = 0;
    return res;
  }
};

using Walk = std::vector<Vec<2>>;

struct Stepper {
  static constexpr std::array<Vec<2>, 4> directions{
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
  std::uniform_int_distribution<uint8_t> distribution{0, directions.size() - 1};
  std::mt19937 gen;

  Stepper(unsigned long seed) : gen{seed} {};

  auto operator()(const Vec<2> &p) -> Vec<2> {
    return p + directions[distribution(gen)];
  }
};

struct LengthStopper {
  size_t length;

  constexpr auto operator()(const Walk &walk) const -> bool {
    return walk.size() > length;
  }
};

struct L1DistanceStopper {
  double distance;

  constexpr auto operator()(const Walk &walk) const -> bool {
    return walk.back().L1() > distance;
  }
};

struct L2DistanceStopper {
  double distance;

  constexpr auto operator()(const Walk &walk) const -> bool {
    return walk.back().L2Sq() > distance;
  }
};

template <class Step>
auto generate_fixed_length_random_walk(std::size_t length, Step stepper)
    -> std::vector<Vec<2>> {
  return generate_stopped_random_walk(LengthStopper{length}, stepper);
}

template <class Step>
auto generate_fixed_distance_L1_random_walk(double distance, Step stepper)
    -> std::vector<Vec<2>> {
  return generate_stopped_random_walk(L1DistanceStopper{distance}, stepper);
}

template <class Step>
auto generate_fixed_distance_L2Sq_random_walk(double distanceSquared, Step stepper)
    -> std::vector<Vec<2>> {
  return generate_stopped_random_walk(L2DistanceStopper{distanceSquared}, stepper);
}

template <class Step, class Stop>
auto generate_stopped_random_walk(Stop stopper, Step stepper) -> std::vector<Vec<2>> {
  Walk walk{Vec<2>::Zero()};
  // TODO: Could take hint about expected length

  while (not stopper(walk))
    walk.emplace_back(stepper(walk.back()));
  return walk;
}

constexpr auto format_walk(const Walk &walk) -> std::string {
  std::string s = "{\n";
  for (const auto &e : walk)
    s += (std::format("{} {}\n", e.values[0], e.values[1]));
  s += "}\n";
  return s;
}

} // namespace lerw
