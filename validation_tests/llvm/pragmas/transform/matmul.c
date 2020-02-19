#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULTM  128
#define DEFAULTN  128
#define DEFAULTK  128

#define RANDOM (1 + rand() / (double) RAND_MAX)
long long unsigned gettime();

int main( int argc, char** argv ){

  double* A;
  double* B;
  double* C;
  int M, N, K, i, j, k;
  long long unsigned t_begin, t_end;

  if( argc < 2 ){
    M = DEFAULTM;
    N = DEFAULTN;
    K = DEFAULTK;
  } else {
    if( argc == 2 ) {
      M = N = K = atoi( argv[1] );
    } else {
      M = atoi( argv[1] );
      N = atoi( argv[2] );
      K = atoi( argv[3] );
    }
  }

  A = (double*) malloc( M*K*sizeof( double ) );
  B = (double*) malloc( K*N*sizeof( double ) );
  C = (double*) malloc( M*N*sizeof( double ) );
  
  srand( 0 );
  for( i = 0 ; i < M*K ; i++ ) A[i] = RANDOM;
  for( i = 0 ; i < N*K ; i++ ) B[i] = RANDOM;
  for( i = 0 ; i < M*N ; i++ ) C[i] = 0;

/*****************************************************************************/

  t_begin = gettime();
  for( i = 0 ; i < M ; i++ ){
    for( j = 0 ; j < K ; j++ ){
      for( k = 0 ; k < N ; k++ ){
	C[ i*N + k ] += A[ i*K + j ] * B[ j*N + k];
      }
    }
  }
  t_end = gettime();
  printf( "loop 1: %d \t %d \t %d \t %u \n", M, N, K, (unsigned int)( ( t_end - t_begin ) / 1e3 ) );

/*****************************************************************************/

  for( i = 0 ; i < M*N ; i++ ) C[i] = 0;

  t_begin = gettime();
#pragma clang transform interleave
  for( i = 0 ; i < M ; i++ ){
    for( j = 0 ; j < K ; j++ ){
      for( k = 0 ; k < N ; k++ ){
	C[ i*N + k ] += A[ i*K + j ] * B[ j*N + k];
      }
    }
  }
  t_end = gettime();
  printf( "loop 2: %d \t %d \t %d \t %u \n", M, N, K, (unsigned int)( ( t_end - t_begin ) / 1e3 ) );

/*****************************************************************************/

  for( i = 0 ; i < M*N ; i++ ) C[i] = 0;

  t_begin = gettime();
  for( i = 0 ; i < M ; i++ ){
#pragma clang transform unrollandjam
    for( j = 0 ; j < K ; j++ ){
      for( k = 0 ; k < N ; k++ ){
	C[ i*N + k ] += A[ i*K + j ] * B[ j*N + k];
      }
    }
  }
  t_end = gettime();
  printf( "loop 3: %d \t %d \t %d \t %u \n", M, N, K, (unsigned int)( ( t_end - t_begin ) / 1e3 ) );

/*****************************************************************************/

  for( i = 0 ; i < M*N ; i++ ) C[i] = 0;

  t_begin = gettime();
#pragma clang transform unroll
  for( i = 0 ; i < M ; i++ ){
    for( j = 0 ; j < K ; j++ ){
      for( k = 0 ; k < N ; k++ ){
	C[ i*N + k ] += A[ i*K + j ] * B[ j*N + k];
      }
    }
  }
  t_end = gettime();
  printf( "loop 4: %d \t %d \t %d \t %u \n", M, N, K, (unsigned int)( ( t_end - t_begin ) / 1e3 ) );

/*****************************************************************************/

  for( i = 0 ; i < M*N ; i++ ) C[i] = 0;

  t_begin = gettime();
#pragma clang transform vectorize
  for( i = 0 ; i < M ; i++ ){
    for( j = 0 ; j < K ; j++ ){
      for( k = 0 ; k < N ; k++ ){
	C[ i*N + k ] += A[ i*K + j ] * B[ j*N + k];
      }
    }
  }
  t_end = gettime();
  printf( "loop 5: %d \t %d \t %d \t %u \n", M, N, K, (unsigned int)( ( t_end - t_begin ) / 1e3 ) );

/*****************************************************************************/

  for( i = 0 ; i < M*N ; i++ ) C[i] = 0;

  t_begin = gettime();
#pragma clang transform distribute
  for( i = 0 ; i < M ; i++ ){
    for( j = 0 ; j < K ; j++ ){
      for( k = 0 ; k < N ; k++ ){
	C[ i*N + k ] += A[ i*K + j ] * B[ j*N + k];
      }
    }
  }
  t_end = gettime();
  printf( "loop 6: %d \t %d \t %d \t %u \n", M, N, K, (unsigned int)( ( t_end - t_begin ) / 1e3 ) );

/*****************************************************************************/

  free( A );
  free( B );
  free( C );

  return EXIT_SUCCESS;
}

long long unsigned gettime(){
  struct timespec tp;
  clock_gettime( CLOCK_REALTIME, &tp );
  return tp.tv_nsec + tp.tv_sec*1e9;
}
