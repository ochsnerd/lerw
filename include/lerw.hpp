#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <execution>
#include <format>
#include <functional>
#include <numeric>
#include <optional>
#include <print>
#include <random>
#include <string>
#include <unordered_set>
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

  struct Hash {
    constexpr std::size_t operator()(const Vec<Dimension> &vec) const {
      std::size_t hashValue = 0;
      for (const auto &elem : vec.values) {
        hashValue ^= std::hash<unsigned int>()(elem) + 0x9e3779b9 +
                     (hashValue << 6) + (hashValue >> 2);
      }
      return hashValue;
    }
  };

  bool operator==(const Vec<Dimension> &) const = default;
};

using Walk = std::vector<Vec<2>>;

struct Stepper {
  // TODO: These should go into Vec
  static constexpr std::array<Vec<2>, 4> directions{
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
  std::uniform_int_distribution<uint8_t> distribution{0, directions.size() - 1};
  std::mt19937 gen;

  explicit Stepper(unsigned seed) : gen{seed} {};

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
  double distance_sq;

  L2DistanceStopper(double distance) : distance_sq{distance * distance} {}

  constexpr auto operator()(const Walk &walk) const -> bool {
    return walk.back().L2Sq() > distance_sq;
  }
};

template <class Step>
auto generate_fixed_length_random_walk(std::size_t length, Step stepper)
    -> std::vector<Vec<2>> {
  return generate_stopped_random_walk(LengthStopper{length}, stepper, length);
}

template <class Step>
auto generate_fixed_distance_L1_random_walk(double distance, Step stepper)
    -> std::vector<Vec<2>> {
  return generate_stopped_random_walk(L1DistanceStopper{distance}, stepper);
}

template <class Step>
auto generate_fixed_distance_L2Sq_random_walk(double distanceSquared,
                                              Step stepper)
    -> std::vector<Vec<2>> {
  return generate_stopped_random_walk(L2DistanceStopper{distanceSquared},
                                      stepper);
}

template <class Step, class Stop> struct RandomWalkGenerator {
  Stop stopper;
  Step stepper;
  std::optional<size_t> expected_size = std::nullopt;

  constexpr auto operator()() -> Walk {
    Walk walk{Vec<2>::Zero()};

    if (expected_size)
      walk.reserve(*expected_size);

    while (not stopper(walk))
      walk.emplace_back(stepper(walk.back()));

    return walk;
  }
};

template <class Step, class Stop> struct LoopErasedRandomWalkGenerator {
  Stop stopper;
  Step stepper;
  std::optional<size_t> expected_size = std::nullopt;

  constexpr auto operator()() -> Walk {
    std::unordered_set<Vec<2>, Vec<2>::Hash> visited{Vec<2>::Zero()};
    Walk walk{Vec<2>::Zero()};

    if (expected_size) {
      // Measure: theres also rehash, and bucket_size
      visited.reserve(*expected_size);
      walk.reserve(*expected_size);
    }

    while (not stopper(walk)) {
      auto proposed = stepper(walk.back());
      auto [_, inserted] = visited.insert(proposed);

      // Measure: [[likely]]
      if (inserted) [[likely]] {
        walk.emplace_back(std::move(proposed));
        continue;
      }

      while (walk.back() != proposed) {
        visited.erase(walk.back());
        walk.pop_back();
      }
    }

    return walk;
  }
};

template <class GeneratorFactory>
auto compute_average_length(GeneratorFactory generator_factory, size_t N)
    -> double {
  std::vector<size_t> lengths(N);

  // TODO: Ugly?
  std::vector<decltype(generator_factory(std::random_device{}()))> generators;
  std::generate_n(std::execution::seq, std::back_inserter(generators), N,
                  [&generator_factory] {
                    return generator_factory(std::random_device{}());
                  });

  std::transform(std::execution::par_unseq, generators.begin(),
                 generators.end(), lengths.begin(),
                 [](auto &generator) { return generator().size(); });

  return 1.0 *
         std::reduce(std::execution::par_unseq, lengths.cbegin(),
                     lengths.cend(), 0, std::plus{}) /
         N;
}

constexpr auto format_walk(const Walk &walk) -> std::string {
  std::string s = "{\n";
  for (const auto &e : walk)
    s += (std::format("{} {}\n", e.values[0], e.values[1]));
  s += "}\n";
  return s;
}

} // namespace lerw
