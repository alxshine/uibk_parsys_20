# %%
import pandas as pd
import matplotlib.pyplot as plt

# %%

data = pd.read_csv(f"performance_full.csv", names=[
                   'num_threads', 'N', 'time', 'valid'])

plt.figure(figsize=(12, 12))
plt.title("Execution times without optimization")

for num_threads in [1, 2, 4, 8]:
    reduced_data = data[data['num_threads'] == num_threads]
    ns = reduced_data['N']
    times = reduced_data['time']

    plt.plot(ns, times, label=f"{num_threads} threads")

plt.legend()
# plt.show()
plt.savefig('performance_full.png')
