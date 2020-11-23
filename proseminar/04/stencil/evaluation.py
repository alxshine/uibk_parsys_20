# %%
import pandas as pd
import matplotlib.pyplot as plt

# %%

for variant in ['full', 'optimized']:
    data = pd.read_csv(f"performance_{variant}.csv", names=[
        'num_threads', 'N', 'time', 'valid'])

    fig, axs = plt.subplots(2, 1, figsize=(12, 12))
    plt.suptitle(f"Performance plot for variant: {variant}")
    axs[0].set_title("Speedup")
    axs[0].set_xlabel("Number of steps")
    axs[0].set_ylabel("Relative speedup")

    axs[1].set_title("Efficiency")
    axs[1].set_xlabel("Number of steps")
    axs[1].set_ylabel("Efficiency")

    baseline = data[data['num_threads'] == 1]['time'].reset_index(drop=True)

    for num_threads in [2, 4, 8]:
        reduced_data = data[data['num_threads'] == num_threads]
        ns = reduced_data['N']
        times = reduced_data['time'].reset_index(drop=True)
        speedup = baseline/times
        efficiency = speedup/num_threads

        axs[0].plot(ns, speedup, label=f"{num_threads} threads")
        axs[1].plot(ns, efficiency, label=f"{num_threads} threads")

    plt.legend()
    # plt.show()
    plt.savefig(f'performance_{variant}.png')
