#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define t 0.0001
#define eps 1.0e-6

void FillArray(double* vect, int N);

void FillA(double* A, int N);
 
void Ax(double* x, double* A, double* res, int N);

void SubVect(double* vect1, double* vect2, double* res, int N);

void MultScalar(double* vect, double* res, double scalar, int N);

double VectLenght(double* vect, int N);

void PrintVect(double* vect, int N);

void PrintA(double* A, int N);

void find_line_counts(int* line_counts, int process_amount, int N); // распределение строчек по процессам

#endif