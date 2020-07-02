/**
 * seidel-2d.c: This file is part of the PolyBench/C 3.2 test suite.
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
static
void init_array (int n,
		 DATA_TYPE A[n][n] ){
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)
      A[i][j] = ((DATA_TYPE) i*(j+2) + 2) / n;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int n,
		 DATA_TYPE A[n][n] ){

  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      fprintf(stderr, DATA_PRINTF_MODIFIER, A[i][j]);
      if ((i * n + j) % 20 == 0) fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_seidel_2d(int tsteps,
		      int n,
		      DATA_TYPE A[N][N] ) {
  int t, i, j;

#pragma scop

#pragma clang loop(i,j) tile sizes(64,64) floor_ids(ii1,ji1) tile_ids(ii2,ji2)

#pragma clang loop id( t )
  for (t = 0; t <= tsteps - 1; t++)
#pragma clang loop id( i )
    for (i = 1; i<= N - 2; i++)
#pragma clang loop id( j )
      for (j = 1; j <= N - 2; j++)
	A[i][j] = (A[i-1][j-1] + A[i-1][j] + A[i-1][j+1]
		   + A[i][j-1] + A[i][j] + A[i][j+1]
		   + A[i+1][j-1] + A[i+1][j] + A[i+1][j+1])/9.0;
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */

  DATA_TYPE A[n][n];


  /* Initialize array(s). */
  init_array( n, A );

  /* Run kernel. */
  kernel_seidel_2d( tsteps, n, A );

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  //  polybench_prevent_dce(print_array(n, A ) );

  return 0;
}
