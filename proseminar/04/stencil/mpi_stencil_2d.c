#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

#include "base.h"
#define DATA_TYPE MPI_DOUBLE

const int from_lower_tag = 0;
const int from_higher_tag = 1;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int N = 40;
    if (argc > 1)
        N = atoi(argv[1]);
    int ranks_per_dim = 2;
    if (argc > 2)
        ranks_per_dim = atoi(argv[2]);
    const int S = N * N;
    const int T = 100;

    MPI_Status mpi_status;

    MPI_Comm cartesian_comm;
    const int dims[] = {ranks_per_dim, ranks_per_dim};
    const int periods[] = {0, 0};
    if (MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cartesian_comm))
    {
        perror("Could not create cartesion communicator");
        return 1;
    }

    int my_rank;
    MPI_Comm_rank(cartesian_comm, &my_rank);

    int nb_left, nb_right, nb_up, nb_down;
    MPI_Cart_shift(cartesian_comm, 0, 1, &nb_up, &nb_down);
    MPI_Cart_shift(cartesian_comm, 1, 1, &nb_left, &nb_right);

    if (!my_rank)
        printf("Computing heat distribution for room size N*N=%d for T=%d timesteps\n", S, T);

    Vector A = (Vector)malloc(sizeof(value_t) * S);
    Vector B = (Vector)malloc(sizeof(value_t) * S);

    // prepare send and receive
    int chunk_size = N / ranks_per_dim;
    int my_row = my_rank / ranks_per_dim;
    int my_col = my_rank % ranks_per_dim;

    // sending top and bottom border is easy
    MPI_Datatype top_bottom_type;
    MPI_Type_vector(1, chunk_size, 1, DATA_TYPE, &top_bottom_type);
    MPI_Type_commit(&top_bottom_type);
    // left and right requires strides
    MPI_Datatype left_right_type;
    MPI_Type_vector(chunk_size, 1, N, DATA_TYPE, &left_right_type);
    MPI_Type_commit(&left_right_type);

    for (int i = 0; i < S; ++i)
    {
        A[i] = 273;
    }

    int source_x = N / 4;
    int source_y = N / 4;
    size_t source_coord = source_y * N + source_x;
    A[source_coord] = heat_source;

    // if (!my_rank)
    // {
    //     printf("Initial:\n");
    //     printTemperature_2d(A, N);
    //     printf("\n");
    // }

    if (!my_rank)
        printf("Dims: %d*%d, chunk_size=%d, ranks_per_dim=%d\n\n", N, N, chunk_size, ranks_per_dim);
    // printf("%d: nb_up=%d, nb_down=%d, nb_left=%d, nb_right=%d\n", my_rank, nb_up, nb_down, nb_left, nb_right);

    int my_top_row_index = my_row * chunk_size * N + my_col * chunk_size;
    int my_bottom_row_index = my_top_row_index + (chunk_size - 1) * N;
    int my_left_column_index = my_top_row_index;
    int my_right_column_index = my_top_row_index + chunk_size - 1;

    int top_nb_index = my_top_row_index - N;
    int down_nb_index = my_bottom_row_index + N;
    int left_nb_index = my_left_column_index - 1;
    int right_nb_index = my_right_column_index + 1;

    // printf("%d: top_row=%d, bottom_row=%d, left_column=%d, right_column=%d, last_index=%d\n", my_rank, my_top_row_index, my_bottom_row_index, my_left_column_index, my_right_column_index, my_bottom_row_index + chunk_size - 1);
    // printf("%d: neighbors: top_row=%d, bottom_row=%d, left_column=%d, right_column=%d\n", my_rank, top_nb_index, down_nb_index, left_nb_index, right_nb_index);
    // printf("%d: up: %d, down: %d, left: %d, right: %d\n", my_rank, nb_up > 0, nb_down > 0, nb_left > 0, nb_right > 0);
    // MPI_Finalize();
    // return 0;

    for (int t = 0; t < T; t++)
    {

#pragma region exchange messages
#pragma region top bottom
        // send top if nb_up exists
        if (nb_up >= 0)
        {
            // printf("%d: sending top row at index %d to rank %d\n", my_rank, my_top_row_index, nb_up);
            MPI_Send(A + my_top_row_index, 1, top_bottom_type, nb_up, from_higher_tag, cartesian_comm);
        }
        if (nb_down > 0)
        {
            // printf("%d: receiving bottom row at index %d from rank %d\n", my_rank, down_nb_index, nb_down);
            // receive bottom if nb_down exists
            MPI_Recv(A + down_nb_index, 1, top_bottom_type, nb_down, from_higher_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive bottom neighbor row on rank %d\n", my_rank);
                return 1;
            }

            // send bottom if nb_down exists
            // printf("%d: sending bottom row at index %d to rank %d\n", my_rank, my_bottom_row_index, nb_down);
            MPI_Send(A + my_bottom_row_index, 1, top_bottom_type, nb_down, from_lower_tag, cartesian_comm);
        }

        // receive top if nb_up exists
        if (nb_up >= 0)
        {
            // printf("%d: receiving top row at index %d from rank %d\n", my_rank, down_nb_index, nb_up);
            MPI_Recv(A + top_nb_index, 1, top_bottom_type, nb_up, from_lower_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive bottom neighbor row on rank %d\n", my_rank);
                return 1;
            }
        }
#pragma endregion
        // send left nb_left exists
        if (nb_left >= 0)
        {
            MPI_Send(A + my_left_column_index, 1, left_right_type, nb_left, from_higher_tag, cartesian_comm);
        }

        if (nb_right >= 0)
        {
            MPI_Recv(A + right_nb_index, 1, left_right_type, nb_right, from_higher_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive right neighbor column on rank %d\n", my_rank);
                return 1;
            }

            MPI_Send(A + my_right_column_index, 1, left_right_type, nb_right, from_lower_tag, cartesian_comm);
        }

        if (nb_left >= 0)
        {
            MPI_Recv(A + left_nb_index, 1, left_right_type, nb_left, from_lower_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive left neighbor column on rank %d\n", my_rank);
                return 1;
            }
        }
#pragma endregion

        for (size_t i = my_row * chunk_size; i < (my_row + 1) * chunk_size; ++i)
        {
            size_t y_above = i != 0 ? i - 1 : 0;
            size_t y_below = i != N - 1 ? i + 1 : N - 1;

            for (size_t j = my_col * chunk_size; j < (my_col + 1) * chunk_size; ++j)
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

    // printf("Final:\n");
    // printTemperature_2d(A, N);
    // printf("\n");

    int success = is_valid(A, N, source_x, source_y);
    printf("%d: Verification: %s\n", my_rank, (success) ? "OK" : "FAILED");
    printf("\n");
    // for (int i = 0; i < N; ++i)
    //     printf("%f\n", A[i]);
    free(A);

    MPI_Type_free(&top_bottom_type);
    MPI_Finalize();

    return 0;
}
