#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <execution>
#include <functional>
#include <numeric>
#include <optional>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

namespace lerw {

template <class T>
concept NumberGenerator = requires(T t) {
  { t() } -> std::same_as<unsigned long>;
};

template <class Generator, class U>
concept WalkGenerator = requires(Generator t) {
  { t() } -> std::same_as<std::vector<U>>;
};

template <class GeneratorFactory, class SeedGenerator, class U>
concept WalkGeneratorFactory = NumberGenerator<SeedGenerator> &&
                               requires(GeneratorFactory t, SeedGenerator n) {
                                 { t(std::move(n)) } -> WalkGenerator<U>;
                               };

template <class C, class T>
concept Indexable = requires(C container, std::size_t i) {
  { container.size() } -> std::convertible_to<std::size_t>;
  // {container[i]} -> std::same_as<T>;
  // does not work because operator[] can also return a reference

  requires std::same_as<std::remove_cvref_t<decltype(container[i])>, T>;
};

template <class T>
concept Point = std::equality_comparable<T> && requires(T p1, T p2) {
  { p1 + p2 } -> std::same_as<T>;
  { p1.L2Sq() } -> std::convertible_to<double>;
  { p1.L1() } -> std::convertible_to<double>;
  // TODO: Hashable
};

template <class T>
concept Graph = requires(T) {
  typename T::Point;
  requires Point<typename T::Point>;
  { T::Zero() } -> std::same_as<typename T::Point>;
  requires Indexable<decltype(T::Directions()), typename T::Point>;
};

template <class T, class RNG, class G>
concept StepperC = Graph<G> && NumberGenerator<RNG> &&
                   requires(T stepper, RNG rng, G::Point p) {
                     { stepper(p) } -> std::same_as<typename G::Point>;
                   };

template <class T, class P>
concept Stopper = Point<P> && requires(T t, std::vector<P> walk) {
  { t(walk) } -> std::same_as<bool>;
};

struct square {
  template <class T> constexpr auto operator()(T t) const -> T { return t * t; }
};

struct abs {
  template <class T> constexpr auto operator()(T t) const -> T {
    return std::abs(t);
  }
};

struct Lattice3D {
  struct Point {
    int x;
    int y;
    int z;

    constexpr auto operator+(const Point &other) const -> Point {
      return {x + other.x, y + other.y, z + other.z};
    }
    constexpr auto L2Sq() const -> double { return x * x + y * y + z * z; }
    constexpr auto L1() const -> double {
      const auto a = abs{};
      return a(x) + a(y) + a(z);
    }

    struct Hash {
      constexpr std::size_t operator()(const Point &vec) const {
        return std::hash<int>()(vec.x) ^ std::hash<int>()(vec.y << 16) ^
               std::hash<int>()(vec.z << 8);
      }
    };

    bool operator==(const Point &) const = default;
  };
  consteval static auto Zero() -> Point { return {0, 0, 0}; }
  consteval static auto Directions() -> std::array<Point, 8> {
    return {Point{1, 0, 0}, {-1, 0, 0}, {0, 1, 0},
            {0, -1, 0},     {0, 0, 1},  {0, 0, -1}};
  }
};

template <size_t Dimension> struct Lattice {
  using field_t = int;
  struct Point {
    std::array<field_t, Dimension> values;

    constexpr auto operator+(const Point &other) const -> Point {
      Point res{};
      for (size_t i = 0; i < Dimension; ++i)
        res.values[i] = values[i] + other.values[i];
      return res;
    }

    constexpr auto L2Sq() const -> double {
      // Don't compute the sqrt because its expensive and unnecessary
      return std::transform_reduce(values.cbegin(), values.cend(), 0,
                                   std::plus{}, square{});
    }

    constexpr auto L1() const -> double {
      return std::transform_reduce(values.cbegin(), values.cend(), 0,
                                   std::plus{}, abs{});
    }

    // TODO: specialize std::hash
    struct Hash {
      constexpr std::size_t operator()(const Point &vec) const {
        std::size_t hashValue = 0;
        for (const auto &elem : vec.values) {
          hashValue ^= std::hash<unsigned int>()(elem) + 0x9e3779b9 +
                       (hashValue << 6) + (hashValue >> 2);
        }
        return hashValue;
      }
    };

    bool operator==(const Point &) const = default;
  };

  consteval static auto Zero() -> Point {
    // TODO: std::views::repeat?
    Point res{};
    for (size_t i = 0; i < Dimension; ++i)
      res.values[i] = 0;
    return res;
  }

  consteval static auto Directions() -> std::array<Point, 2 * Dimension> {
    auto directions = std::array<Point, 2 * Dimension>{};
    for (size_t i = 0; i < 2 * Dimension; ++i) {
      directions[i].values[i / 2] = i % 2 ? -1 : 1;
    }
    return directions;
  }
};

template <NumberGenerator RNG, Graph G> struct Stepper {
  std::uniform_int_distribution<uint8_t> distribution{
      0, G::Directions().size() - 1};
  RNG rng;

  explicit Stepper(RNG &&rng) : rng{std::move(rng)} {};

  auto operator()(const G::Point &p) -> G::Point {
    return p + G::Directions()[distribution(rng)];
  }
};

struct LengthStopper {
  size_t length;

