/**
 * jacobi-1d-imper.c: This file is part of the PolyBench/C 3.2 test suite.
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
#define N      4096

/* Array initialization. */
static void init_array (int n, DATA_TYPE A[n], DATA_TYPE B[n] ){
  int i;

  for (i = 0; i < n; i++)
      {
	A[i] = ((DATA_TYPE) i+ 2) / n;
	B[i] = ((DATA_TYPE) i+ 3) / n;
      }
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static void print_array(int n, DATA_TYPE A[n] ){
  int i;

  for (i = 0; i < n; i++)
    {
      fprintf(stderr, DATA_PRINTF_MODIFIER, A[i]);
      if (i % 20 == 0) fprintf(stderr, "\n");
    }
  fprintf(stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static void kernel_jacobi_1d_imper(int tsteps,
			    int n,
			    DATA_TYPE A[n], DATA_TYPE B[n] ){
  int t, i, j;

#pragma scop

#define _DIRECT2_ 0
#define _DIRECT_ 0
#define _PEELING_ 0
#P0

#if _DIRECT_
#pragma clang loop id( t )
  for (t = 0; t < tsteps; t++) {
#pragma clang loop id( i )
    for (i = 1; i < n - 1; i++)
      B[i] = 0.33333 * (A[i-1] + A[i] + A[i + 1]);
#pragma clang loop id( j )
    for (j = 1; j < n - 1; j++)
      A[j] = B[j];
  }
#else // _DIRECT_
 
#if _DIRECT2_
#pragma clang loop id( t )
  for (t = 0; t < tsteps; t++) {
#pragma clang loop id( i )
    for (i = 1; i < n - 1; i++)
      B[i] = 0.33333 * (A[i-1] + A[i] + A[i + 1]);
#pragma clang loop id( j )
    for (j = 2; j < n ; j++)
      A[j-1] = B[j-1];
  }
#else // _DIRECT2_
#if _PEELING_

#pragma clang loop id( t )
  for (t = 0; t < tsteps; t++) {
    B[1] = 0.33333 * (A[0] + A[1] + A[2]);
#pragma clang loop id( i )
    for (i = 2; i < n - 1; i++){
      B[i] = 0.33333 * (A[i-1] + A[i] + A[i + 1]);
      A[i-1] = B[i-1];
    }
    A[n-2] = B[n-2];
  }
#else // _PEELING_

#pragma clang loop id( t )
  for (t = 0; t < tsteps; t++) {
#pragma clang loop id( i )
    for (i = 1; i < n - 1; i++){
      B[i] = 0.33333 * (A[i-1] + A[i] + A[i + 1]);
      if( i > 1 )
	A[i-1] = B[i-1];
      if( i == n - 2 )
	A[i] = B[i];
    }
  }

#endif // _PEELING_
#endif // _DIRECT2_
#endif // _DIRECT_
  
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int n = N;
  int tsteps = TSTEPS;

  /* Variable declaration/allocation. */
  DATA_TYPE A[n];
  DATA_TYPE B[n];

  /* Initialize array(s). */
  init_array (n, A, B );

  /* Start timer. */
  //  polybench_start_instruments;

  /* Run kernel. */
  kernel_jacobi_1d_imper( tsteps, n, A, B );

  /* Stop and print timer. */
  //  polybench_stop_instruments;
  //  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  //  polybench_prevent_dce(print_array(n, POLYBENCH_ARRAY(A)));
  print_array( n, A );

  /* Be clean. */
  /*  POLYBENCH_FREE_ARRAY(A);
      POLYBENCH_FREE_ARRAY(B);*/

  return 0;
}
