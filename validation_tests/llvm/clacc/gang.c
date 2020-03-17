#include <stdio.h>
#include <stdlib.h>

#define DEFAULT 4

int main( int argc, char** argv ) {
  int nt = DEFAULT;
  if( argc > 1 ) {
    nt = atoi( argv[ 1 ] );
  }
#pragma acc parallel num_gangs( nt )
    printf( "toto\n" );
  return EXIT_SUCCESS;
}
