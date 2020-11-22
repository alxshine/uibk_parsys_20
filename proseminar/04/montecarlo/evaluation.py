# %%

import pandas as pd
import matplotlib.pyplot as plt
import os

# %%
variants = ["atomic", "critical", "inside_reduction", "pi_reduction"]
for num_threads in [1, 2, 4, 8]:

    plt.figure(figsize=(8, 4))
    plt.title(f"{num_threads} threads")
    # plt.title(f"{variant}")
    for variant in variants:
        path = f"performance_{variant}.csv"
        data = pd.read_csv(
            path, names=["num_threads", "total_steps", "time", "pi"])
        reduced_data = data[data['num_threads'] == num_threads]
        total_steps = reduced_data['total_steps']
        times = reduced_data['time']

        plt.plot(total_steps, times, label=f"{variant}")

    # plt.yscale("log")
    plt.legend()
    os.makedirs("images", exist_ok=True)
    # plt.savefig(f"images/{variant}.png")
    plt.savefig(f"images/{num_threads}-threads.png")
