# Assignment 6: optimized N-body

## Exercise 1

### Parallel implementation

I didn't implement a full Barne-Hut algorithm.
Instead, I split the full simulation area in 64 blocks, and assigned each particle a block based on their starting position.
A single step is computed in two parallelized loops (three with balancing):

1. compute interaction between particles in the same block, and compute the center of mass of the block
1. interact particle in a block with the centers of mass of each other block

By splitting this in two parallelized loops, there is an implicit barrier in between, and the centers of mass are updated correcly.

### Measure the speedup

The following is a plot of the speedup.
Because the parallelized algorithm is much faster, even with only one thread, that is the baseline I will compare the speedup to.

![speedup plot](plots/speedup.png)

And a plot of the utilization:

![utiliaztion plot](plots/utilization.png)

The utilization is relatively high, above 80% for problem sizes above 4000 particles (for 8 threads).
I haven't looked at the exact value, but it seems to stabilize at around 90%.
