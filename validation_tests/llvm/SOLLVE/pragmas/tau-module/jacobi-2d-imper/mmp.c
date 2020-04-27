/**
 * jacobi-2d-imper.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 *
 * Modified by Camille Coti to use it with sollve and the autotuner.
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define DATA_TYPE double
#define DATA_PRINTF_MODIFIER "%lf"

#define TSTEPS 50
#define N      512

/* Array initialization. */
static void init_array (int n, DATA_TYPE A[n][n], DATA_TYPE B[n][n] ){
  int i, j;

  for (i = 0; i < n; i++)      {
    for (j = 0; j < n; j++)      {
      A[i][j] = ((DATA_TYPE) i+j + 2) / n;
      B[i][j] = ((DATA_TYPE) i*j+ 3) / n;
    }
  }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static void print_array(int n, DATA_TYPE A[n][n] ){
  int i, j;

  for (i = 0; i < n; i++){
    for (j = 0; j < n; j++) {
      fprintf(stderr, DATA_PRINTF_MODIFIER, A[i][j]);
      if (i % 20 == 0) fprintf(stderr, "\n");
    }
  }
  fprintf(stderr, "\n");
}

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_jacobi_2d_imper(int tsteps,
			    int n,
			    DATA_TYPE A[n][n], 
			    DATA_TYPE B[n][n] ){
  int t, i, j;

#pragma scop
  for (t = 0; t < tsteps; t++)   {
#P0
#P1
#pragma clang loop(i1,j1) tile sizes(#P4,#P5) floor_ids(ii1,ji1) tile_ids(ik2,jk2)
#pragma clang loop id(i1)
      for (i = 1; i < n - 1; i++)
#pragma clang loop id(j1)
	for (j = 1; j < n - 1; j++)
	  B[i][j] = 0.2 * (A[i][j] + A[i][j-1] + A[i][1+j] + A[1+i][j] + A[i-1][j]);
#P2
#P3
#pragma clang loop(i2,j2) tile sizes(#P6,#P7) floor_ids(ii2,ji2) tile_ids(ik2,jk2)
#pragma clang loop id(i2)
      for (i = 1; i < n - 1; i++)
#pragma clang loop id(j2)
	for (j = 1; j < n - 1; j++)
	  A[i][j] = B[i][j];
    }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */

  DATA_TYPE A[n][n];
  DATA_TYPE B[n][n];

  /* Initialize array(s). */
  init_array (n, A, B );

  /* Start timer. */
  //  polybench_start_instruments;

  /* Run kernel. */
  kernel_jacobi_2d_imper (tsteps, n, A, B );

  /* Stop and print timer. */
  //  polybench_stop_instruments;
  //  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  //  polybench_prevent_dce(print_array(n, A));

  /* Be clean. */
  //  POLYBENCH_FREE_ARRAY(A);
  //  POLYBENCH_FREE_ARRAY(B);

  return 0;
}
