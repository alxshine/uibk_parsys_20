# N Queens

Find the task parallelized version of n-queens in [nqueens.c](nqueens.c).
The sequential version is slightly slower than other solutions.

The parallelized version is slower than sequential by a factor of 100 (almost exactly 100, which is weird).
I assume this is due to the threads blocking each other, but I don't know where that is happening.
