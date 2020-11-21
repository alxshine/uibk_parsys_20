#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "mpi.h"

const int shift_offset = RAND_MAX / 2;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    int seed = 1337;

    long total_steps = 1e8;
    if (argc > 1)
    {
        total_steps = atoi(argv[1]);
    }

    int num_ranks;
    MPI_Comm_size(MPI_COMM_WORLD, &num_ranks);
    if (total_steps % num_ranks)
    {
        perror("ERROR: problem size not divisible by number of ranks");
        return 1;
    }
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    long my_steps = total_steps / num_ranks;
    int inside = 0;

    double before = MPI_Wtime();

    for (long i = 0; i < my_steps; ++i)
    {
        int x = rand_r(&seed) - shift_offset;
        float xf = x * 2.f / RAND_MAX;
        int y = rand_r(&seed) - shift_offset;
        float yf = y * 2.f / RAND_MAX;

        float distance = sqrtf(xf * xf + yf * yf);
        inside += distance < 1;
    }
    float pi = 4. * (float)inside / my_steps;

    float results[num_ranks];
    MPI_Gather(&pi, 1, MPI_FLOAT, results, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

    if (!my_rank)
    {
        float final_pi = 0;
        for (int i = 0; i < num_ranks; ++i)
            final_pi += results[i];
        final_pi /= num_ranks;

        double after = MPI_Wtime();
        printf("Approximation for pi after %ld steps: %f\n", total_steps, final_pi);
        printf("Elapsed wall time: %lf seconds\n", after - before);
    }

    MPI_Finalize();
}