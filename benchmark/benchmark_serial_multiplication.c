#include <stdio.h>
#include <string.h>

#include "helper.h"
#include "benchmark_helper.h"

#define MIN_RAND 1
#define MAX_RAND 1000000
#define ITERATIONS 16
#define STEP 256

result run(int n)
{
    // Allocate memory for the matrices
    double *A = malloc(n * n * sizeof(double));
    double *B = malloc(n * n * sizeof(double));
    double *C = malloc(n * n * sizeof(double));
    if (!A || !B || !C)
    {
        fprintf(stderr, "Out of memory, reduce dimension n\n");
        exit(-1);
    }

    // Initialise the matrices with some random values
    init_random_square_matrix(n, A, MIN_RAND, MAX_RAND);
    init_random_square_matrix(n, B, MIN_RAND, MAX_RAND);

    struct timeval start, end;
    struct timezone tz;

    gettimeofday(&start, &tz);

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            double sum = 0;
            for (int k = 0; k < n; k++)
                sum += A[i * n + k] * B[k * n + j];
            C[i * n + j] = sum;
        }

    gettimeofday(&end, &tz);

    // Calculate the elapsed time in seconds
    double elapsed_s = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) * 1.e-6;

    // Deallocate the memory
    free(A);
    free(B);

    return (result){n, elapsed_s};
}

int main(int argc, char **argv)
{
    // Use current time as seed for random generator
    srand(time(0));

    // Get number of processors on the system
    printf("Running benchmark_serial_multiplication.\n");

    result results[ITERATIONS];
    for (int i = 0; i < ITERATIONS; i++)
    {
        results[i] = run((i + 1) * STEP);
        printf("[SERIAL_MULTIPLICATION] Matrix multiplication for matrix %d x %d took %fs\n", results[i].n, results[i].n, results[i].elapsed_s);
    }

    char path[256];
    snprintf(path, sizeof(path), "./data/benchmark_serial_multiplication.dat");
    export_results(results, ITERATIONS, path);

    return 0;
}