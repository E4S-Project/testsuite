/**
 * adi.c: This file is part of the PolyBench/C 3.2 test suite.
 *
 *
 * Contact: Louis-Noel Pouchet <pouchet@cse.ohio-state.edu>
 * Web address: http://polybench.sourceforge.net
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define DATA_TYPE double
#define DATA_PRINTF_MODIFIER "%.2lf   "

#define N 128
#define TSTEPS 10

/* Array initialization. */
static
void init_array (int n,
		 DATA_TYPE X[n][n], 
		 DATA_TYPE A[n][n],
		 DATA_TYPE B[n][n] ) {

  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++)      {
	X[i][j] = ((DATA_TYPE) i*(j+1) + 1) / n;
	A[i][j] = ((DATA_TYPE) i*(j+2) + 2) / n;
	B[i][j] = ((DATA_TYPE) i*(j+3) + 3) / n;
      }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static void print_array(int n, DATA_TYPE X[n][n] ){
  int i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      fprintf(stderr, DATA_PRINTF_MODIFIER, X[i][j]);
      if ((i * N + j) % 20 == 0) fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_adi(int tsteps,
		int n,
		 DATA_TYPE X[n][n], 
		 DATA_TYPE A[n][n],
		 DATA_TYPE B[n][n] ) {
  int t, i1, i2;

#pragma scop

  /*#P0
#P1
#P2
#P3*/

  /*
#pragma clang loop(i10,i20) tile sizes(#L0,#L1) floor_ids(i101,i201) tile_ids(i102,i202)
#pragma clang loop(i12,i22) tile sizes(#L2,#L3) floor_ids(i121,i221) tile_ids(i122,i222)
#pragma clang loop(i13,i23) tile sizes(#L4,#L5) floor_ids(i131,i231) tile_ids(i132,i232)
#pragma clang loop(i15,i25) tile sizes(#L6,#L7) floor_ids(i151,i251) tile_ids(i152,i252)
  */

  for (t = 0; t < tsteps; t++) {
#pragma clang loop id(i10)
      for (i1 = 0; i1 < n; i1++)
#pragma clang loop id(i20)
	for (i2 = 1; i2 < n; i2++) {
	    X[i1][i2] = X[i1][i2] - X[i1][i2-1] * A[i1][i2] / B[i1][i2-1];
	    B[i1][i2] = B[i1][i2] - A[i1][i2] * A[i1][i2] / B[i1][i2-1];
	  }

#pragma clang loop id(i11)
      for (i1 = 0; i1 < n; i1++)
	X[i1][n-1] = X[i1][n-1] / B[i1][n-1];

#pragma clang loop id(i12)
      for (i1 = 0; i1 < n; i1++)
#pragma clang loop id(i22)
	for (i2 = 0; i2 < n-2; i2++)
	  X[i1][n-i2-2] = (X[i1][n-2-i2] - X[i1][n-2-i2-1] * A[i1][n-i2-3]) / B[i1][n-3-i2];
 
#pragma clang loop id(i13)
      for (i1 = 1; i1 < n; i1++)
#pragma clang loop id(i23)
	for (i2 = 0; i2 < n; i2++) {
	  X[i1][i2] = X[i1][i2] - X[i1-1][i2] * A[i1][i2] / B[i1-1][i2];
	  B[i1][i2] = B[i1][i2] - A[i1][i2] * A[i1][i2] / B[i1-1][i2];
	}

#pragma clang loop id(i24)
      for (i2 = 0; i2 < n; i2++)
	X[n-1][i2] = X[n-1][i2] / B[n-1][i2];

#pragma clang loop id(i15)
      for (i1 = 0; i1 < n-2; i1++)
#pragma clang loop id(i25)
	for (i2 = 0; i2 < n; i2++)
	  X[n-2-i1][i2] = (X[n-2-i1][i2] - X[n-i1-3][i2] * A[n-3-i1][i2]) / B[n-2-i1][i2];
    }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  DATA_TYPE X[n][n];
  DATA_TYPE A[n][n];
  DATA_TYPE B[n][n];

  /* Initialize array(s). */
  init_array (n, X, A, B );

  /* Run kernel. */
  kernel_adi (tsteps, n, X, A, B );

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  //  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(X)));
  //  print_array(n, X );

  return 0;
}
