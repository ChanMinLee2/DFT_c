import re
import matplotlib.pyplot as plt

# Load and parse the file
file_path = "result.txt"
with open(file_path, 'r', encoding='utf-8') as f:
    lines = f.readlines()

# Extract the values of Xr[n]
xr_values = []
xk_values = []
for line in lines:
    match = re.search(r'Xr\[\d+\]:\s+([-\d.]+)\s+Xk\[\d+\]:\s+([-\d.]+)', line)
    if match:
        xr_values.append(float(match.group(1)))
        xk_values.append(float(match.group(2)))

# Plotting
plt.figure(figsize=(12, 6))
# plt.plot(xr_values, label='Xk[n]')
plt.plot(xk_values, label='Xk[n]')
plt.title('Plot of Xk[n] values')
plt.xlabel('Index (n)')
plt.ylabel('Xk[n]')
plt.grid(True)
plt.legend()
plt.tight_layout()
plt.show()
