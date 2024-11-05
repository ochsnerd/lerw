import sys
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

# Read the data
data = pd.read_csv(sys.argv[1], header=None, names=["x", "y"])

# Create figure and axis
plt.figure(figsize=(10, 6))

# Plot the actual data points
plt.loglog(data["x"], data["y"], "bo-", markersize=8)

# Generate points for the theoretical line
theory_exp_2d = 5 / 4
theory_exp_3d = 1.624
theory_exp_4d = 2

first_x = data["x"].iloc[0]
first_y = data["y"].iloc[0]
scaling_factor_2d = first_y / (first_x**theory_exp_2d)
scaling_factor_3d = first_y / (first_x**theory_exp_3d)
scaling_factor_4d = first_y / (first_x**theory_exp_4d)
x_theory = np.logspace(np.log10(data["x"].min()), np.log10(data["x"].max()), 100)
y_theory_2d = scaling_factor_2d * x_theory**theory_exp_2d
y_theory_3d = scaling_factor_3d * x_theory**theory_exp_3d
y_theory_4d = scaling_factor_4d * x_theory**theory_exp_4d

# Plot the theoretical line
plt.loglog(x_theory, y_theory_2d, "g--", label="x^(5/4)")
plt.loglog(x_theory, y_theory_3d, "g--", label="x^(1.624)")
plt.loglog(x_theory, y_theory_4d, "g--", label="x^2")

# Customize the plot
plt.grid(True, which="both", ls="-", alpha=0.2)
plt.xlabel("D", fontsize=12)
plt.ylabel("L", fontsize=12)
plt.title("LERW Distance-Length comparison", fontsize=14)
plt.legend(fontsize=10)

# Ensure tight layout
plt.tight_layout()

# Save the plot
plt.savefig("scaling.png", dpi=300, bbox_inches="tight")
plt.show()
