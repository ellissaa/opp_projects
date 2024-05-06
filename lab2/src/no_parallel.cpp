#include <string>
#include <cmath>
#include <iostream>
#include <chrono>
#include "Functions_for_lab2.h"

double* FindSolution(double* A, double* b, int N) {
    double* x = new double[N];
    double* new_x = new double[N];

    double lenght_new_x = 0;
    double lenght_b = VectLenght(b, N);
    double res = eps;

    for (int i = 0; i < N; i++) {
        x[i] = 0;
        new_x[i] = 0;
    }

    while (res >= eps) {
        for (int i = 0; i < N; i++) { // Ax = new_x
            for (int j = 0; j < N; j++) {
                new_x[i] += A[i * N + j] * x[j];
            }
            new_x[i] = new_x[i] - b[i];
            lenght_new_x += new_x[i] * new_x[i]; // || Ax - b ||^2
            new_x[i] = new_x[i] * t;
            new_x[i] = x[i] - new_x[i]; // x - (Ax - b)*thau
        }

        for (int i = 0; i < N; i++) {
            x[i] = new_x[i];
        }

        res = sqrt(lenght_new_x) / lenght_b;
        lenght_new_x = 0;
        for (int i = 0; i < N; i++) {
            new_x[i] = 0;   
        }
    }

    delete[] new_x;
    return x;
}

int main(int argc, char** argv) {
    int N = std::stoi(argv[1]);

    double* A = new double[N * N];
    double* b = new double[N];

    FillA(A, N);    
    FillArray(b, N);

    std::chrono::high_resolution_clock clock;

    auto start = clock.now();
    double* x = FindSolution(A, b, N);
    auto end = clock.now();

    auto time = std::chrono::duration_cast<std::chrono::milliseconds> (end - start);
    std::cout << time.count() << " ms\n";

    // PrintVect(x, N);

    delete[] A;
    delete[] b;
    delete[] x;
}
