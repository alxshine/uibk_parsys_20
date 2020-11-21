#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <omp.h>

const int shift_offset = RAND_MAX / 2;

int main(int argc, char **argv)
{

    long total_steps = 1e8;
    if (argc > 1)
    {
        total_steps = atoi(argv[1]);
    }
    int num_threads = 1;
    if (argc > 2)
        num_threads = atoi(argv[2]);

    double pi;
    int inside = 0;

    omp_set_dynamic(0);
    omp_set_num_threads(num_threads);

    double before = omp_get_wtime();
#pragma omp parallel
    {
        int seed = 1337 + omp_get_thread_num();
        // printf("num_threads: %d, number of threads: %d\n", num_threads, omp_get_num_threads());
#pragma omp for
        for (long i = 0; i < total_steps; ++i)
        {
            int x = rand_r(&seed) - shift_offset;
            float xf = x * 2.f / RAND_MAX;
            int y = rand_r(&seed) - shift_offset;
            float yf = y * 2.f / RAND_MAX;

            float distance = sqrtf(xf * xf + yf * yf);
#pragma critical
            inside += distance < 1;
        }
    }
    pi = 4. * (double)inside / total_steps;
    double after = omp_get_wtime();
    double elapsed = after - before;

    FILE *results = fopen("performance.csv", "a");
    if (!results)
    {
        perror("Could not open performance.csv");
        return 1;
    }

    fprintf(results, "%d,%ld,%lf,%lf\n", num_threads, total_steps, elapsed, pi);
    fclose(results);
}