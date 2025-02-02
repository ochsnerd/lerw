# code stolen & generated by claude

import numpy as np
import matplotlib.pyplot as plt
from typing import List, Dict, Union
from numpy.typing import NDArray
from itertools import product

from interface import Norm, get_walk_lengths


def estimate_exponent_with_errors(
    R_values: List[int], avg_lengths: List[float], n_bootstrap: int = 1000
) -> Dict[str, Union[float, NDArray[np.float64]]]:
    """
    Estimate the scaling exponent D and its uncertainty using robust bootstrap methods.

    Args:
        R_values: List of system sizes
        avg_lengths: List of average path lengths
        n_bootstrap: Number of bootstrap samples for error estimation
    """
    # Convert to numpy arrays and take logs
    log_R = np.log(np.array(R_values, dtype=np.float64))
    log_L = np.log(np.array(avg_lengths, dtype=np.float64))
    n_points = len(R_values)

    # Perform main regression on original data
    main_slope, main_intercept = np.polyfit(log_R, log_L, 1)

    # Initialize bootstrap arrays
    bootstrap_slopes = np.zeros(n_bootstrap)
    bootstrap_intercepts = np.zeros(n_bootstrap)

    # Compute residuals from main fit
    y_fit = main_slope * log_R + main_intercept
    residuals = log_L - y_fit

    # Perform parametric bootstrap using residuals
    for i in range(n_bootstrap):
        # Resample residuals
        bootstrap_residuals = np.random.choice(residuals, size=n_points, replace=True)
        # Generate new y values by adding resampled residuals to fitted line
        bootstrap_y = y_fit + bootstrap_residuals

        # Fit on bootstrap sample
        try:
            slope, intercept = np.polyfit(log_R, bootstrap_y, 1)
            bootstrap_slopes[i] = slope
            bootstrap_intercepts[i] = intercept
        except np.linalg.LinAlgError:
            # If fitting fails, use the main fit values
            bootstrap_slopes[i] = main_slope
            bootstrap_intercepts[i] = main_intercept

    # Calculate confidence intervals
    ci_slope = np.percentile(
        bootstrap_slopes[np.isfinite(bootstrap_slopes)], [2.5, 97.5]
    )
    ci_intercept = np.percentile(
        bootstrap_intercepts[np.isfinite(bootstrap_intercepts)], [2.5, 97.5]
    )

    # Calculate standard errors
    slope_stderr = np.std(bootstrap_slopes[np.isfinite(bootstrap_slopes)])
    intercept_stderr = np.std(bootstrap_intercepts[np.isfinite(bootstrap_intercepts)])

    # Calculate R-squared
    r_squared = 1 - (np.sum(residuals**2) / np.sum((log_L - np.mean(log_L)) ** 2))

    return {
        "D": float(main_slope),
        "a": float(main_intercept),
        "std_error_slope": float(slope_stderr),
        "std_error_intercept": float(intercept_stderr),
        "bootstrap_ci_slope": ci_slope,
        "bootstrap_ci_intercept": ci_intercept,
        "r_squared": float(r_squared),
        "bootstrap_std_slope": float(slope_stderr),
        "bootstrap_std_intercept": float(intercept_stderr),
        "residual_std": float(np.std(residuals)),
    }


def analyze_results(
    dim: int,
    norm: Norm,
    R_values: List[int],
    alpha: float,
    trials: int,
) -> Dict[str, Union[float, NDArray[np.float64]]]:
    avg_lengths = [
        np.mean(get_walk_lengths(dim, R, trials, alpha, norm)) for R in R_values
    ]

    # Perform analysis
    results = estimate_exponent_with_errors(R_values, avg_lengths)

    print(f"\nResults for {dim}D LERW (α={alpha:.3f}):")
    print(f"D (slope) = {results['D']:.6f} ± {results['std_error_slope']:.6f}")
    print(f"a (intercept) = {results['a']:.6f} ± {results['std_error_intercept']:.6f}")
    print(
        f"D 95% Bootstrap CI: [{results['bootstrap_ci_slope'][0]:.6f}, "
        f"{results['bootstrap_ci_slope'][1]:.6f}]"
    )
    print(f"R² = {results['r_squared']:.6f}")

    return results


