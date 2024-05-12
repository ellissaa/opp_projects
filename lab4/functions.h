#ifndef FUNCTIONS_FOR_LAB4
#define FUNCTIONS_FOR_LAB4

#include "const.h"

void initialize(double* block, int block_count, int rank, int num_processes);

void print(double* block, int block_count);

double calc_next_val(double const* block, int x, int y, int z);

#endif