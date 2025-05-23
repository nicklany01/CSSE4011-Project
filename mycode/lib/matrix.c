#include <math.h>

void mat_mul(double *A, double *B, double *C, int r1, int c1, int c2)
{
    for (int i = 0; i < r1; ++i)
    {
        for (int j = 0; j < c2; ++j)
        {
            C[i * c2 + j] = 0;
            for (int k = 0; k < c1; ++k)
            {
                C[i * c2 + j] += A[i * c1 + k] * B[k * c2 + j];
            }
        }
    }
}

void mat_add(double *A, double *B, double *C, int r, int c)
{
    for (int i = 0; i < r * c; ++i)
        C[i] = A[i] + B[i];
}

void mat_sub(double *A, double *B, double *C, int r, int c)
{
    for (int i = 0; i < r * c; ++i)
        C[i] = A[i] - B[i];
}

void mat_transpose(double *A, double *AT, int r, int c)
{
    for (int i = 0; i < r; ++i)
        for (int j = 0; j < c; ++j)
            AT[j * r + i] = A[i * c + j];
}

void mat_identity(double *A, int size)
{
    for (int i = 0; i < size * size; ++i)
        A[i] = 0;
    for (int i = 0; i < size; ++i)
        A[i * size + i] = 1;
}

void mat_copy(double *src, double *dest, int r, int c)
{
    for (int i = 0; i < r * c; ++i)
        dest[i] = src[i];
}

void mat_print(double *A, int r, int c)
{
    for (int i = 0; i < r; ++i)
    {
        for (int j = 0; j < c; ++j)
            printf("%7.3f ", A[i * c + j]);
        printf("\n");
    }
}