#include <ctime>
#include <iostream>
#include <cstdlib>

#include "functions.h"

int ValidParameters(int A_num_rows, int grid_num_rows, int B_num_cols, int grid_num_cols, int size) {
    if (A_num_rows % grid_num_rows != 0) {
        std::cerr << "Height of matrix A must be divisible by height of the grid.\n";
        return -1;
    }
    if (B_num_cols % grid_num_cols != 0) {
        std::cerr << "Width of matrix B must be divisible by width of the grid.\n";
        return -1;
    }
    if (grid_num_rows * grid_num_cols != size) {
        std::cerr << "Invalid grid size for this number of processes.\n";
        return -1;
    }
    return 1;
}

void FillRandom(double* arr, int size) {
    srand(clock());
    for (int i = 0; i < size; i++) {
        arr[i] = (double) (rand() % 100) / 10;
    }
}

void PrintMatrix(double* arr, int rows, int cols) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << arr[i * cols + j] << "\t";
        }
        std::cout << "\n";
    }
}
