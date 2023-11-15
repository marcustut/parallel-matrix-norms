#ifndef HELPER_H
#define HELPER_H

#include <stdlib.h>
#include <pthread.h>

#if __APPLE__
#include <sys/sysctl.h>
#elif __linux__
#include <sys/sysinfo.h>
#else
#error "Unknown compiler"
#endif

#ifndef MIN
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef MAX
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif

double random_number(int min, int max)
{
    return rand() % max + min;
}

void init_random_square_matrix(int n, double *mat, int min, int max)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            mat[i * n + j] = random_number(min, max);
}

void transpose_matrix(int n, double *mat, double *transposed)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            transposed[j * n + i] = mat[i * n + j];
}

double max_of_array(int n, double *arr)
{
    if (n <= 0)
        return 0.0;

    if (n == 1)
        return arr[0];

    double max = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];

    return max;
}

// Get number of processors
int get_nproc()
{
// Get number of processors
#if __APPLE__
    int mib[2], nproc;
    size_t len;

    mib[0] = CTL_HW;
    mib[1] = HW_NCPU;
    len = sizeof(nproc);
    sysctl(mib, 2, &nproc, &len, NULL, 0);
    return nproc;
#elif __linux__
    int nproc = get_nprocs();
    return nproc;
#else
#error "Unknown compiler"
#endif
}

void print_vector(int n, double *vec)
{
    printf("[ ");
    for (int i = 0; i < n; i++)
        if (i == n - 1)
            printf("%f; ", vec[i]);
        else
            printf("%f ", vec[i]);
    printf("]\n");
}

void print_matrix(int n, double *mat)
{
    printf("[ ");
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (j == n - 1)
                printf("%f; ", mat[i * n + j]);
            else
                printf("%f ", mat[i * n + j]);
    printf("]\n");
}

void compare_matrix(int n, double *A, double *B)
{
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (A[i * n + j] != B[i * n + j])
            {
                fprintf(stderr, "[FAILED] %f(%d) != %f(%d)\n", A[i * n + j], i * n + j, B[i * n + j], i * n + j);
                exit(1);
            }
}

#endif