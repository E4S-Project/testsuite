#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_TYPE double
#define DATA_PRINTF_MODIFIER "%.2lf   "
#define DUMP_TARGET stdout

#define M_DEF 128
#define N_DEF 128

void initRand( int, int, DATA_TYPE mat[][*] );
void initZero( int, int, DATA_TYPE mat[][*] );

static void print_array( int ni, int nj, DATA_TYPE mat[][*] );
static void kernel_crout( int M, int N, DATA_TYPE A[][*], DATA_TYPE L[][*], DATA_TYPE U[][*] );

int main( int argc, char** argv) {
  
  int M, N;

  M = M_DEF;
  N = N_DEF;

  DATA_TYPE A[M][N];
  DATA_TYPE L[M][N];
  DATA_TYPE U[N][N];
    
  /* Initialize array(s). */
  initRand( M, N, A );
  initZero( M, N, L );
  initZero( N, N, U );

  /* Run kernel. */
  kernel_crout( M, N, A, L, U );

  /* Prevent DCE */
  print_array( M, N, L );
  print_array( N, N, U );

  return EXIT_SUCCESS;
}


/* Array initialization. */

void initRand( int lines, int col, DATA_TYPE mat[lines][col] ){
    int i, j;
    srand( 1 );
    for( i = 0 ; i < lines ; i++ ) {
        for( j = 0 ; j < col ; j++ ) {
            mat[i][j] = (DATA_TYPE) rand() / (DATA_TYPE) ( RAND_MAX + 1 );
        }
    }
}

void initZero( int lines, int col, DATA_TYPE mat[lines][col] ){
    int i, j;
    for( i = 0 ; i < lines ; i++ ) {
        for( j = 0 ; j < col ; j++ ) {
            mat[i][j] = 0;
        }
    }
}

/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static void print_array( int M, int N, DATA_TYPE C[M][N] ){
  int i, j;

  for (i = 0; i < M; i++)
    for (j = 0; j < N; j++) {
        if ((i * N + j) % 20 == 0) fprintf (DUMP_TARGET, "\n");
        fprintf (DUMP_TARGET, DATA_PRINTF_MODIFIER, C[i][j]);
    }
}

/* LU factorization using Crout's algorithm (source: Wikipedia) */

static void kernel_crout( int M, int N, DATA_TYPE A[M][N], DATA_TYPE L[M][N], DATA_TYPE U[N][N] ){
  int i, j, k;
  DATA_TYPE sum = 0;
  
  for (j = 0; j < N; j++) {
    U[j][j] = 1;
  }

  /*#pragma clang loop(k2) pack array( L, U ) allocate( malloc )
    #pragma clang loop(k1) pack array( L ) allocate( malloc )*/
  
#pragma clang loop id( j )
  for (j = 0; j < N; j++) {

      //#pragma clang loop ( i1, k1 ) tile sizes( 16, 16 ) floor_ids( if1, kf1) tile_ids( it1, kt1 )
#pragma clang loop id( i1 )
      for (i = j; i < M; i++) {
          sum = 0;
#pragma clang loop id( k1 )
          for (k = 0; k < N; k++) {
              sum = sum + L[i][k] * U[k][j];
          }
          L[i][j] = A[i][j] - sum;
      }
      
#pragma clang loop id( i2 )
      for (i = j; i < N; i++) {
          sum = 0;
#pragma clang loop id( k2 )
          for(k = 0; k < j; k++) {
              sum = sum + L[j][k] * U[k][i];
          }
          if (L[j][j] == 0) {
              printf("det(L) close to 0!\n Can't divide by 0...\n");
              exit(EXIT_FAILURE);
          }
          U[j][i] = (A[j][i] - sum) / L[j][j];
      }
  }
}
