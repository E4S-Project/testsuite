#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define N 32

int main( int argc, char** argv ) {

  int tab[N], tib[N];
  int i;

  for( i = 0 ; i < N ; i++ )
    tab[i] = i;
  for( i = 0 ; i < N ; i++ )
    tib[i] = 0;

#pragma acc parallel copy( tab )
  for( i = 0 ; i < N ; i++ )
    tab[i] *= 5;

#pragma acc parallel copyin( tab ) copyout( tib )
  for( i = 0 ; i < N ; i++ )
    tib[i] = -tab[i];

  /*
  for( i = 0 ; i < N ; i++ )
    printf( "%d  ", tab[i] );
  printf( "\n" );

  for( i = 0 ; i < N ; i++ )
    printf( "%d  ", tib[i] );
  printf( "\n" );
  */

  bool passed = true;
  for( i = 0 ; i < N ; i++ ){
    if( tab[i] != -tib[i] ) {
      passed = false;
      break;
    }
  }
  if( passed == false ) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
