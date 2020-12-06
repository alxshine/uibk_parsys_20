import matplotlib.pyplot as plt
import pandas as pd


times = pd.read_csv("timings/optimized.csv")

# Execution times
plt.figure(figsize=(12, 12))
for t in [1, 2, 4, 8]:
    plt.plot(times["num_particles"], times[str(t)], label=f"{t} thread(s)")

plt.legend()
plt.title("Execution times")
plt.savefig("plots/execution_time.png")

# Speedup
plt.figure(figsize=(12, 12))
for t in [2, 4, 8]:
    speedup = times["1"]/times[str(t)]
    plt.plot(times["num_particles"], speedup, label=f"{t} threads")

plt.legend()
plt.title("Speedup")
plt.savefig("plots/speedup.png")

# Utilization
plt.figure(figsize=(12, 12))
for t in [2, 4, 8]:
    utilization = times["1"]/times[str(t)]/t
    plt.plot(times["num_particles"], utilization, label=f"{t} threads")

plt.legend()
plt.title("Utilization")
plt.savefig("plots/utilization.png")