  template <class P>
  constexpr auto operator()(const std::vector<P> &walk) const -> bool {
    return walk.size() > length;
  }
};

struct L1DistanceStopper {
  double distance;

  template <Point P>
  constexpr auto operator()(const std::vector<P> &walk) const -> bool {
    return walk.back().L1() > distance;
  }
};

struct L2DistanceStopper {
  double distance_sq;

  L2DistanceStopper(double distance) : distance_sq{distance * distance} {}

  template <Point P>
  constexpr auto operator()(const std::vector<P> &walk) const -> bool {
    return walk.back().L2Sq() > distance_sq;
  }
};

template <NumberGenerator RNG, Graph G, Stopper<typename G::Point> Stop,
          StepperC<RNG, G> Step>
struct RandomWalkGenerator {
  Stop stopper;
  Step stepper;
  std::optional<size_t> expected_size = std::nullopt;

  constexpr auto operator()() -> auto {
    std::vector<typename G::Point> walk{G::Zero()};

    if (expected_size)
      walk.reserve(*expected_size);

    while (not stopper(walk))
      walk.emplace_back(stepper(walk.back()));

    return walk;
  }
};

template <NumberGenerator RNG, Graph G, Stopper<typename G::Point> Stop,
          StepperC<RNG, G> Step>
struct LoopErasedRandomWalkGenerator {
  Stop stopper;
  Step stepper;
  std::optional<size_t> expected_size = std::nullopt;

  constexpr auto operator()() -> auto {
    using Point = G::Point;
    std::unordered_set<Point, typename Point::Hash> visited{G::Zero()};
    std::vector<Point> walk{G::Zero()};

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

template <class ExecutionPolicy, NumberGenerator RNG, Graph G,
          WalkGeneratorFactory<RNG, typename G::Point> GeneratorFactory>
auto compute_average_length(ExecutionPolicy policy, RNG seedRNG,
                            GeneratorFactory generator_factory, size_t N)
    -> double {
  // sadly, ranges do not support parallel execution (yet)
  // we could write for example
  // auto total_lengths = std::views::iota(1, N)
  //   | std::views::transform([&generator_factory, &seedRNG](auto) {
  //       return generator_factory(RNG{seedRNG()});
  //     })
  //   | std::views::transform(policy, [](auto& generator){
  //       return generator().size();
  //     })
  //   | std::ranges::fold_left_first(policy, std::plus{});
  // (and hope that it can get optimized)

  std::vector<decltype(generator_factory(RNG{seedRNG()}))> generators;
  // This has to stay std::exectution::seq to prevent a race condition on the
  // seed rng
  std::generate_n(std::execution::seq, std::back_inserter(generators), N,
                  [&generator_factory, &seedRNG] {
                    return generator_factory(RNG{seedRNG()});
                  });

  auto total_lengths = std::transform_reduce(
      policy, generators.begin(), generators.end(), 0, std::plus{},
      [](auto &generator) { return generator().size(); });

  return 1.0 * total_lengths / N;
}

template <size_t Dimension>
constexpr auto
format_walk(const std::vector<typename Lattice<Dimension>::Point> &walk)
    -> std::string {
  std::string s = "{\n";
  for (const auto &e : walk) {
    for (const auto &v : e.values)
      s += std::to_string(v) + " ";
    s += '\n';
  }
  s += "}\n";
  return s;
}

} // namespace lerw
