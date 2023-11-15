#ifndef PARALLEL_H
#define PARALLEL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include "helper.h"

// absolute_sum's mutex.
static pthread_mutex_t absolute_sum_mutex = PTHREAD_MUTEX_INITIALIZER;

// The arguments to absolute_sum
typedef struct absolute_sum_args
{
    int n;
    double *row;
    double *sums;
} absolute_sum_args;
// The routine to be ran by each thread to compute the absolute sum for a
// given matrix row.
void *absolute_sum(void *arguments)
{
    absolute_sum_args *args = arguments;

    // Using a mutex here because race condition will occur if multiple
    // threads mutate `sums` at the same time.
    pthread_mutex_lock(&absolute_sum_mutex);
    for (int i = 0; i < args->n; i++)
        args->sums[i] += fabs(args->row[i]);
    pthread_mutex_unlock(&absolute_sum_mutex);

    free(arguments);
    return NULL;
}

double one_norm_parallel(int n, double *A)
{
    if (n <= 0)
        return 0.0;

    // Make an array for each column's absolute sum.
    double sums[n];
    for (int i = 0; i < n; i++)
        sums[i] = 0;

    // Spawn n threads (because there are n columns)
    pthread_t threads[n];
    absolute_sum_args *args;
    for (int i = 0; i < n; i++)
    {
        args = calloc(1, sizeof(absolute_sum_args));
        args->n = n;
        args->row = A + i * n; // Give a slice (each row)
        args->sums = sums;

        int errno = pthread_create(&threads[i], NULL, absolute_sum, (void *)args);
        if (errno != 0)
        {
            fprintf(stderr, "[one_norm_parallel] Failed to create thread: %d\n", errno);
            exit(1);
        }
    }

    // Wait for all the spawned threads to complete
    for (int i = 0; i < n; i++)
    {
        int errno = pthread_join(threads[i], NULL);
        if (errno != 0)
        {
            fprintf(stderr, "[one_norm_parallel] Failed to join thread: %d\n", errno);
            exit(1);
        }
    }

    // Since one-norm is the maximum column sum, so return the maximum
    return max_of_array(n, sums);
}

// The arguments for matrix_mult
typedef struct matrix_mult_args
{
    int i;
    int n;
    int num_threads;
    double *A;
    double *B;
    double *C;
} matrix_mult_args;
// The rountine to be ran by each thread to perform partial matrix
// multiplication.
void *matrix_mult(void *arguments)
{
    matrix_mult_args *args = arguments;
    int i = args->i;                            // thread index
    int n = args->n;                            // matrix size
    int partition_size = n / args->num_threads; // eg. 1024 / 4 = 256 (each thread handles a partition of rows)
    int col_start = i * partition_size;
    int col_end = (i + 1) * partition_size;

    for (int j = col_start; j < col_end; ++j)
        for (int i = 0; i < n; ++i)
        {
            double sum = 0.0;
            for (int k = 0; k < n; ++k)
                sum += args->A[i * n + k] * args->B[k * n + j];
            args->C[i * n + j] = sum;
        }

    free(arguments);
    return NULL;
}

double norm_of_product_parallel(int n, double *A, double *B)
{
    // Allocate memory for the resulting matrix of A * B.
    double *C = calloc(n * n, sizeof(double));
    if (!C)
    {
        fprintf(stderr, "Out of memory, reduce dimension n\n");
        exit(1);
    }

    int num_threads = get_nproc();

    if (n % num_threads != 0)
    {
        fprintf(stderr, "matrix size %d must be a multiple of num of threads %d\n", n, num_threads);
        exit(1);
    }

    // Spawn threads up to num_threads
    pthread_t threads[num_threads];
    matrix_mult_args *args;
    for (int i = 0; i < num_threads; i++)
    {
        args = calloc(1, sizeof(matrix_mult_args));
        args->i = i;
        args->n = n;
        args->num_threads = num_threads;
        args->A = A;
        args->B = B;
        args->C = C;

        int errno = pthread_create(&threads[i], NULL, matrix_mult, (void *)args);
        if (errno != 0)
        {
            fprintf(stderr, "[norm_of_product_parallel] Failed to create thread: %d\n", errno);
            exit(1);
        }
    }

    // Wait for all spawned threads to complete
    for (int i = 0; i < num_threads; i++)
    {
        int errno = pthread_join(threads[i], NULL);
        if (errno != 0)
        {
            fprintf(stderr, "[norm_of_product_parallel] Failed to join thread: %d\n", errno);
            exit(1);
        }
    }

    // Compute the one-norm for C
    double norm = one_norm_parallel(n, C);

    free(C);

    return norm;
}

#endif