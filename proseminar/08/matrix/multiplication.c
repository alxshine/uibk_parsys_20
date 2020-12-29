#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef double *Matrix;

void multiply_matrices(const Matrix lh, const Matrix rh, Matrix result, unsigned int height, unsigned int width)
{
    for (unsigned int i = 0; i < height; ++i)
    {
        for (unsigned int j = 0; j < width; ++j)
        {
            result[i * width + j] = 0;

            for (unsigned int k = 0; k < height; ++k)
            {
                result[i * width + j] += lh[i * width + k] * rh[k * width + j];
            }
        }
    }
}

int verify_matrices_identical(Matrix a, Matrix b, unsigned int height, unsigned int width)
{
    for (unsigned int i = 0; i < height * width; i++)
    {
        if (a[i] != b[i])
            return 0;
    }
    return 1;
}

void dump_matrix(Matrix m, int height, int width)
{
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            printf("%lf, ", m[i * width + j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    unsigned int N = 10;
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
    multiply_matrices(A, B, C, N, N);
    printf("Success\n");

    printf("Verifying correct result\n");
    if (verify_matrices_identical(A, C, N, N))
        printf("Success\n");
    else
        printf("ERROR: A != C\n");

    printf("Freeing allocated memory\n");
    free(A);
    free(B);
    free(C);
    printf("Success\n");
    return 0;
}
