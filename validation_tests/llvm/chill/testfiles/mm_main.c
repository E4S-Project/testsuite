#include <stdio.h>
#include <stdlib.h>

#define N 128

void mm( float**, float**, float**, int, int, int );

int main(){
  float** A;
  float** B;
  float** C;
  int i, j;

  srand( 0 );
  A = (float**) malloc( N*sizeof( float* ) );
  B = (float**) malloc( N*sizeof( float* ) );
  C = (float**) malloc( N*sizeof( float* ) );
  for( i = 0 ; i < N ; i++ ){
    A[i] = (float*) malloc( N*sizeof( float ) );
    B[i] = (float*) malloc( N*sizeof( float ) );
    C[i] = (float*) malloc( N*sizeof( float ) );
  }
  for( i = 0 ; i < N ; i++ ) {
    for( j = 0 ; j < N ; j++ ) {
      A[i][j] = (double)rand()/(double)RAND_MAX;
      B[i][j] = (double)rand()/(double)RAND_MAX;
    }
  }
  mm( A, B, C, N, N, N );
  for( i = 0 ; i < N ; i++ ) {
    for( j = 0 ; j < N ; j++ ) {
      printf( "%f  ", C[i][j] );
    }
  }
  free( A );
  free( B );
  free( C );
  return EXIT_SUCCESS;
}
