#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_M 32
#define DEFAULT_N 32

void initRand( int, int, double[][*] );
void initZero( int, int, double[][*] );
double getTime( void );

int main( int argc, char** argv) {

  int M, N, i, j;
  double time_start, time_end;

  if( argc < 3 ) {
    M = DEFAULT_M;
    N = DEFAULT_N;
  } else {
    M = atoi( argv[1] );
    N = atoi( argv[2] );
  }
  
  double A[M][N], B[M][N], W[M][N];
  
  initRand( M, N, A );
  
  time_start = getTime();

  /* dim 1 */
  for( j = 0 ; j < M ; j++ ) {
    for( i = 0 ; i < N / 2 ; i++ ){ 
      W[j][i] = ( A[j][2*i] + A[j][2*i+1] ) / 2.0;
    }
    for( i = 0 ; i < N / 2 ; i++ ){ 
      W[j][i + N/2] = ( A[j][2*i] - A[j][2*i+1] ) / 2.0;
    }
  }
  
  /* dim 2 */
  
  for( j = 0 ; j < M / 2 ; j++ ){ 
    for( i = 0 ; i < N ; i++ ){
      B[j][i] = ( W[2*j][i] + W[2*j+1][i] ) / 2.0;
      B[j+M/2][i] = ( W[2*j][i] - W[2*j+1][i] ) / 2.0;
    }
  }
  
  time_end = getTime();
  printf( "%d \t %d \t %.2lf\n", M, N, (time_end - time_start) );

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

