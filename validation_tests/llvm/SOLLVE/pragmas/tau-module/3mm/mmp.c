/**
 * 3mm.c: This file is part of the PolyBench/C 3.2 test suite.
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
#define NM 128


/* Array initialization. */
static
void init_array(int ni, int nj, int nk, int nl, int nm,
		DATA_TYPE A[ni][nk],
		DATA_TYPE B[nk][nj],
		DATA_TYPE C[nj][nm],
		DATA_TYPE D[nm][nl] )
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nk; j++)
      A[i][j] = ((DATA_TYPE) i*j) / ni;
  for (i = 0; i < nk; i++)
    for (j = 0; j < nj; j++)
      B[i][j] = ((DATA_TYPE) i*(j+1)) / nj;
  for (i = 0; i < nj; i++)
    for (j = 0; j < nm; j++)
      C[i][j] = ((DATA_TYPE) i*(j+3)) / nl;
  for (i = 0; i < nm; i++)
    for (j = 0; j < nl; j++)
      D[i][j] = ((DATA_TYPE) i*(j+2)) / nk;
}


/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static
void print_array(int ni, int nl,
		 DATA_TYPE G[ni][nl] )
{
  int i, j;

  for (i = 0; i < ni; i++)
    for (j = 0; j < nl; j++) {
	fprintf (stderr, DATA_PRINTF_MODIFIER, G[i][j]);
	if ((i * ni + j) % 20 == 0) fprintf (stderr, "\n");
    }
  fprintf (stderr, "\n");
}


/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static
void kernel_3mm(int ni, int nj, int nk, int nl, int nm,
		DATA_TYPE E[ni][nj],
		DATA_TYPE A[ni][nk],
		DATA_TYPE B[nk][nj],
		DATA_TYPE F[nj][nl],
		DATA_TYPE C[nj][nm],
		DATA_TYPE D[nm][nl],
		DATA_TYPE G[ni][nl] )
{
  int i, j, k;

#pragma scop
  for (i = 0; i < ni; i++){
    for (j = 0; j < nj; j++){
        E[i][j] = 0;
    }
  }
    for (i = 0; i < nj; i++){
      for (j = 0; j < nl; j++){
          F[i][j] = 0;
      }
    }
  for (i = 0; i < ni; i++){
      for (j = 0; j < nl; j++) {
          G[i][j] = 0;
      }
  }



  //#pragma clang loop(it1) pack array(A)
  //#pragma clang loop(it1) pack array(B)
  
  
#pragma clang loop(it1,jt1,kt1) interchange permutation(#P00)
#pragma clang loop(it2,jt2,kt2) interchange permutation(#P01)
#pragma clang loop(it3,jt3,kt3) interchange permutation(#P02)
      
#pragma clang loop(i1,j1,k1) tile sizes(#P3,#P4,#P5) /*pit_ids(ip1,jp1,kp1)*/ floor_ids(if1,jf1,kf1)  tile_ids(it1,jt1,kt1)
#pragma clang loop(i2,j2,k2) tile sizes(#P6,#P7,#P8) /*pit_ids(ip2,jp2,kp2)*/ floor_ids(if2,jf2,kf2)  tile_ids(it2,jt2,kt2)
#pragma clang loop(i3,j3,k3) tile sizes(#P9,#P10,#P11) /*pit_ids(ip3,jp3,kp3)*/ floor_ids(if3,jf3,kf3) tile_ids(it3,jt3,kt3)


  /* E := A*B */
#pragma clang loop id(i1)
  for (i = 0; i < ni; i++){
#pragma clang loop id(j1)
   for (j = 0; j < nj; j++){
#pragma clang loop id(k1)
        for (k = 0; k < nk; ++k){
            E[i][j] += A[i][k] * B[k][j];
        }
    }
  }
  /* F := C*D */
#pragma clang loop id(i2)
  for (i = 0; i < nj; i++){
#pragma clang loop id(j2)
      for (j = 0; j < nl; j++){
#pragma clang loop id(k2)
          for (k = 0; k < nm; ++k){
              F[i][j] += C[i][k] * D[k][j];
          }
      }
  }
  /* G := E*F */
#pragma clang loop id(i3)
  for (i = 0; i < ni; i++){
#pragma clang loop id(j3)
      for (j = 0; j < nl; j++) {
#pragma clang loop id(k3)
          for (k = 0; k < nj; ++k){
              G[i][j] += E[i][k] * F[k][j];
          }
      }
  }
#pragma endscop

}


int main(int argc, char** argv)
{
  /* Retrieve problem size. */
  int ni = NI;
  int nj = NJ;
  int nk = NK;
  int nl = NL;
  int nm = NM;

  /* Variable declaration/allocation. */
  
  DATA_TYPE A[NI][NK];
  DATA_TYPE B[NK][NJ];
  DATA_TYPE C[NL][NJ];
  DATA_TYPE D[NI][NL];
  DATA_TYPE E[NI][NJ];
  DATA_TYPE F[NJ][NL];
  DATA_TYPE G[NI][NL];

  /* Initialize array(s). */
  init_array (ni, nj, nk, nl, nm, A, B, C, D );
  /* Start timer. */
  //  polybench_start_instruments;

  /* Run kernel. */
  kernel_3mm (ni, nj, nk, nl, nm,
              E, A, B, F, C, D, G );
	
  /* Stop and print timer. */
  /*  polybench_stop_instruments;
      polybench_print_instruments;*/

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  //  polybench_prevent_dce(print_array(ni, nl,  POLYBENCH_ARRAY(G)));

  /* Be clean. */
/*  POLYBENCH_FREE_ARRAY(E);
  POLYBENCH_FREE_ARRAY(A);
  POLYBENCH_FREE_ARRAY(B);
  POLYBENCH_FREE_ARRAY(F);
  POLYBENCH_FREE_ARRAY(C);
  POLYBENCH_FREE_ARRAY(D);
  POLYBENCH_FREE_ARRAY(G);*/

  return 0;
}
