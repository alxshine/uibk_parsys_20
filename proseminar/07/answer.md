# Assignment 7

## Task 1

### Hybrid parallel version

#### Implement a hybrid parallel implementation of a 2D heat stencil

I had previously implemented a 2D stencil using MPI, so I based my implementation off of that.
Additionally for this week, I imprvode the message passing to remove the unnecessary enforcement of sequentiality and reduced it to a two-step scheme like in the lecture (thanks for that tip).
The "local" computation is parallelized using an OMP parallel for loop.
It can be found in [mpi_omp_hybrid.c](mpi_omp_hybrid.c).

#### How does performance compare to your MPI only version

The hybrid version for the posted problem size of 4000\*4000 for 100 timesteps took 1.445252 seconds on the cluster.
The configuration for that was running on `openmpi-8perhost 32`, with `mpiexec -n4` and a 2x2 cartesian communicator.
Running the pure MPI version with the same configuration takes 5.626182 seconds.

I had a lot of trouble with communication deadlocks, and after a few hours of unsuccesful printf debugging I gave up to be honest.
For that reason I couldn't run the pure MPI implementation utilizing all cores.
Consequently, the varying rank setup was also not really possible for me...

#### Implement error handling

Error handling is present in both the hybrid and MPI only variant.
Note that to catch errors during the creation of the cartesian communicator the error handler of the global communicator must first be change to MPI_ERRORS_RETURN.

I'm pretty sure the versions present should catch all errors.
At least it catches all errors I encountered.

## Task 2

After the trouble I had with the communication code for Task 1, I didn't want to rewrite it all to deal with the ranks skipped due to being allocated on the same node.
