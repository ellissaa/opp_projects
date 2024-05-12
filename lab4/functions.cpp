#include <stdio.h>
#include <iostream>

#include "functions.h"

double calc_border(int x, int y, int z) {
    static const double hx = (double) Dx / (Nx - 1);
    static const double hy = (double) Dy / (Ny - 1);
    static const double hz = (double) Dz / (Nz - 1);

    return (x0 + x * hx) * (x0 + x * hx) + (y0 + y * hy) * (y0 + y * hy) + 
        (z0 + z * hz) * (z0 + z * hz);
}

void initialize(double* block, int block_count, int rank, int num_processes) {
    for (int z = 0; z < block_count; z++) {
        for (int y = 0; y < Ny; y++) {
            for (int x = 0; x < Nx; x++) {
                int coord = z * Nx * Ny + Nx * y + x; // индекс в массиве блоков процесса

                if ((rank == 0 && z == 0) || (rank == num_processes - 1 && z == block_count - 1) ||
                        y == 0 || y == Ny - 1 || x == 0 || x == Nx - 1) {
                    int abs_z = Nz / num_processes * rank + z;
                    block[coord] = calc_border(x, y, abs_z);
                } else {
                    block[coord] = 0;
                }    
            }
        }
    }
}

double calc_next_val(double const* block, int x, int y, int z) {
    static const double hx_sqr = ((double) Dx / (Nx - 1)) * ((double) Dx / (Nx - 1));
    static const double hy_sqr = ((double) Dy / (Ny - 1)) * ((double) Dy / (Ny - 1));
    static const double hz_sqr = ((double) Dz / (Nz - 1)) * ((double) Dz / (Nz - 1));
    static const double factor = 1 / (2 / hx_sqr + 2 / hy_sqr + 2 / hz_sqr + a);

    double prev_add = (block[(z - 1) * Nx * Ny + y * Nx + x] + block[(z + 1) * Nx * Ny + y * Nx + x]) / hz_sqr +
        (block[z * Nx * Ny + (y - 1) * Nx + x] + block[z * Nx * Ny + (y + 1) * Nx + x]) / hy_sqr +
        (block[z * Nx * Ny + y * Nx + (x - 1)] + block[z * Nx * Ny + y * Nx + (x + 1)]) / hx_sqr -
        (6 - a * block[z * Nx * Ny + y * Nx + x]); 

    return factor * prev_add;
}

void print(double* block, int block_count) {
    for (int z = 0; z < block_count; z++) {
        for (int y = 0; y < Ny; y++) {
            for (int x = 0; x < Nx; x++) {
                int coord = z * Nx * Ny + y * Nx + x;
                printf("%8.4f ", block[coord]);
            }
            std::cout << "\n";
        }
        std::cout << "============================================================================\n";
    }
}
