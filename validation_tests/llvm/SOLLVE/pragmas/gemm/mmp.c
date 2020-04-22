/* gemm.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define DATA_TYPE double
#define DATA_PRINTF_MODIFIER "%.2lf   "

#define NI 128
#define NJ 128
#define NK 128


/* Array initialization. */
static
void init_array(int ni, int nj, int nk,
		DATA_TYPE *alpha,
		DATA_TYPE *beta,
		DATA_TYPE C[ni][nj],
		DATA_TYPE A[ni][nk],
		DATA_TYPE B[nk][nj] )
{
  int i, j;

  *alpha = 1.5;
  *beta = 1.2;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++)
      C[i][j] = (DATA_TYPE) (i*j % ni) / ni;
  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = (DATA_TYPE) (i*(j+1) % nk) / nk;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = (DATA_TYPE) (i*(j+2) % nj) / nj;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int ni, int nj,
		 DATA_TYPE D[ni][nj] ) {
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nj; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, D[i][j]);
	if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_gemm(int ni, int nj, int nk,
		 DATA_TYPE alpha,
		 DATA_TYPE beta,
		 DATA_TYPE C[ni][nj],
		 DATA_TYPE A[ni][nk],
		 DATA_TYPE B[nk][nj]) {
  int i, j, k;

// => Form C := alpha*A*B + beta*C,
  for (i = 0; i < ni; i++) {
    for (j = 0; j < nj; j++){
       	C[i][j] *= beta;
    }
  }

#P0
#P1
#P2
#pragma clang loop(i,j,k) tile sizes(#P3,#P4,#P5) floor_ids(i1,j1,k1) tile_ids(i2,j2,k2)
#pragma clang loop id(i)
  for (i = 0; i < ni; i++) {
    #pragma clang loop id(j)
    for (j = 0; j < nk; j++){
    #pragma clang loop id(k)
     for (k = 0; k < nj; k++) {
       C[i][k] += alpha * A[i][j] * B[j][k];
    }
   }
 }
}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;
  int nj = NJ;
  int nk = NK;

  /*  printf ("NI: %d\n", ni);
  printf ("NJ: %d\n", nj);
  printf ("NK: %d\n", nk);*/

  /* Variable declaration/allocation. */
  DATA_TYPE alpha;
  DATA_TYPE beta;
  DATA_TYPE A[NI][NK];
  DATA_TYPE B[NK][NJ];
  DATA_TYPE C[NI][NJ];

  /* Initialize array(s). */
  init_array( ni, nj, nk, &alpha, &beta,
	      C, A, B );

  /* Run kernel. */
  kernel_gemm (ni, nj, nk,
	       alpha, beta,
	       C, A, B );

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  //  polybench_prevent_dce(print_array(ni, nj,  POLYBENCH_ARRAY(C)));

  return 0;
}