def plot_alpha_dependence(
    results: Dict[str, List[float]], dimension: int, norm: Norm
) -> None:
    """Create a plot showing how D varies with alpha"""

    alpha = np.array(results["alpha"])
    D = np.array(results["D"])
    D_ci_lower = np.array(results["D_ci_lower"])
    D_ci_upper = np.array(results["D_ci_upper"])

    plt.errorbar(
        alpha,
        D,
        yerr=[D - D_ci_lower, D_ci_upper - D],
        fmt="o-",
        capsize=5,
        label=f"{dimension}D LERW, {norm}",
    )

    plt.fill_between(alpha, D_ci_lower, D_ci_upper, alpha=0.2, label="95% Bootstrap CI")

    plt.xlabel("α")
    plt.ylabel("D")
    plt.title(f"Fractal Dimension vs Alpha LERW")
    plt.grid(True, alpha=0.3)
    plt.legend(loc="lower right")

    stats_text = (
        f"Dimension: {dimension}D\n"
        f"α range: [{min(alpha):.2f}, {max(alpha):.2f}]\n"
        f"D range: [{min(D):.2f}, {max(D):.2f}]\n"
        f"CI width: [{min(D_ci_upper - D_ci_lower):.3f}, {max(D_ci_upper - D_ci_lower):.3f}]"
    )
    plt.text(
        0.05,
        0.95,
        stats_text,
        transform=plt.gca().transAxes,
        verticalalignment="top",
        bbox=dict(facecolor="white", alpha=0.8),
    )

    plt.ylim(min(D_ci_lower) - 0.1, max(D_ci_upper) + 0.1)

    plt.savefig(f"lerw_alpha_dependence_{dimension}d.png", dpi=300, bbox_inches="tight")


def show_alpha_dependence(
    dim: int,
    norm: Norm,
    R_values: List[int],
    alpha_values: List[int],
    trials: int,
):
    results = {"alpha": [], "D": [], "D_err": [], "D_ci_lower": [], "D_ci_upper": []}

    for alpha in alpha_values:
        analysis_results = analyze_results(dim, norm, R_values, alpha, trials)

        results["alpha"].append(alpha)
        results["D"].append(analysis_results["D"])
        results["D_err"].append(analysis_results["std_error_slope"])
        results["D_ci_lower"].append(analysis_results["bootstrap_ci_slope"][0])
        results["D_ci_upper"].append(analysis_results["bootstrap_ci_slope"][1])

    plot_alpha_dependence(results, dim, norm)


def generate_R_values(min_size: int, max_size: int, num_sizes: int) -> List[int]:
    """Generate system sizes in geometric progression"""
    return [
        int(min_size * (max_size / min_size) ** (i / (num_sizes - 1)))
        for i in range(num_sizes)
    ]


def main():
    config = {
        "min_size": 500,
        "max_size": 2_000,
        "num_sizes": 4,
        "alpha_min": 0.2,
        "alpha_max": 2.0,
        "alpha_steps": 10,
    }

    # Generate parameter arrays
    R_values = generate_R_values(
        config["min_size"], config["max_size"], config["num_sizes"]
    )
    alpha_values = np.linspace(
        config["alpha_min"], config["alpha_max"], config["alpha_steps"]
    )

    plt.figure(figsize=(10, 6))
    x_range = np.array([min(alpha_values), max(alpha_values)])
    plt.plot(x_range, x_range, "--", color="gray", alpha=0.7, label="α = D")
    for dim, norm in product([2, 3], [Norm.L1, Norm.L2, Norm.LINF]):
        show_alpha_dependence(dim, norm, R_values, alpha_values, 500)
    plt.show()


if __name__ == "__main__":
    main()
