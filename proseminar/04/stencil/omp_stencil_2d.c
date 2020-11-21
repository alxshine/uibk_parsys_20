#include <stdio.h>
#include <stdlib.h>

#include "base.h"
#include "omp.h"

int main(int argc, char const **argv)
{
    int N = 4000;
    if (argc > 1)
        N = atoi(argv[1]);
    int num_threads = 1;
    if (argc > 2)
        num_threads = atoi(argv[2]);
    int S = N * N;
    int T = 100;
    // printf("Computing heat distribution for room size N=%d for T=%d timesteps, %d threads\n", N, T, num_threads);

    Vector A = (Vector)malloc(sizeof(value_t) * S);
    Vector B = (Vector)malloc(sizeof(value_t) * S);

    for (int i = 0; i < S; ++i)
        A[i] = B[i] = 273;

    int source_x = N / 4;
    int source_y = N / 4;
    int source_coord = source_y * N + source_x;
    A[source_coord] = heat_source;

    int north = source_y - T;
    int south = source_y + T + 1;
    int west = source_x - T;
    int east = source_x + T + 1;

    north = north < 0 ? 0 : north;
    south = south > N ? N : south;
    west = west < 0 ? 0 : west;
    east = east > N ? N : east;

    // north = west = 0;
    // south = east = N;

    omp_set_dynamic(0);
    omp_set_num_threads(num_threads);

    double before = omp_get_wtime();

    for (int t = 0; t < T; t++)
    {
#pragma omp parallel for collapse(2) schedule(static)
        for (int i = north; i < south; ++i)
        {
            for (int j = west; j < east; ++j)
            {
                int y_above = i != 0 ? i - 1 : 0;
                int y_below = i != N - 1 ? i + 1 : N - 1;
                int x_left = j != 0 ? j - 1 : 0;
                int x_right = j != N - 1 ? j + 1 : N - 1;

                value_t tc = A[i * N + j];
                value_t tl = A[i * N + x_left];
                value_t tr = A[i * N + x_right];
                value_t tu = A[y_above * N + j];
                value_t td = A[y_below * N + j];

                // printf("%d: t=%d, i=%d, j=%d\n", omp_get_thread_num(), t, i, j);
                B[i * N + j] = tc + 0.4 * .25 * (tl + tr + tu + td - 4 * tc);
            }
        }

        B[source_coord] = heat_source;

        Vector tmp = A;
        A = B;
        B = tmp;
    }

    double after = omp_get_wtime();
    double elapsed = after - before;

    free(B);

    // printf("Final:\n");
    // printTemperature_2d(A, N);
    // printf("\n");

    // for (int i = 0; i < S; ++i)
    //     printf("%f\n", A[i]);
    int success = is_valid(A, N, source_x, source_y);
    // printf("valid: %d, source_x=%d, source_y=%d\n", success, source_x, source_y);
    free(A);

    FILE *performance_file = fopen("performance.csv", "a");
    if (!performance_file)
    {
        perror("Could not open performance.csv");
        return 1;
    }
    fprintf(performance_file, "%d,%d,%lf,%d\n", num_threads, N, elapsed, success);
    fclose(performance_file);

    return 0;
}