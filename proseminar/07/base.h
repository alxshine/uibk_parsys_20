#ifndef BASE_H
#define BASE_H

typedef double value_t;
typedef value_t *Vector;
const int RESOLUTION;
const int heat_source;

int is_valid(Vector m, int N, int source_x, int source_y);
void printTemperature_2d(Vector m, int N);

#endif