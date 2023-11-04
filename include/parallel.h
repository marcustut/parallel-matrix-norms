#ifndef PARALLEL_H
#define PARALLEL_H

#if __APPLE__
#include <sys/sysctl.h>
#elif __linux__
#include <sys/sysinfo.h>
#else
#error "Unknown compiler"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#include "helper.h"

// The arguments to absolute_sum
typedef struct absolute_sum_args
{
    int n;
    double *row;
    double *sums;
    pthread_mutex_t sums_lock;
} absolute_sum_args;
// The routine to be ran by each thread to compute the absolute sum for a
// given matrix row.
void *absolute_sum(void *arguments)
{
    absolute_sum_args *args = arguments;

    // Using a mutex here because race condition will occur if multiple
    // threads mutate `sums` at the same time.
    pthread_mutex_lock(&args->sums_lock);
    for (int i = 0; i < args->n; i++)
        args->sums[i] += fabs(args->row[i]);
    pthread_mutex_unlock(&args->sums_lock);

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
    pthread_mutex_t sums_lock = PTHREAD_MUTEX_INITIALIZER;
    absolute_sum_args *args;
    for (int i = 0; i < n; i++)
    {
        args = malloc(sizeof(absolute_sum_args));
        args->n = n;
        args->row = A + i * n; // Give a slice (each row)
        args->sums = sums;
        args->sums_lock = sums_lock;

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
    double *row;
    double *cols;
    double *C;
} matrix_mult_args;
// The rountine to be ran by each thread to perform partial matrix
// multiplication.
void *matrix_mult(void *arguments)
{
    matrix_mult_args *args = arguments;
    int i = args->i; // row index
    int n = args->n; // matrix size

    for (int j = 0; j < n; j++)
    {
        double sum = 0.0;

        for (int k = 0; k < n; k++)
        {
            // printf("%f(%d) * %f(%d)\n", args->row[k], k, args->cols[j * n + k], j * n + k);
            sum += args->row[k] * args->cols[j * n + k];
        }

        // printf("C[%d][%d]\n", i, j);
        args->C[i * n + j] = sum;
    }

    free(arguments);
    return NULL;
}

double norm_of_product_parallel(int n, double *A, double *B)
{
    // Allocate memory for the resulting matrix of A * B.
    double *C = malloc(n * n * sizeof(double));
    if (!C)
    {
        fprintf(stderr, "Out of memory, reduce dimension n\n");
        exit(1);
    }

    // Allocate memory for transposed B.
    double *Bt = malloc(n * n * sizeof(double));
    if (!Bt)
    {
        fprintf(stderr, "Out of memory, reduce dimension n\n");
        exit(1);
    }

    // Transpose B
    transpose_matrix(n, B, Bt);

    // // Get number of processors
    // #if __APPLE__
    //     int mib[2], nproc;
    //     size_t len;

    //     mib[0] = CTL_HW;
    //     mib[1] = HW_NCPU;
    //     len = sizeof(nproc);
    //     sysctl(mib, 2, &nproc, &len, NULL, 0);
    // #elif __linux__
    //     int nproc = get_nprocs();
    // #else
    // #error "Unknown compiler"
    // #endif
    // Always use the smaller number as number of threads because we want
    // num_threads to be divisible by n.
    //
    // For example,
    //   if n=7, nproc=8, then num_threads=7. Hence, n / num_threads (7 / 7)
    //   if n=1024, nproc=8, then num_threads=8. Hence, n / num_threads (1024 / 8)
    // int num_threads = MIN(n, nproc);
    int num_threads = n;

    printf("Running norm_of_product_parallel on %d threads.\n", num_threads);

    // Spawn threads up to num_threads
    pthread_t threads[num_threads];
    matrix_mult_args *args;
    for (int i = 0; i < num_threads; i++)
    {
        args = malloc(sizeof(matrix_mult_args));
        args->i = i;
        args->n = n;
        args->row = A + i * n; // Give a slice (each row)
        args->cols = Bt;       // Give a slice (each col)
        args->C = C;

        int errno = pthread_create(&threads[i], NULL, matrix_mult, (void *)args);
        if (errno != 0)
        {
            fprintf(stderr, "[norm_of_product_parallel] Failed to create thread: %d\n", errno);
            exit(1);
        }
        pthread_join(threads[i], NULL);
    }

    // Wait for all spawned threads to complete
    // for (int i = 0; i < num_threads; i++)
    // {
    //     int errno = pthread_join(threads[i], NULL);
    //     if (errno != 0)
    //     {
    //         fprintf(stderr, "[norm_of_product_parallel] Failed to join thread: %d\n", errno);
    //         exit(1);
    //     }
    // }

    double *D = malloc(n * n * sizeof(double));
    if (!D)
    {
        fprintf(stderr, "Out of memory, reduce dimension n\n");
        exit(1);
    }
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, n, n, n, BLAS_ALPHA, A, n, B, n, BLAS_BETA, D, n);
    compare_matrix(n, C, D);

    // Compute the one-norm for C
    double norm = one_norm_parallel(n, C);

    free(C);
    free(Bt);

    return norm;
}

#endif