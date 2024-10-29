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
theory_exp = 5 / 4
last_x = data["x"].iloc[-1]
last_y = data["y"].iloc[-1]
scaling_factor = last_y / (last_x**theory_exp)
x_theory = np.logspace(np.log10(data["x"].min()), np.log10(data["x"].max()), 100)
y_theory = scaling_factor * x_theory**theory_exp

# Plot the theoretical line
plt.loglog(x_theory, y_theory, "g--")

# Customize the plot
plt.grid(True, which="both", ls="-", alpha=0.2)
plt.xlabel("D", fontsize=12)
plt.ylabel("L", fontsize=12)
plt.title("2D LERW Distance-Length comparison", fontsize=14)
# plt.legend(fontsize=10)

# Ensure tight layout
plt.tight_layout()

# Save the plot
plt.savefig("scaling.png", dpi=300, bbox_inches="tight")
plt.show()
