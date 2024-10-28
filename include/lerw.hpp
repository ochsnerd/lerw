#include <array>
#include <cstdint>
#include <random>
#include <vector>

namespace lerw {
using field_t = int;

struct Stepper {
  // TODO: Can we make the dimension a template argument?
  static constexpr std::array<std::array<field_t, 2>, 4> directions{
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}};
  std::uniform_int_distribution<uint8_t> distribution{0, directions.size() - 1};
  std::mt19937 gen;

  Stepper(unsigned long seed) : gen{seed} {};

  auto operator()(const std::array<field_t, 2> &p) -> std::array<field_t, 2> {
    auto step = directions[distribution(gen)];
    return {p[0] + step[0], p[1] + step[1]};
  }
};

template <class Step>
auto generate_random_walk(std::size_t length, Step step)
    -> std::vector<std::array<field_t, 2>> {
  std::vector<std::array<int, 2>> walk{{0, 0}};

  while (walk.size() < length) {
    walk.emplace_back(step(walk.back()));
  }
  return walk;
}

} // namespace lerw
