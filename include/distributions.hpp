#pragma once

#include <cassert>
#include <cmath>
#include <concepts>
#include <cstdint>
#include <random>

#include <boost/math/distributions/pareto.hpp>
#include <stdexcept>

namespace lerw {

// generate Pareto-distributed doubles
struct Pareto {
  using result_type = double;

  std::uniform_real_distribution<> uniform{};
  boost::math::pareto_distribution<> pareto;

  explicit Pareto(double alpha) : pareto{1.0, alpha} {};

  template <std::uniform_random_bit_generator RNG>
  auto operator()(RNG &rng) -> double {
    // boost does ~ U^{-1/α}
    return boost::math::quantile(pareto, uniform(rng));
  }
};

// generate Zeta/Zipf-distributed integral values
  template <std::unsigned_integral R = std::uint32_t> struct Zipf {
  // generate a zipf distributed random variable using rejection sampling
  // for a reference see "Non Iniform Random Variate Generation" by Devroye

  using result_type = R;

  std::uniform_real_distribution<> uniform{};
  // TODO: Define alpha as alpha - 1, to be consistent with Pareto (and our
  // usage in general)
  double alpha;
  double b = std::pow(2, alpha - 1);

  explicit Zipf(double alpha_) : alpha{alpha_} {
    if (alpha <= 1.0) {
      throw std::invalid_argument{"Alpha needs to be strictly larger than 1."};
    }
  };

  template <std::uniform_random_bit_generator RNG>
  auto operator()(RNG &rng) -> R {
    while (true) {
      const long double u = uniform(rng);
      const result_type X =
          static_cast<result_type>(std::pow(u, -1.0 / (alpha - 1)));
      const long double T = std::pow(1 + 1.0 / X, alpha - 1);
      const long double v = uniform(rng);
      if (v * X * (T - 1) / (b - 1) <= T / b) {
        return X;
      }
    }
  }
};

} // namespace lerw
