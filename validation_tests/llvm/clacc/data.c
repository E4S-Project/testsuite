#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

long double fabsl( long double );

#define DEFAULTM   32
#define DEFAULTN   32
#define CHECKPRECI 1e-8

int main( int argc, char** argv ) {
  int M, N;
  bool eq, failsomewhere;
  int i, j;
  double a; 

  M = DEFAULTM;
  N = DEFAULTN;
  if( argc >= 2 ) {
    M = atoi( argv[1] );
    if( argc >= 3 ) {
      N = atoi( argv[2] );
    }
  }

  /*********************************************************************************
   * Initialize
   *********************************************************************************/

  double V[N];
  double R[M*N];
  double S[M*N];
  double w[M];
  double T[M];
  double U[M*N];
  
  srand( 0 );
  for( i = 0 ; i < N ; i++ ) {
    V[i] = 0;
  }
  for( j = 0 ; j < M ; j++ ) {
    for( i = 0 ; i < N ; i++ ) {
      R[ j*N + i ] = (double)rand() / (double)RAND_MAX;
      S[ j*N + i ] = 0;
    }    
    w[j] = (double)rand() / (double)RAND_MAX;
  }
  failsomewhere = false;
  
  /*********************************************************************************
   * Copy in and out
   *********************************************************************************/

#pragma acc parallel loop copyin( R[:M*N] ) copyout( S[:M*N] )
  for( j = 0 ; j < M ; j++ ) {
    for( i = 0 ; i < N ; i++ ) {
      S[ j*N + i ] = 2.0 / R[ j*N + i ];
    }
  }

  /* Verification */

  eq = true;
  for( j = 0 ; j < M ; j++ ) {
    for( i = 0 ; i < N ; i++ ) {
      if( fabsl( S[ j*N + i ] - ( 2.0 / R[ j*N + i ] ) ) > CHECKPRECI ) {
	eq = false;
	break;
      }
    }
  }
  printf( "copyin and copyout (loop): %s\n", eq?"[PASS]":"[FAILED]" );
  failsomewhere |= !eq;

  /* Data directive */

#pragma acc data copyin( R[:M*N] ) copyout( S[:M*N] )
#pragma acc parallel loop 
  for( j = 0 ; j < M ; j++ ) {
    for( i = 0 ; i < N ; i++ ) {
      S[ j*N + i ] = 2.0 / R[ j*N + i ];
    }
  }

  /* Verification */

  eq = true;
  for( j = 0 ; j < M ; j++ ) {
    for( i = 0 ; i < N ; i++ ) {
      if( fabsl( S[ j*N + i ] - ( 2.0 / R[ j*N + i ] ) ) > CHECKPRECI ) {
	eq = false;
	break;
      }
    }
  }
  printf( "copyin and copyout (data): %s\n", eq?"[PASS]":"[FAILED]" );
  failsomewhere |= !eq;

  /*********************************************************************************
   * Create: allocate some space, leave it there, retrieve it later
   *********************************************************************************/


  /* Data directive */

#pragma acc data create( U[:M*N] ) copyin( R[:M*N] ) copyout( S[:M*N] )
  {
#pragma acc parallel loop
    for( j = 0 ; j < M ; j++ ) {
      for( i = 0 ; i < N ; i++ ) {
	U[ j*N + i ] = 2.0 / R[ j*N + i ];
      }
    }
    
#pragma acc parallel loop
    for( j = 0 ; j < M ; j++ ) {
      for( i = 0 ; i < N ; i++ ) {
	S[ j*N + i ] = U[ j*N + i ] / 2.0;
      }
    }
  }
  
  /* Verification */
    
  eq = true;
  for( j = 0 ; j < M ; j++ ) {
    for( i = 0 ; i < N ; i++ ) {
      if( fabsl( S[ j*N + i ] - ( 1.0 / R[ j*N + i ] ) ) > CHECKPRECI ) {
	eq = false;
	break;
      }
    }
  }
  printf( "create (data):             %s\n", eq?"[PASS]":"[FAILED]" );
  failsomewhere |= !eq;

  /*********************************************************************************
   * Present: S is already on the device
   *********************************************************************************/

#pragma acc data copyin( S[:M*N] ) copyout( w[:M] )
  {
#pragma acc data create( T[:M] ) present( S[:M*N] )
    {
#pragma acc parallel loop 
      for( j = 0 ; j < M ; j++ ) {
	T[j] = 0;
	for( i = 0 ; i < N ; i++ ) {
	  T[j] += S[ j*N + i ]; // yes this is uggly and should be done with a reduction
	}
      }
      
#pragma acc parallel loop 
      for( j = 0 ; j < M ; j++ ) {
	w[j] = T[j] ;
      }
    }
  }

  /* Verification */

  eq = true;
  for( j = 0 ; j < M ; j++ ) {
    a = 0;
    for( i = 0 ; i < N ; i++ ) {
      a += S[ j*N + i ];
    }
    if( fabsl( a - w[j] ) > CHECKPRECI ) {
      	eq = false;
	break;
    }
  }
  printf( "present (data):            %s\n", eq?"[PASS]":"[FAILED]" );
  failsomewhere |= !eq;

  /*********************************************************************************
   * Nocreate
   *********************************************************************************/

#pragma acc data copyin( S[:M*N] ) copyout( w[:M] ) create( T[:M] )
  {
#pragma acc data no_create( T[:M] ) present( S[:M*N] )
    {
#pragma acc parallel loop 
      for( j = 0 ; j < M ; j++ ) {
	T[j] = 0;
	for( i = 0 ; i < N ; i++ ) {
	  T[j] += S[ j*N + i ]; 
	}
      }
    }
#pragma acc parallel loop 
    for( j = 0 ; j < M ; j++ ) {
      w[j] = T[j] ;
    }
  }

  /* Verification */

  eq = true;
  for( j = 0 ; j < M ; j++ ) {
    a = 0;
    for( i = 0 ; i < N ; i++ ) {
      a += S[ j*N + i ];
    }
    if( fabsl( a - w[j] ) > CHECKPRECI ) {
      	eq = false;
	break;
    }
  }
  printf( "no_create (data):          %s\n", eq?"[PASS]":"[FAILED]" );
  failsomewhere |= !eq;

  return !failsomewhere ? EXIT_SUCCESS : EXIT_FAILURE;
}
