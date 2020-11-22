# Montecarlo simulation for PI

The omp implementation of pi calculation can be found in [pi_omp.c](pi_omp.c)

## Measure execution times with different optimizations

I measured execution with the following optimization approaches:

- atomic increment of the inside points
- incrementing inside points in a critical region
- using `reduction` on the number of inside points
- using `reduction` on the intermediate results for pi
