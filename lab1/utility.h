#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define t 0.0001
#define eps 1.0e-6

void FillArray(double* vect, int N);

void FillAPart(double* A_part, int N, int line_count, int line_displ);

void FillA(double* A, int N);

void FillbPart(double* b, int N, int line_count);
 
void Ax(double* x, double* A, double* res, int N);

void SubVect(double* vect1, double* vect2, double* res, int N);

void MultScalar(double* vect, double* res, double scalar, int N);

double VectLength(double* vect, int N);

void PrintVect(double* vect, int N);

void PrintA(double* A, int N);

void FindLineCounts(int* line_counts, int process_amount, int N); // распределение строчек по процессам

void FindDispls(int* displs, int* send_counts, int process_amount);

void N_mul(int length, int* in_buf, int* out_buf, int k);

void FillZero(double* mas, int N);

#endif
