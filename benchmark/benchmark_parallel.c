#include <stdio.h>
#include <sys/time.h>

#include "parallel.h"
#include "helper.h"
#include "benchmark_helper.h"

#define MIN_RAND 1
#define MAX_RAND 1000000
#define ITERATIONS 14

result run(int n)
{
    // Allocate memory for the matrices
    double *A = malloc(n * n * sizeof(double));
    double *B = malloc(n * n * sizeof(double));
    if (!A || !B)
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

    norm_of_product_parallel(n, A, B);

    gettimeofday(&end, &tz);

    // Calculate the elapsed time in seconds
    double elapsed_s = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_usec - start.tv_usec) * 1.e-6;

    // Deallocate the memory
    free(A);
    free(B);

    return (result){n, elapsed_s};
}

int main(void)
{
    // Use current time as seed for random generator
    srand(time(0));

    // Get number of processors on the system
    int nproc = get_nproc();
    printf("Running benchmark_parallel on %d threads.\n", nproc);

    result results[ITERATIONS];
    for (int i = 0; i < ITERATIONS; i++)
    {
        results[i] = run((i + 1) * nproc * nproc * 2);
        printf("[PARALLEL] Matrix norm for matrix %d x %d took %fs\n", results[i].n, results[i].n, results[i].elapsed_s);
    }

    char path[256];
    snprintf(path, sizeof(path), "./data/benchmark_parallel.dat");

    export_results(results, ITERATIONS, path);

    return 0;
}