#include <stdio.h>
#include <stdlib.h>

#include "base.h"

int main(int argc, char const **argv)
{
    int N = 200;
    if (argc > 1)
        N = atoi(argv[1]);
    int S = N * N;
    int T = N * 500;
    printf("Computing heat distribution for room size N*N=%d for T=%d timesteps\n", S, T);

    Vector A = (Vector)malloc(sizeof(value_t) * S);
    Vector B = (Vector)malloc(sizeof(value_t) * S);

    for (int i = 0; i < S; ++i)
    {
        A[i] = 273;
    }

    int source_x = N / 4;
    int source_y = N / 4;
    size_t source_coord = source_y * N + source_x;
    A[source_coord] = heat_source;

    printf("Initial:\n");
    printTemperature_2d(A, N);
    printf("\n");

    for (int t = 0; t < T; t++)
    {

        for (size_t i = 0; i < N; ++i)
        {
            size_t y_above = i != 0 ? i - 1 : 0;
            size_t y_below = i != N - 1 ? i + 1 : N - 1;

            for (size_t j = 0; j < N; ++j)
            {
                size_t x_left = j != 0 ? j - 1 : 0;
                size_t x_right = j != N - 1 ? j + 1 : N - 1;

                value_t tc = A[i * N + j];
                value_t tl = A[i * N + x_left];
                value_t tr = A[i * N + x_right];
                value_t tu = A[y_above * N + j];
                value_t td = A[y_below * N + j];

                B[i * N + j] = tc + 0.4 * .25 * (tl + tr + tu + td - 4 * tc);
            }
        }

        B[source_coord] = heat_source;

        Vector tmp = A;
        A = B;
        B = tmp;

        //        if (t % 1000 == 0)
        //        {
        //            printf("Step t=%d:\t", t);
        //            printTemperature_2d(A, N);
        //            printf("\n");
        //        }
    }

    free(B);

    printf("Final:\n");
    printTemperature_2d(A, N);
    printf("\n");

    int success = is_valid(A, N, source_x, source_y);
    printf("Verification: %s\n", (success) ? "OK" : "FAILED");
    printf("\n");
    // for (int i = 0; i < N; ++i)
    //     printf("%f\n", A[i]);
    free(A);

    return 0;
}