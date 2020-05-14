#include <stdio.h>
#include <stdlib.h>

#define N 128

void foo( double*, double*, int );

int main(){
  double in[N], out[N];
  int i;  
  srand( 0 );
  for( i = 0 ; i < N ; i++ ) 
    in[i] = (double)rand()/(double)RAND_MAX;
  foo( out, in, N );
  for( i = 0 ; i < N ; i++ ) 
    printf( "%lf   ", out[i] );
  return EXIT_SUCCESS;
}
