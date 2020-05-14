#include <stdlib.h>
#include <stdio.h>

#define N 128

void foo( int, double[*][*][*], double[*][*][*] );

int main(){
  int i, j, k;
  double in[N][N][N];
  double out[N][N][N];
  srand( 0 );  
  for( i = 0 ; i < N ; i++ )
    for( j = 0 ; j < N ; j++ )
      for( k = 0 ; k < N ; k++ )
	in[i][j][k] = (double)rand()/(double)RAND_MAX;
  foo( N, out, in );
  for( i = 0 ; i < N ; i++ )
    for( j = 0 ; j < N ; j++ )
      for( k = 0 ; k < N ; k++ )
	printf( "%lf   ", out[i][j][k] );
  return EXIT_SUCCESS;
}
