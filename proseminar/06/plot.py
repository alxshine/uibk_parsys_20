import matplotlib.pyplot as plt
import pandas as pd


times = pd.read_csv("timings/optimized.csv")

# Execution times
plt.figure(figsize=(12, 12))
for t in [1, 2, 4, 8]:
    plt.plot(times["num_particles"], times[str(t)], label=f"{t} thread(s)")

plt.ylabel("Execution time [ms]")
plt.xlabel("Number of particles")

plt.legend()
plt.title("Execution times")
plt.savefig("plots/execution_time.png")

# Speedup
plt.figure(figsize=(12, 12))
for t in [2, 4, 8]:
    speedup = times["1"]/times[str(t)]
    plt.plot(times["num_particles"], speedup, label=f"{t} threads")

plt.ylabel("Speedup relative to single thread")
plt.xlabel("Number of particles")

plt.legend()
plt.title("Speedup")
plt.savefig("plots/speedup.png")

# Utilization
plt.figure(figsize=(12, 12))
for t in [2, 4, 8]:
    utilization = times["1"]/times[str(t)]/t
    plt.plot(times["num_particles"], utilization, label=f"{t} threads")

plt.ylabel("Utilization")
plt.xlabel("Number of particles")

plt.legend()
plt.title("Utilization")
plt.savefig("plots/utilization.png")

# imbalance mitigation
mitigation_times = pd.read_csv("timings/imbalances.csv")

sizes = mitigation_times["num_particles"]
single_thread = mitigation_times["single_thread"]
imbalanced_8 = mitigation_times["imbalanced_8"]
rebalanced_8 = mitigation_times["rebalanced_8"]

plt.figure(figsize=(12, 12))
plt.plot(sizes, single_thread, label="Imbalanced single thread")
plt.plot(sizes, imbalanced_8, label="Imbalanced 8 threads")
plt.plot(sizes, rebalanced_8, label="Rebalanced 8 threads")
plt.yscale("log")

plt.ylabel("Execution time [ms]")
plt.xlabel("Number of particles")

plt.legend()
plt.title("Imbalances")
plt.savefig("plots/imbalances.png")
