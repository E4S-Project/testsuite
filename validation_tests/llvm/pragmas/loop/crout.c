#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_M 32
#define DEFAULT_N 32

void initRand( int, int, double[][*] );
void initZero( int, int, double[][*] );
double getTime( void );
void printPerf( int, int, double );

int main( int argc, char** argv) {

  int M, N;
  double time_start, time_end;

  if( argc < 3 ) {
    M = DEFAULT_M;
    N = DEFAULT_N;
  } else {
    M = atoi( argv[1] );
    N = atoi( argv[2] );
  }
  
  double A[M][N], U[N][N], L[M][N];
  
  initRand( M, N, A );
  initZero( M, N, L );
  initZero( N, N, U );
  
  int i, j, k;
  double sum = 0;
  
  /* LU factorization using Crout's algorithm (source: Wikipedia) */

  /* Basic version */

  time_start = getTime();

  for (i = 0; i < N; i++) {
    U[i][i] = 1;
  }

  for (j = 0; j < N; j++) {
    for (i = j; i < M; i++) {
      sum = 0;
      for (k = 0; k < N; k++) {
	sum = sum + L[i][k] * U[k][j];
      }
      L[i][j] = A[i][j] - sum;
    }
    
    for (i = j; i < N; i++) {
      sum = 0;
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

  time_end = getTime();

  printf( "Base      " );
  printPerf( M, N, (time_end - time_start ) );




  return EXIT_SUCCESS;
}


void initRand( int lines, int col, double mat[ lines ][ col ] ){
    int i, j;
    srand( 1 );
    for( i = 0 ; i < lines ; i++ ) {
        for( j = 0 ; j < col ; j++ ) {
            mat[i][j] = (double) rand() / (double) ( RAND_MAX + 1 );
        }
    }
}

void initZero( int lines, int col, double mat[ lines ][ col ] ){
    int i, j;
    for( i = 0 ; i < lines ; i++ ) {
        for( j = 0 ; j < col ; j++ ) {
            mat[i][j] = 0;
        }
    }
}

double getTime(){
    struct timespec tv;
    clock_gettime( CLOCK_REALTIME, &tv );
    return ( tv.tv_nsec + tv.tv_sec*1e9 );
}

void printPerf( int M, int N, double time ) {
    double flops = 2.0 * (double) M * (double) N * (double) N / 3.0;
    time *= 1e-3;
    printf( "%d \t %d \t %.0lf usec \t %.3lf Mflops\n", M, N, time, flops / time );
}
