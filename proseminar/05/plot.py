import matplotlib.pyplot as plt

sizes = [10, 50, 100, 200, 500, 750, 1000, 2000, 5000, 7500, 10000]
times = [0.159293, 3.76374, 14.9485, 59.3073, 362.14,
         796.893, 1395.35, 5348.05, 32282.5, 71925.5, 126811]

plt.figure(figsize=(12, 12))
plt.plot(sizes, times)
# plt.yscale('log')
plt.xlabel("Number of particles")
plt.ylabel("Execution time in ms")
plt.savefig('plot.png')
