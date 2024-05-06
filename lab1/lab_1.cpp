#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <mpi.h>
#include "utility.h"

double* FindSolution(double* A_part, double* b, int* line_counts, int* line_displs, int N, int proc_number) {
    int line_count = line_counts[proc_number];
    int line_displ = line_displs[proc_number];

    double* x = new double[N]; // тут лежит ответ
    double* next_x_part = new double[line_count];
    
    FillZero(x, N);
    FillZero(next_x_part, line_count);

    double length_x = 0;
    double length_next_x_part = 0;
    double length_b = VectLength(b, N);
    double res = eps;

    while (res >= eps) {
        for (int i = 0; i < line_count; i++) { // Ax = new_x
            for (int j = 0; j < N; j++) {
                next_x_part[i] += A_part[i * N + j] * x[j];
            }
            next_x_part[i] = next_x_part[i] - b[line_displ + i];
            length_next_x_part += next_x_part[i] * next_x_part[i]; // || Ax - b ||^2
            next_x_part[i] = next_x_part[i] * t;
            next_x_part[i] = x[line_displ + i] - next_x_part[i]; // x - (Ax - b)*thau
        }

        MPI_Allgatherv(next_x_part, line_count, MPI_DOUBLE, x, line_counts, line_displs, 
            MPI_DOUBLE, MPI_COMM_WORLD); // станет одинаковый на всех процессах
        MPI_Allreduce(&length_next_x_part, &length_x, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        res = sqrt(length_x) / length_b;
        length_next_x_part = 0;
        length_x = 0;
        FillZero(next_x_part, line_count);
    }
    delete[] next_x_part;
    return x;
}

int main(int argc, char** argv) {
    std::chrono::high_resolution_clock clock;
    auto start = clock.now();

    if (argc != 2) {
        std::cout << "Matrix size expected.\n";
        return -1;
    }
    int N = std::stoi(argv[1]);

    MPI_Init(NULL, NULL); // инициализация интерфейса

    int process_count, proc_number;
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);   
    MPI_Comm_rank(MPI_COMM_WORLD, &proc_number);
    
    int* line_counts = new int[process_count]; // распределение строк по процессам
    int* line_displs = new int[process_count]; // сдвиги для процессов

    FindLineCounts(line_counts, process_count, N); 
    FindDispls(line_displs, line_counts, process_count); // смещения для строк
    
    int line_count = line_counts[proc_number];
    int line_displ = line_displs[proc_number];

    double* A_part = new double[line_count * N];
    FillAPart(A_part, N, line_count, line_displ);
    double* b = new double[N];
    FillArray(b, N);

    double* x = FindSolution(A_part, b, line_counts, line_displs, N, proc_number);

    // PrintVect(x, N);

    delete[] x;
    delete[] b;
    delete[] line_counts;
    delete[] line_displs;
    delete[] A_part;
    
    MPI_Finalize();
    auto end = clock.now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
    std::cout << time.count() << " ms\n";
}
