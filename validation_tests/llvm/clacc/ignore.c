#include <stdio.h>
#include <stdlib.h>

int main( int argc, char** argv ) {
    int l = 8;
    /* The vector_length argument must also be a constant */
#pragma acc parallel vector_length( l )
    printf( "toto\n" );
  return EXIT_SUCCESS;
}
