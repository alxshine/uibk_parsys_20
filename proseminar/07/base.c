#include "base.h"

#include <stdio.h>

const int RESOLUTION = 120;
const int heat_source = 273 + 60;

int is_valid(Vector m, int N, int source_x, int source_y)
{
    // I test if the diagonals are monotone falling functions
    // because they are not explicitly set in the computation,
    // I assume that valid results in the diagonals imply valid results in general

    value_t old_temp;

    // from source to top left
    old_temp = heat_source;
    for (int i = source_y, j = source_x; i >= 0 && j >= 0; --i, --j)
    {
        value_t new_temp = m[i * N + j];
        if (new_temp > old_temp || new_temp > heat_source || new_temp < 273)
            return 0;
        old_temp = new_temp;
    }

    // from source to top right
    old_temp = heat_source;
    for (int i = source_y, j = source_x; i >= 0 && j < N; --i, ++j)
    {
        value_t new_temp = m[i * N + j];
        if (new_temp > old_temp || new_temp > heat_source || new_temp < 273)
            return 0;
        old_temp = new_temp;
    }

    // from source to bottom left
    old_temp = heat_source;
    for (int i = source_y, j = source_x; i < N && j >= 0; ++i, --j)
    {
        value_t new_temp = m[i * N + j];
        if (new_temp > old_temp || new_temp > heat_source || new_temp < 273)
            return 0;
        old_temp = new_temp;
    }

    // from source to bottom right
    old_temp = heat_source;
    for (int i = source_y, j = source_x; i < N && j < N; ++i, ++j)
    {
        value_t new_temp = m[i * N + j];
        if (new_temp > old_temp || new_temp > heat_source || new_temp < 273)
            return 0;
        old_temp = new_temp;
    }

    return 1;
}

void printTemperature_2d(Vector m, int N)
{
    const char *colors = " .-:=+*^X#%@";
    const int numColors = 12;

    // boundaries for temperature (for simplicity hard-coded)
    const value_t max = heat_source;
    const value_t min = 273;

    // set the 'render' resolution
    int W = RESOLUTION < N ? RESOLUTION : N;

    // step size in each dimension
    int sW = N / W;

    // room
    for (int i = 0; i < W; i++)
    {
        // left wall
        printf("X");
        for (int j = 0; j < W; ++j)
        {
            // actual room

            // get max temperature in this tile
            value_t max_t = 0;
            for (int y = sW * i; y < sW * i + sW; y++)
            {
                for (int x = sW * j; x < sW * j + sW; x++)
                {
                    max_t = (max_t < m[y * W + x]) ? m[y * W + x] : max_t;
                }
            }
            value_t temp = max_t;

            // pick the 'color'
            int c = ((temp - min) / (max - min)) * numColors;
            c = (c >= numColors) ? numColors - 1 : ((c < 0) ? 0 : c);

            // print the average temperature
            printf("%c", colors[c]);
        }
        // right wall
        printf("X\n");
    }
}