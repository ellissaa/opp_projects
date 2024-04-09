#include "utility.h"
#include <cmath>
#include <iostream>

void FillArray(double* vect, int N) {
    for (int i = 0; i < N; i++) {
        vect[i] = N + 1;
    }
}

void FillA(double* A, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = (i == j) ? 2 : 1;
        }
    }
}

void Ax (double* x, double* A, double* res, int N) {
    for (int i = 0; i < N; i++) {
        double curr_coord = 0;

        for (int j = 0; j < N; j++) {
            curr_coord += A[i * N + j] * x[j];
        } 
        res[i] = curr_coord;
    }
}

void SubVect(double* vect1, double* vect2, double* res, int N) {
    for (int i = 0; i < N; i++) {
        res[i] = vect1[i] - vect2[i];
    }
}

void MultScalar(double* vect, double* res, double scalar, int N) {
    for (int i = 0; i < N; i++) {
        res[i] = vect[i] * scalar;
    }
}

double VectLength(double* vect, int N) {
    double length = 0;

    for (int i = 0; i < N; i++) {
        length += vect[i] * vect[i];
    }
    return std::sqrt(length);
}

void FillAPart(double* A, int N, int line_count, int displ) {
    for (int i = 0; i < line_count; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = (j == i + displ) ? 2 : 1;
        }
    }
}

void FillbPart(double* b, int N, int line_count) {
    for (int i = 0; i < line_count; i++) {
        b[i] = N + 1;
    }
}

void PrintVect(double* vect, int N) {
    for (int i = 0; i < N; i++) {
        std::cout << vect[i] << "\t";
    }
    std::cout << "\n";
}

void PrintA(double* A, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << A[i * N + j] << " ";
        }
        std::cout << "\n";
    }
}

void FindLineCounts(int* line_counts, int process_count, int N) {
    int main_count = N / process_count;
    int remainder = N - main_count * process_count;

    for (int i = 0; i < process_count; i++) {
        line_counts[i] = main_count;
        if (i < remainder) {
            line_counts[i]++;
        }
    }
}

void FindDispls(int* displs, int* send_counts, int process_count) {
    displs[0] = 0;
    for (int i = 1; i < process_count; i++) {
        displs[i] = displs[i - 1] + send_counts[i - 1]; 
    }
}

void N_mul(int length, int* in_buf, int* out_buf, int k) {
    for (int i = 0; i < length; i++) {
        out_buf[i] = in_buf[i] * k;
    }
}


void FillZero(double* mas, int N) {
    for (int i = 0; i < N; i++) {
        mas[i] = 0;
    }
}
