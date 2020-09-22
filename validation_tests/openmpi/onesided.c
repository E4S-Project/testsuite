#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define VALUE 166442

int main( int argc, char** argv ){

  int* token;
  MPI_Win win;
  int rank, size, ret, value;
  ret = EXIT_SUCCESS;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  
  if( size < 2 ) {
    fprintf( stderr, "Error: not enought processes (size = %d)\n", size );
    ret = EXIT_FAILURE;
    goto theend;
  }
  
  MPI_Win_allocate( sizeof( int ), sizeof( int ),
		    MPI_INFO_NULL, MPI_COMM_WORLD, &token, &win );

  if( 0 == rank ){
    value = VALUE;
    MPI_Put( &value, 1, MPI_INT, 1, 0, 1, MPI_INT, win );
  }
  MPI_Win_flush_all( win );
  MPI_Win_sync( win );
  
  if( 1 == rank ){
    if( *token == VALUE ){
      ret = EXIT_SUCCESS;
    } else {
      fprintf( stderr, "Error: the received value is not the expected one\n" );
      ret = EXIT_FAILURE;
    }
  }

  MPI_Win_free( &win );
 theend:
  MPI_Finalize();

  return ret;
}
