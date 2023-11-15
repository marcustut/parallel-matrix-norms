#include <sys/time.h>
#include <string.h>

#include "serial.h"
#include "parallel.h"

#define N 1024
#define NUM_MIN 1
#define NUM_MAX 10
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

int main(void)
{
    // Use current time as seed for random generator
    srand(time(0));

    // Allocate memory for the matrices
    double *A = malloc(N * N * sizeof(double));
    double *B = malloc(N * N * sizeof(double));
    if (!A || !B)
    {
        fprintf(stderr, "Out of memory, reduce dimension n\n");
        return -1;
    }

    // Initialise the matrices with some random values
    init_random_square_matrix(N, A, NUM_MIN, NUM_MAX);
    init_random_square_matrix(N, B, NUM_MIN, NUM_MAX);

    // Run matrix multiplication and norm
    double norm_serial = norm_of_product_serial(N, A, B);
    double norm_parallel = norm_of_product_parallel(N, A, B);

    // Compare the two matrices
    if (norm_serial != norm_parallel)
        fprintf(stderr, "[FAILED] %f != %f\n", norm_serial, norm_parallel);
    else
        printf("[PASSED] %s\n", __FILENAME__);

    free(A);
    free(B);
}