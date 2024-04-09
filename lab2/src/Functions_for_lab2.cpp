#include "Functions_for_lab2.h"
#include <cmath>
#include <iostream>

void FillArray(double* vect, int N) {
    for (int i = 0; i < N; i++) {
        vect[i] = N + 1;
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

double VectLenght(double* vect, int N) {
    double lenght = 0;

    for (int i = 0; i < N; i++) {
        lenght += vect[i] * vect[i];
    }
    return std::sqrt(lenght);
}

void FillA(double* A, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = (i == j) ? 2 : 1;
        }
    }
}

void PrintVect(double* vect, int N) {
    for (int i = 0; i < N; i++) {
        std::cout << vect[i] << "\n";
    }
}

void PrintA(double* A, int N) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << A[i * N + j] << " ";
        }
        std::cout << "\n";
    }
}
