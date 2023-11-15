#ifndef SERIAL_H
#define SERIAL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
    double *C = calloc(n * n, sizeof(double));
    if (!C)
    {
        fprintf(stderr, "Out of memory, reduce dimension n\n");
        return -1;
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                C[i * n + j] += A[i * n + k] * B[k * n + j];

    double norm = one_norm_serial(n, C);

    free(C);

    return norm;
}

#endif