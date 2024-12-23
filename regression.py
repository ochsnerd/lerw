import sys
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import stats

# Read the data
data = pd.read_csv(sys.argv[1], header=None, names=["x", "y"])

# Convert scientific notation to float if needed
data["x"] = data["x"].astype(float)
data["y"] = data["y"].astype(float)

# Take logarithm of both columns
log_x = np.log10(data["x"])
log_y = np.log10(data["y"])

# Perform linear regression
slope, intercept, r_value, p_value, std_err = stats.linregress(log_x, log_y)

# Create the plot
plt.figure(figsize=(10, 6))

# Plot the data points
plt.loglog(data["x"], data["y"], "bo", label="Data points")

# Plot the fitted line
x_range = np.logspace(np.log10(data["x"].min()), np.log10(data["x"].max()), 100)
y_fit = 10**intercept * x_range**slope
plt.loglog(x_range, y_fit, "r-", label=f"Fitted line (slope = {slope:.3f})")

# Add labels and title
plt.xlabel("x")
plt.ylabel("y")
plt.title("Power Law Fit (Log-Log Scale)")
plt.grid(True)
plt.legend()

# Print the results
print(f"Power law exponent (slope): {slope:.3f}")
print(f"Y-intercept: {10**intercept:.3f}")
print(f"R-squared: {r_value**2:.3f}")
print(f"Standard error: {std_err:.3f}")
print(f"P-value: {p_value:.3f}")

plt.show()
