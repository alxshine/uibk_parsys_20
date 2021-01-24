# Task 1: shared memory

I downloaded and unpacked the chapel tarball to my scratch directory.
Then I `cd`'d into it, sourced `utils/quickstart.sh` (or similar), and compiled chapel.
The chapel distribution is not included in the submission.zip

The code for the parallel pi computation can be found in [pi-multinode](./pi-multinode.chpl).
It is basically a superset of the code in [pi.chpl](./pi.chpl) and simplifies to the code for the case of one locale.
The same holds true for the matrix multiplication.

I first tried using the openmp/openmpi environment to limit the number of cores that can be used by chapel.
This would've allowed me to use a single `forall` loop for the pi computation over an array of `x` and `y` coordinates.
Matrix multiplication would've been even more elegant, as I could've used `forall (i,j) in C.domain do`, which I think is a really cool language feature.
Unfortunately `here.getNumPUS()` reported 8 cores for all configurations of queue, and the times were very similar (except for 4 cores to 8 cores, at which point execution time halved as expected, but still 8 PUs were reported for both cases...)

Performance of my code is terrible, for both problems.
At some point in this lecture I got lost due to not being able to invest enough time, and whatever I try to do now to make my code faster seems to actually make it slower.
This is unfortunate, and I would have loved to put in more effort, but I have to think of the progress in my PhD first.
This is not to say I didn't invest ~10 hours into this assignment, they just haven't been fruitful at all.

I will give a brief overview of execution times together with the distributed code.

# Task 2: distributed memory

The code for my distributed versions is the same.
Splitting the problem size between locales for the pi calculation is relatively simple, as I just divide per locale, and then further by the number of PUs.
This assumes homogenous locales, but that is given on our cluster.

For the matrix multiplication I split over rows of C (my result matrix), and then again between PUs per locale.
The slice for each PU is calculated given the `id` of their locale, and the index of the current PU.

Performance for the pi calculation is terrible, but scales almost linearly with the number of processors (even over multiple locales).
The single thread version runs out of memory, as I wanted to use `fillRandom` on an array of `x` and `y` coordinates.
It works for 2 cores and more, which doesn't really make sense to me to be honest, as that still should only have the same amount of memory.
I'm not sure what kind of magic `chapel` is doing in the background.

2 cores takes roughly 1900s, 4 halves it to 950s.
8 cores reduces it to 261 seconds for some reason, and 16 then doesn't bring any improvement.
32 cores on 4 locales then brings the expected halving again at 131s.
The exact times can be found in [pi.csv](./pi.csv).

My matrix multiplication code is actually worse.
1 thread again does'nt finish, also for reasons I'm not sure about.
2 threads takes 11k seconds, 4 threads goes down to 7414s, and 8 threads to 2032s.
The 2 and 4 locale variants did not finish.