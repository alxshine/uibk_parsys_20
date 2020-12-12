#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

#include "base.h"
#define DATA_TYPE MPI_DOUBLE

const int bogus_message_tag = 0;

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);

    int N = 200;
    if (argc > 1)
        N = atoi(argv[1]);
    int multi_thread = 0;
    if (argc > 2)
        multi_thread = 1;
    const int S = N * N;
    const int T = 100;

    int dim_y = 4;
    int dim_x = 2;

    MPI_Status mpi_status;

    MPI_Comm cartesian_comm;
    int dims[] = {1, 1};
    if (multi_thread)
    {
        dims[0] = dim_y;
        dims[1] = dim_x;
    }
    const int periods[] = {1, 1}; // enable periodicity
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

#ifdef DEBUG_NEIGHBORS
    printf("%d: nb_up=%d, nb_down=%d, nb_left=%d, nb_right=%d\n", my_rank, nb_up, nb_down, nb_left, nb_right);
    MPI_Finalize();
    return 0;
#endif

    if (!my_rank)
        printf("Computing heat distribution for room size N*N=%d for T=%d timesteps\n", S, T);

    Vector A = (Vector)malloc(sizeof(value_t) * S);
    Vector B = (Vector)malloc(sizeof(value_t) * S);

    // prepare send and receive
    int chunk_size_x, chunk_size_y, my_row, my_col;
    if (multi_thread)
    {
        chunk_size_x = N / dim_x;
        chunk_size_y = N / dim_y;
        my_row = my_rank / dim_x;
        my_col = my_rank % dim_x;
    }
    else
    {
        chunk_size_x = chunk_size_y = N;
        my_row = my_col = 0;
    }

    // sending top and bottom border is easy
    MPI_Datatype top_bottom_type;
    MPI_Type_vector(1, chunk_size_x, 1, DATA_TYPE, &top_bottom_type);
    MPI_Type_commit(&top_bottom_type);
    // left and right requires strides
    MPI_Datatype left_right_type;
    MPI_Type_vector(chunk_size_y, 1, N, DATA_TYPE, &left_right_type);
    MPI_Type_commit(&left_right_type);
    //chunk vector for returning
    MPI_Datatype full_chunk_type;
    MPI_Type_vector(chunk_size_y, chunk_size_x, N, DATA_TYPE, &full_chunk_type);
    MPI_Type_commit(&full_chunk_type);

    for (int i = 0; i < S; ++i)
    {
        A[i] = 273;
    }

    // hard coded heat source coordinates
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
        printf("Dims: %d*%d, chunk_size=%dx%d, %d threads\n\n", N, N, chunk_size_y, chunk_size_x, multi_thread ? 8 : 1);
    // printf("%d: nb_up=%d, nb_down=%d, nb_left=%d, nb_right=%d\n", my_rank, nb_up, nb_down, nb_left, nb_right);

    int my_top_row_index = my_row * chunk_size_y * N + my_col * chunk_size_x;
    int my_bottom_row_index = my_top_row_index + (chunk_size_y - 1) * N;
    int my_left_column_index = my_top_row_index;
    int my_right_column_index = my_top_row_index + chunk_size_x - 1;

    int top_nb_index = my_top_row_index - N;
    int down_nb_index = my_bottom_row_index + N;
    int left_nb_index = my_left_column_index - 1;
    int right_nb_index = my_right_column_index + 1;

    // fix periodic boundaries
    if (my_row == 0)
        top_nb_index += S;
    if (my_row == dim_y - 1)
        down_nb_index -= S;
    if (my_col == 0)
        left_nb_index += N;
    if (my_col == dim_x - 1)
        right_nb_index -= N;

#ifdef debug_coordinates
    printf("%d: top_row=%d, bottom_row=%d, left_column=%d, right_column=%d, last_index=%d\n", my_rank, my_top_row_index, my_bottom_row_index, my_left_column_index, my_right_column_index, my_bottom_row_index + chunk_size_x - 1);
    printf("%d: neighbors: top_row=%d, bottom_row=%d, left_column=%d, right_column=%d\n", my_rank, top_nb_index, down_nb_index, left_nb_index, right_nb_index);
    printf("%d: up: %d, down: %d, left: %d, right: %d\n", my_rank, nb_up > 0, nb_down > 0, nb_left > 0, nb_right > 0);
    MPI_Finalize();
    return 0;
