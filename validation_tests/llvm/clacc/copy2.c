#include <stdio.h>
#include <stdlib.h>

#define DEFAULTM 32
#define DEFAULTN 32

#define FUNCALL 1

#if FUNCALL
void toto( int, int, double[*], double[*], double[*] );
#endif

int main( int argc, char** argv ) {
  int M, N;

  M = DEFAULTM;
  N = DEFAULTN;
  if( argc >= 2 ) {
    M = atoi( argv[1] );
    if( argc >= 3 ) {
      N = atoi( argv[2] );
    }
  }

  double V[N];
  double R[M*N];
  double w[M];
  
  srand( 0 );
  for( int i = 0 ; i < N ; i++ ) {
    V[i] = 0;
  }
  for( int j = 0 ; j < M ; j++ ) {
    for( int i = 0 ; i < N ; i++ ) {
      R[ j*N + i ] = (double)rand() / (double)RAND_MAX;
    }    
    w[j] = (double)rand() / (double)RAND_MAX;
  }
  
  /*********************************************************************************
   * First step: we are doing the loop in the main function
   *********************************************************************************/

#pragma acc parallel loop copyin( w, R ) copyout( V )
  for( int j = 0 ; j < N ; j++ ){
    for( int i = 0 ; i < M ; i++ ) {
      V[j] += w[i]*R[ i*N + j ];
    }
  }
  
  printf( "In-function loop passed\n" );
  
  /*********************************************************************************
   * Second step: the same thing in another function
   *********************************************************************************/
  
#if FUNCALL
  toto( M, N, V, w, R );
  
  printf( "Loop in a function call passed\n" );
#endif
  
  return EXIT_SUCCESS;
}

#if FUNCALL
void toto( int M, int N, double V[N], double w[M], double R[M*N] ) {
#pragma acc parallel loop copyin( w, R ) copyout( V )
  for( int j = 0 ; j < N ; j++ ){
    for( int i = 0 ; i < M ; i++ ) {
      V[j] += w[i]*R[ i*N + j ];
    }
  }
}
#endif
