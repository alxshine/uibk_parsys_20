#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <omp.h>

typedef int index_t;

typedef struct Queen
{
    index_t x, y;
} Queen;

typedef struct QueenList
{
    struct QueenList *previous;
    const Queen *current;
} QueenList;

int doCollide(const Queen a, const Queen b)
{
    return a.x == b.x || a.y == b.y || abs(a.x - b.x) == abs(a.y - b.y);
}

int isThreatened(Queen *currentConfiguration, index_t numPlacedQueens, Queen candidateQueen)
{
    for (index_t i = 0; i < numPlacedQueens; ++i)
    {
        if (doCollide(currentConfiguration[i], candidateQueen))
            return 1;
    }

    return 0;
}

int getNumPossibleConfigurations(index_t boardSize, Queen *currentConfiguration, index_t numPlacedQueens)
{
    if (numPlacedQueens == boardSize)
        return 1;

    int ret = 0;
    for (index_t y = 0; y < boardSize; y++)
    {
#pragma omp task firstprivate(currentConfiguration) shared(ret)
      {
        Queen candidateQueen = {numPlacedQueens, y};
        if (!isThreatened(currentConfiguration, numPlacedQueens, candidateQueen))
        {
          Queen newConfiguration[numPlacedQueens + 1];
          memcpy(newConfiguration, currentConfiguration, numPlacedQueens * sizeof(Queen));
          newConfiguration[numPlacedQueens] = candidateQueen;
          ret += getNumPossibleConfigurations(boardSize, newConfiguration, numPlacedQueens + 1);
        }
      }
    }
#pragma omp taskwait
    return ret;
}

int main(int argc, char **argv)
{
    int N = 4;
    if (argc > 1)
        N = atoi(argv[1]);

    int numPossible;
    double time_before = omp_get_wtime();
#pragma omp parallel
    {
#pragma omp single
        {
            numPossible = getNumPossibleConfigurations(N, NULL, 0);
        }
    }
    double time_after = omp_get_wtime();
    printf("%d possible configurations for a %dx%d board, took %f seconds\n", numPossible, N, N, time_after - time_before);
}
