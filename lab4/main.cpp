#include <stdio.h>
#include <memory>
#include <chrono>
#include <iostream>
#include <string.h>
#include <mpi.h>

#include "functions.h"

static double update_layer(double const* current_block, int layer_z, double* updated_block) {
    double max_delta = 0;
    for (int y = 0; y < Ny; y++) {
        for (int x = 0; x < Nx; x++) {
            int coord = layer_z * Nx * Ny + y * Nx + x;

            if (y == 0 || y == Ny - 1 || x == 0 || x == Nx - 1) {
                updated_block[coord] = current_block[coord];
                continue;
            }

            updated_block[coord] = calc_next_val(current_block, x, y, layer_z);
            double delta = std::abs(current_block[coord] - updated_block[coord]);
            max_delta = delta > max_delta ? delta : max_delta;
        }
    }
    return max_delta;
}

static double update_core(double const* current_block, int block_count, int rank,
        int num_processes, double* updated_block) {
    if (rank == 0) {
        memcpy(updated_block, current_block, Nx * Ny * sizeof(double));
    }
    if (rank == num_processes - 1) {
        memcpy(updated_block + (block_count - 1) * Nx * Ny,
            current_block + (block_count - 1) * Nx * Ny, Nx * Ny * sizeof(double));
    }

    int start_layer = 2;
    int end_layer = block_count - 2;

    double max_delta = 0;
    for (int layer_z = start_layer; layer_z < end_layer; layer_z++) {
        double layer_delta = update_layer(current_block, layer_z, updated_block);
        max_delta = layer_delta > max_delta ? layer_delta : max_delta;
    }
    return max_delta;
}

int main(int argc, char** argv) {
    MPI_Init(NULL, NULL);
    int rank, num_processes;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &num_processes);

    int block_count;
    if (num_processes == 1) {
        block_count = Nz;
    } else {
        block_count = (rank == 0 || rank == num_processes - 1) ? Nz / num_processes + 1 : Nz / num_processes + 2;
    }

    auto curr_blocks = std::make_unique<double[]>(Nx * Ny * block_count);
    auto next_iter = std::make_unique<double[]>(Nx * Ny * block_count);
    initialize(curr_blocks.get(), block_count, rank, num_processes);    

    std::chrono::high_resolution_clock clock;
    auto start = clock.now();

    int iter = 0;
    double max_delta = EPS;

    while (max_delta >= EPS) {
        iter++;
        MPI_Request requests[4];

        if (rank != 0) {
            MPI_Isend(curr_blocks.get() + 1 * Nx * Ny, Nx * Ny, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[0]);
            MPI_Irecv(curr_blocks.get(), Nx * Ny, MPI_DOUBLE, rank - 1, 0, MPI_COMM_WORLD, &requests[1]);
        }

        if (rank != num_processes - 1) {
            MPI_Isend(curr_blocks.get() + (block_count - 2) * Nx * Ny, Nx * Ny, MPI_DOUBLE,
                rank + 1, 0, MPI_COMM_WORLD, &requests[2]);
            MPI_Irecv(curr_blocks.get() + (block_count - 1) * Nx * Ny, Nx * Ny, MPI_DOUBLE,
                rank + 1, 0, MPI_COMM_WORLD, &requests[3]);
        }

        // обновляется все, что не зависит от соседних процессов
        double process_delta = update_core(curr_blocks.get(), block_count, rank, num_processes,
            next_iter.get());

        if (rank != 0) {
            MPI_Wait(&requests[0], MPI_STATUS_IGNORE);
            MPI_Wait(&requests[1], MPI_STATUS_IGNORE);
        }
        if (rank != num_processes - 1) {
            MPI_Wait(&requests[2], MPI_STATUS_IGNORE);
            MPI_Wait(&requests[3], MPI_STATUS_IGNORE);
        }

        if (block_count >= 3) {
            double layer_delta = update_layer(curr_blocks.get(), block_count - 2, next_iter.get());
            process_delta = layer_delta > process_delta ? layer_delta : process_delta;
            layer_delta = update_layer(curr_blocks.get(), 1, next_iter.get());
            process_delta = layer_delta > process_delta ? layer_delta : process_delta;
        }

        curr_blocks.swap(next_iter);

        if (iter % 1000 == 0)
            MPI_Allreduce(&process_delta, &max_delta, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
    }

    auto end = clock.now();
    int64_t time = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();
    int64_t max_time;

    MPI_Allreduce(&time, &max_time, 1, MPI_LONG_LONG, MPI_MAX, MPI_COMM_WORLD);
    if (rank == 0) std::cout << max_time << " ms\n";
    if (rank == 0) std::cout << iter << "\n";

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}
