#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef double *Matrix;

void dump_matrix(Matrix m, unsigned int n)
{
    for (unsigned int i = 0; i < n; ++i)
    {
        for (unsigned int j = 0; j < n; ++j)
        {
            printf("%lf, ", m[i * n + j]);
        }
        printf("\n");
    }
}

void multiply_matrices(const Matrix lh, const Matrix rh, Matrix result, unsigned int n)
{
    Matrix rh_transposed = malloc(n * n * sizeof(double));
#pragma omp parallel for collapse(2)
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            rh_transposed[i * n + j] = rh[i * n + j];
            result[i * n + j] = 0;
        }
    }

#pragma omp parallel for collapse(2)
    for (unsigned int i = 0; i < n; ++i)
    {
        for (unsigned int j = 0; j < n; ++j)
        {
            for (unsigned int k = 0; k < n; ++k)
            {
                result[i * n + j] += lh[i * n + k] *
                                     rh_transposed[j * n + k];
            }
        }
    }
    free(rh_transposed);
}

int verify_matrices_identical(Matrix a, Matrix b, unsigned int n)
{
    for (unsigned int i = 0; i < n * n; i++)
    {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

int main(int argc, char **argv)
{
    unsigned int N = 3000;
    if (argc > 1)
        N = atoi(argv[1]);
    size_t binary_size = N * N * sizeof(double);

    printf("Allocating matrices\n");
    Matrix A = malloc(binary_size);
    Matrix B = malloc(binary_size);
    Matrix C = malloc(binary_size);

    if (!A || !B || !C)
    {
        perror("Could not allocate matrices");
        return 1;
    }
    printf("Success\n");

    printf("Initializing matrices\n");
    for (unsigned int i = 0; i < N; i++)
    {
        for (unsigned int j = 0; j < N; j++)
        {
            A[i * N + j] = i * j;
            B[i * N + j] = i == j;
        }
    }
    printf("Success\n");

    printf("Multiplying Matrices\n");
    double before_time = omp_get_wtime();
    multiply_matrices(A, B, C, N);
    double after_time = omp_get_wtime();
    printf("Success\n");

    printf("Verifying correct result\n");
    if (verify_matrices_identical(A, C, N))
        printf("Success\n");
    else
        printf("ERROR: A != C\n");

    printf("Freeing allocated memory\n");
    free(A);
    free(B);
    free(C);
    printf("Success\n");

    printf("Execution took %lf seconds\n", after_time - before_time);
    return 0;
}
