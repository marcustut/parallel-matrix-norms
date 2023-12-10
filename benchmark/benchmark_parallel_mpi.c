#include <stdio.h>
#include <string.h>
#include <mpi.h>

#include "helper.h"
#include "benchmark_helper.h"

#define MIN_RAND 1
#define MAX_RAND 1000000
#define ITERATIONS 16
#define STEP 256

result run(int n)
{
    int p, rank;
    double start, end;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int local_n = n / p;

    // Allocating memory for partial result and full result B
    double *local_a = malloc(local_n * n * sizeof(double));
    double *local_b = malloc(local_n * n * sizeof(double));
    double *local_c = calloc(local_n * n, sizeof(double));
    double *B = malloc(n * n * sizeof(double));

    // Assign random values for local matrix A and B
    for (int i = 0; i < local_n * n; i++)
        local_a[i] = random_number(MIN_RAND, MAX_RAND), local_b[i] = random_number(MIN_RAND, MAX_RAND);

    MPI_Barrier(MPI_COMM_WORLD);
    // Start benchmark time in process 0
    if (rank == 0)
        start = MPI_Wtime();

    // Send the generated local_b to B matrix
    for (int i = 0; i < p; i++)
        MPI_Gather(local_b, local_n * n, MPI_DOUBLE, B, local_n * n, MPI_DOUBLE, i, MPI_COMM_WORLD);

    // Do the matrix multiplication for each block of local_n
    for (int i = 0; i < local_n; i++)
        for (int j = 0; j < n; j++)
            for (int k = 0; k < n; k++)
                local_c[i * n + j] += local_a[i * n + k] * B[k * n + j];

    MPI_Barrier(MPI_COMM_WORLD);
    // End benchmark time in process 0
    if (rank == 0)
        end = MPI_Wtime();

    // Only allocate the result matrix C once in process 0
    double *C;
    if (rank == 0)
        C = malloc(n * n * sizeof(double));

    // Sends the partial results back into the result matrix C
    MPI_Gather(local_c, local_n * n, MPI_DOUBLE, C, local_n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Free locally used memories
    free(local_a);
    free(local_b);
    free(local_c);

    // Free result matrix C only on process 0
    if (rank == 0)
        free(C);

    if (rank == 0)
        return (result){n, end - start};
    else
        return (result){n, 0.0};
}

int main(int argc, char **argv)
{
    // Use current time as seed for random generator
    srand(time(0));

    int p, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Get number of processors on the system
    if (rank == 0)
        printf("Running benchmark_parallel_mpi on %d processes.\n", p);

    result results[ITERATIONS];
    for (int i = 0; i < ITERATIONS; i++)
    {
        results[i] = run((i + 1) * STEP);

        if (rank == 0)
            printf("[PARALLEL_MPI] Matrix multiplication for matrix %d x %d took %fs\n", results[i].n, results[i].n, results[i].elapsed_s);
    }

    if (rank == 0)
    {
        char path[256];
        snprintf(path, sizeof(path), "./data/benchmark_parallel_mpi%d.dat", p);
        export_results(results, ITERATIONS, path);
    }

    MPI_Finalize();

    return 0;
}