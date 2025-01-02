#pragma once

#include <random>

#include <boost/math/distributions/pareto.hpp>

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

} // namespace lerw
