#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <cblas.h>

#include "helper.h"

double one_norm_serial(int n, double *A)
{
    if (n <= 0)
        return 0.0;

    double sums[n];
    for (int i = 0; i < n; i++)
        sums[i] = 0;

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            sums[j] += fabs(A[i * n + j]);

    return max_of_array(n, sums);
}

double norm_of_product_serial(int n, double *A, double *B)
{
    double *C = malloc(n * n * sizeof(double));
    if (!C)
    {
        fprintf(stderr, "Out of memory, reduce dimension n\n");
        return -1;
    }

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, BLAS_ALPHA, A, n, B, n, BLAS_BETA, C, n);

    double norm = one_norm_serial(n, C);

    free(C);

    return norm;
}

#endif