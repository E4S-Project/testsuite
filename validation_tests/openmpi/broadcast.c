#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define VALUE 166442

int main( int argc, char** argv ){
  
  int rank, size;
  int value;
  int ret = EXIT_SUCCESS;
  
  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  
  if( size < 2 ) {
    fprintf( stderr, "Error: not enought processes (size = %d)\n", size );
    ret = EXIT_FAILURE;
    goto theend;
  }
  
  if( 0 == rank )
    value = VALUE;
  else
    value = 0xffffffff;
  MPI_Bcast( &value, 1, MPI_INT, 0, MPI_COMM_WORLD );

  if( 1 == rank ){
    if( value == VALUE ){
      ret = EXIT_SUCCESS;
    } else {
      fprintf( stderr, "Error: the received value is not the expected one\n" );
      ret = EXIT_FAILURE;
    }
  }
  
 theend:
  MPI_Finalize();
  return ret;
}
