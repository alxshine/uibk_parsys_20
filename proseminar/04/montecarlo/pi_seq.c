#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const int shift_offset = RAND_MAX / 2;

int main(int argc, char **argv)
{
    int seed = 1337;

    long total_steps = 1e8;
    if (argc > 1)
    {
        total_steps = atoi(argv[1]);
    }

    long my_steps = total_steps;
    int inside = 0;

    for (long i = 0; i < my_steps; ++i)
    {
        int x = rand_r(&seed) - shift_offset;
        float xf = x * 2.f / RAND_MAX;
        int y = rand_r(&seed) - shift_offset;
        float yf = y * 2.f / RAND_MAX;

        float distance = sqrtf(xf * xf + yf * yf);
        inside += distance < 1;
    }
    double pi = 4. * (double)inside / my_steps;
    printf("Approximation for pi after %ld steps: %f\n", total_steps, pi);
}