#endif

    double before = MPI_Wtime();

    for (int t = 0; t < T; t++)
    {
#pragma region exchange messages
#pragma region top bottom
        if (my_row % 2 == 0)
        {
            // if we are an even row, send first, then receive
            // neighbor up
            // printf("%d, sending top row to nb_up %d\n", my_rank, nb_up);
            MPI_Send(A + my_top_row_index, 1, top_bottom_type, nb_up, bogus_message_tag, cartesian_comm);
            MPI_Recv(A + top_nb_index, 1, top_bottom_type, nb_up, bogus_message_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive top neighbor row on rank %d\n", my_rank);
                return 1;
            }

            // neighbor down
            MPI_Send(A + my_bottom_row_index, 1, top_bottom_type, nb_down, bogus_message_tag, cartesian_comm);
            MPI_Recv(A + down_nb_index, 1, top_bottom_type, nb_down, bogus_message_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive bottom neighbor row on rank %d\n", my_rank);
                return 1;
            }
        }
        else
        {
            // if we are an odd row, receive first, then send
            // neighbor down
            // printf("%d, receiving below row from nb_down %d\n", my_rank, nb_down);
            MPI_Recv(A + down_nb_index, 1, top_bottom_type, nb_down, bogus_message_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive bottom neighbor row on rank %d\n", my_rank);
                return 1;
            }
            MPI_Send(A + my_bottom_row_index, 1, top_bottom_type, nb_down, bogus_message_tag, cartesian_comm);

            // neighbor up
            MPI_Recv(A + top_nb_index, 1, top_bottom_type, nb_up, bogus_message_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive top neighbor row on rank %d\n", my_rank);
                return 1;
            }
            MPI_Send(A + my_top_row_index, 1, top_bottom_type, nb_up, bogus_message_tag, cartesian_comm);
        }

        // same for columns
        if (my_col % 2 == 0)
        {
            //neighbor left
            // printf("%d: sending to left neighbor %d\n", my_rank, nb_left);
            MPI_Send(A + my_left_column_index, 1, left_right_type, nb_left, bogus_message_tag, cartesian_comm);
            MPI_Recv(A + left_nb_index, 1, left_right_type, nb_left, bogus_message_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive left neighbor column on rank %d\n", my_rank);
                return 1;
            }

            //neighbor right
            MPI_Send(A + my_right_column_index, 1, left_right_type, nb_right, bogus_message_tag, cartesian_comm);
            MPI_Recv(A + right_nb_index, 1, left_right_type, nb_right, bogus_message_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive right neighbor column on rank %d\n", my_rank);
                return 1;
            }
        }
        else
        {
            //neighbor right
            // printf("%d: receiving from right neighbor %d\n", my_rank, nb_right);
            MPI_Recv(A + right_nb_index, 1, left_right_type, nb_right, bogus_message_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive right neighbor column on rank %d\n", my_rank);
                return 1;
            }
            MPI_Send(A + my_right_column_index, 1, left_right_type, nb_right, bogus_message_tag, cartesian_comm);

            //neighbor left
            MPI_Recv(A + left_nb_index, 1, left_right_type, nb_left, bogus_message_tag, cartesian_comm, &mpi_status);
            if (mpi_status.MPI_ERROR != MPI_SUCCESS)
            {
                printf("ERROR: could not receive left neighbor column on rank %d\n", my_rank);
                return 1;
            }
            MPI_Send(A + my_left_column_index, 1, left_right_type, nb_left, bogus_message_tag, cartesian_comm);
        }
#pragma endregion
#pragma endregion

        for (size_t i = my_row * chunk_size_y; i < (my_row + 1) * chunk_size_y; ++i)
        {
            size_t y_above = i != 0 ? i - 1 : 0;
            size_t y_below = i != N - 1 ? i + 1 : N - 1;

            for (size_t j = my_col * chunk_size_x; j < (my_col + 1) * chunk_size_x; ++j)
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

    double after = MPI_Wtime();
    double time = after - before;

    // printf("Final:\n");
    // printTemperature_2d(A, N);
    // printf("\n");
    free(B);

    if (!my_rank)
    {
        if (multi_thread)
        {
            for (int i = 1; i < 8; ++i)
            {
                int current_row = i / 2;
                int current_col = i % 2;
                int current_start = current_row * chunk_size_y * N + current_col * chunk_size_x;

                MPI_Recv(A + current_start, 1, full_chunk_type, i, 0, cartesian_comm, MPI_STATUS_IGNORE);
            }
        }

        int success = is_valid(A, N, source_x, source_y);
        printf("%d: Verification: %s\n", my_rank, (success) ? "OK" : "FAILED");
        printf("\n");
        // printTemperature_2d(A, N);
        printf("\nWall time: %lf\n", time);
        // for (int i = 0; i < N; ++i)
        //     printf("%f\n", A[i]);
    }
    else
    {
        MPI_Send(A + my_top_row_index, 1, full_chunk_type, 0, 0, cartesian_comm);
    }

    free(A);

    MPI_Type_free(&top_bottom_type);
    MPI_Type_free(&left_right_type);
    MPI_Type_free(&full_chunk_type);
    MPI_Finalize();

    return 0;
}
