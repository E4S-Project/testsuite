#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 8

static long steps = 1000000000;
double step;

int main(int argc, const char* argv[]) {

    double pi = 0.0;
    double start, delta, sum[NUM_THREADS];
    start = omp_get_wtime();
    step = 1.0 / (double)steps;
    omp_set_num_threads(NUM_THREADS);
#pragma omp parallel
    {
        double x;
        int id, i;
        id = omp_get_thread_num();
        for (i = id, sum[id] = 0.0; i < steps; i = i + NUM_THREADS) {
            x = (i + 0.5) * step;
            sum[id] += 4.0 / (1.0 + x * x);
        }
    }
    for (int i = 0; i < NUM_THREADS; i++) {
        pi += sum[i] * step;
    }
    delta = omp_get_wtime() - start;
    printf("PI = %.16g computed in %.4g seconds with %d threads.\n", pi, delta, NUM_THREADS);

    return EXIT_SUCCESS;
}

