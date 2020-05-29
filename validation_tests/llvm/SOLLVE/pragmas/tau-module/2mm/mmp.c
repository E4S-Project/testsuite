/**
 * 2mm.c: This file is part of the PolyBench/C 3.2 test suite.
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
#define DATA_PRINTF_MODIFIER "%.2lf   "

#define NI 128
#define NJ 128
#define NK 128
#define NL 128

/* Array initialization. */
static
void init_array(int ni, int nj, int nk, int nl,
		DATA_TYPE *alpha,
		DATA_TYPE *beta,
		DATA_TYPE A[ni][nk],
		DATA_TYPE B[nk][nj],
		DATA_TYPE C[nl][nj],
		DATA_TYPE D[ni][nl] ) {
  int i, j;

  *alpha = 32412;
  *beta = 2123;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = ((DATA_TYPE) i*(j+1)) / nj;
  for (i = 0; i < nl; i++)
    for (j = 0; j < nj; j++)
      C[i][j] = ((DATA_TYPE) i*(j+3)) / nl;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++)
      D[i][j] = ((DATA_TYPE) i*(j+2)) / nk;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int ni, int nl,
		 DATA_TYPE D[ni][nl] ) {
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, D[i][j]);
	if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_2mm(int ni, int nj, int nk, int nl,
		DATA_TYPE alpha,
		DATA_TYPE beta,
		DATA_TYPE tmp[ni][nj],
		DATA_TYPE A[ni][nk],
		DATA_TYPE B[nk][nj],
		DATA_TYPE C[nl][nj],
		DATA_TYPE D[ni][nl] )
{
  int i, j, k;

#pragma scop
  /* D := alpha*A*B*C + beta*D */

  for (i = 0; i < ni; i++) {
    for (j = 0; j < nj; j++){
      tmp[i][j] = 0;
    }
  }
  /* TODO here */
  for (i = 0; i < ni; i++) {
    for (j = 0; j < nj; j++){
      A[i][j] *= alpha;
      D[i][j] *= beta;
    }
  }

  //#P0
  //#P1
  /*#P2*/
  //#pragma clang loop(il,jl,kl) tile sizes(#P6,#P7,#P8) floor_ids(il1,jl1,kl1) tile_ids(il2,jl2,kl2)
#pragma clang loop id(il)
  for (i = 0; i < ni; i++)
#pragma clang loop id(jl)
    for (j = 0; j < nj; j++)	//	tmp[i][j] = 0;
#pragma clang loop id(kl)
	for (k = 0; k < nk; ++k)
	  tmp[i][j] += A[i][k] * B[k][j];
    
#if 0  
  /*#P3
#P4
#P5*/
  //#pragma clang loop(im,jm,km) tile sizes(#P9,#P10,#P11) floor_ids(im1,jm1,km1) tile_ids(im2,jm2,km2)
#pragma clang loop id(im)
  for (i = 0; i < ni; i++)
#pragma clang loop id(jm)
    for (j = 0; j < nl; j++)	//	D[i][j] *= beta;
#pragma clang loop id(km)
	for (k = 0; k < nj; ++k)
	  D[i][j] += tmp[i][k] * C[k][j];
#endif 
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;
  int nj = NJ;
  int nk = NK;
  int nl = NL;

  /* Variable declaration/allocation. */
  DATA_TYPE alpha;
  DATA_TYPE beta;

  DATA_TYPE tmp[NI][NJ];
  DATA_TYPE A[NI][NK];
  DATA_TYPE B[NK][NJ];
  DATA_TYPE C[NL][NJ];
  DATA_TYPE D[NI][NL];

  /* Initialize array(s). */
  init_array (ni, nj, nk, nl, &alpha, &beta,
	      A, B, C, D );

  /* Start timer. */
  //  polybench_start_instruments;

  /* Run kernel. */
  kernel_2mm (ni, nj, nk, nl,
	      alpha, beta,
	      tmp, A, B, C, D );

  /* Stop and print timer. */
  //  polybench_stop_instruments;
  // polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  //  polybench_prevent_dce(print_array(ni, nl,  POLYBENCH_ARRAY(D)));

  /* Be clean. */
  /* POLYBENCH_FREE_ARRAY(tmp);
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);
  POLYBENCH_FREE_ARRAY(C);
  POLYBENCH_FREE_ARRAY(D);*/

  return 0;
}
