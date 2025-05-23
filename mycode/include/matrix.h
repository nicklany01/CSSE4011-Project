#ifndef MATRIX_H
#define MATRIX_H

void mat_mul(double *A, double *B, double *C, int r1, int c1, int c2);
void mat_add(double *A, double *B, double *C, int r, int c);
void mat_sub(double *A, double *B, double *C, int r, int c);
void mat_transpose(double *A, double *AT, int r, int c);
void mat_identity(double *A, int size);
void mat_copy(double *src, double *dest, int r, int c);
void mat_print(double *A, int r, int c);

#endif // MATRIX_H