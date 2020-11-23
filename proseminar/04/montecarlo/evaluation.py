# %%

import pandas as pd
import matplotlib.pyplot as plt
import os

variants = ["atomic", "critical", "inside_reduction", "pi_reduction"]

# %%
for num_threads in [1, 2, 4, 8]:

    plt.figure(figsize=(8, 4))
    plt.title(f"{num_threads} threads")
    for variant in variants:
        path = f"performance_{variant}.csv"
        data = pd.read_csv(
            path, names=["num_threads", "total_steps", "time", "pi"])
        reduced_data = data[data['num_threads'] == num_threads]
        total_steps = reduced_data['total_steps']
        times = reduced_data['time']

        plt.plot(total_steps, times, label=f"{variant}")

    plt.legend()
    os.makedirs("images", exist_ok=True)
    plt.savefig(f"images/{num_threads}-threads.png")

# %%
for variant in variants:
    fig, axs = plt.subplots(2, 1, figsize=(12, 12))
    plt.suptitle(f"Variant: {variant}")
    axs[0].set_title("Speedup")
    axs[0].set_xlabel("Number of steps")
    axs[0].set_ylabel("Relative speedup")

    axs[1].set_title("Efficiency")
    axs[1].set_xlabel("Number of steps")
    axs[1].set_ylabel("Efficiency")

    path = f"performance_{variant}.csv"
    data = pd.read_csv(
        path, names=["num_threads", "total_steps", "time", "pi"])

    baseline = data[data['num_threads'] == 1]['time']
    baseline.reset_index(drop=True, inplace=True)

    for num_threads in [2, 4, 8]:
        reduced_data = data[data['num_threads'] == num_threads]
        total_steps = reduced_data['total_steps']
        times = reduced_data['time']
        times.reset_index(drop=True, inplace=True)
        speedup = baseline/times
        efficiency = speedup/num_threads

        axs[0].plot(total_steps, speedup, label=f"{num_threads} threads")
        axs[1].plot(total_steps, efficiency, label=f"{num_threads} threads")

    axs[0].legend()
    axs[1].legend()
    # plt.show()
    plt.savefig(f"images/{variant}.png")
