#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

#define VALUE 166442
#define TAG 1

int main( int argc, char** argv ){

  int rank, size;
  int tosend, torecv;
  MPI_Status stat;
  int ret = EXIT_SUCCESS;

  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &rank );
  
  if( size < 2 ) {
    fprintf( stderr, "Error: not enought processes (size = %d)\n", size );
    ret = EXIT_FAILURE;
    goto theend;
  }
  
  tosend = VALUE;
  torecv = 0xffffffff;
  if( 0 == rank ){
    MPI_Send( &tosend, 1, MPI_INT, 1, TAG, MPI_COMM_WORLD );
  } else {
    if( 1 == rank ){
      MPI_Recv( &torecv, 1, MPI_INT, 0, TAG, MPI_COMM_WORLD, &stat );
      if( tosend == torecv ){
	ret = EXIT_SUCCESS;
      } else {
	fprintf( stderr, "Error: the received value is not the expected one\n" );
	ret = EXIT_FAILURE;
      }
    }
  }
    
  theend:
    MPI_Finalize();
    return ret;
}
