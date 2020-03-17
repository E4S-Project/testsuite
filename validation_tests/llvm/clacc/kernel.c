#include <stdio.h>
#include <stdlib.h>

int main( ) {
#pragma acc kernels
    printf( "toto\n" );
  return EXIT_SUCCESS;
}
