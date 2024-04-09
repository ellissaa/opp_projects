#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <mpi.h>
#include "utility.h"

void FindSolution(double *A_part, double *b_part, double *x_part, int N, int max_line_count,
                  int *line_counts, int *line_displs, int proc_number, int process_count) {
    double* next_x_part = new double[max_line_count];
    FillZero(next_x_part, max_line_count);

    double length_x = 0, length_b = 0;
    double length_x_part = 0, length_b_part = 0;
    double res = eps;

    for (int i = 0; i < line_counts[proc_number]; i++) {
        length_b_part += b_part[i] * b_part[i];
    }
    MPI_Allreduce(&length_b_part, &length_b, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD); // сумма квадратов координат по b
    length_b = sqrt(length_b); // конечная длина b

    while (res >= eps) {
        int curr_proc_num = proc_number;
        for (int k = 0; k < process_count; k++) { // передаем части х по процессам, пока не посчитаем результат
            for (int i = 0; i < line_counts[proc_number]; i++) { // обходим строчки А_парт
                int shift = line_displs[curr_proc_num]; // сдвиг для соотв. проц.
                for (int j = 0; j < line_counts[curr_proc_num]; j++) {
                    next_x_part[i] += A_part[i * N + shift + j] * x_part[j]; 
                }
            }
            MPI_Sendrecv_replace(x_part, max_line_count, MPI_DOUBLE, (proc_number + 1) % process_count, 0,
                                 (process_count + proc_number - 1) % process_count, 0, MPI_COMM_WORLD,
                                 MPI_STATUS_IGNORE); // передаем следующему, берем у предыдущего
            curr_proc_num = (process_count + curr_proc_num - 1) % process_count; // считаем индекс предыдущего
        }
        
        for (int i = 0; i < line_counts[proc_number]; i++) {
            next_x_part[i] -= b_part[i];
            length_x_part += next_x_part[i] * next_x_part[i]; // || Ax - b ||^2
            next_x_part[i] = next_x_part[i] * t;
            x_part[i] = x_part[i] - next_x_part[i]; // x - (Ax - b)*thau 
        }  

        MPI_Allreduce(&length_x_part, &length_x, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

        res = sqrt(length_x) / length_b;
        length_x_part = 0;
        length_x = 0;
        FillZero(next_x_part, max_line_count);
    }
    delete[] next_x_part;
}

int main(int argc, char **argv) {
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

    int* line_counts = new int[process_count];
    int* line_displs = new int[process_count];

    FindLineCounts(line_counts, process_count, N); // кол-во строк по процессам
    FindDispls(line_displs, line_counts, process_count); // смещения для строк

    int line_count = line_counts[proc_number];
    int line_displ = line_displs[proc_number];
    int max_line_count = *std::max_element(line_counts, line_counts + process_count);

    double *A_part = new double[line_count * N];
    double *b_part = new double[line_count];
    double *x_part = new double[max_line_count];

    FillAPart(A_part, N, line_count, line_displ);
    FillbPart(b_part, N, line_count);
    FillZero(x_part, max_line_count);

    FindSolution(A_part, b_part, x_part, N, max_line_count, line_counts, line_displs, proc_number,
                 process_count); // вызываем функцию на каждом процессе (у каждого свой х_парт)

    double* solution = NULL;
    if (proc_number == 0) {
        solution = new double[N];
    }
    MPI_Gatherv(x_part, line_count, MPI_DOUBLE, solution, line_counts, line_displs,
                MPI_DOUBLE, 0, MPI_COMM_WORLD); // все части х записали в solution 
    
    delete[] solution;
    delete[] line_counts;
    delete[] line_displs;
    delete[] A_part;
    delete[] b_part;
    delete[] x_part;

    MPI_Finalize();
    auto end = clock.now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << time.count() << " ms\n";
}
