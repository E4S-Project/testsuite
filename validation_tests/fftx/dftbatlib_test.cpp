#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fftx_dftbat_cpu_public.h"
#include "fftx_idftbat_cpu_public.h"
#include "fftx_prdftbat_cpu_public.h"
#include "fftx_iprdftbat_cpu_public.h"

static int M, N, K;

static void buildInputBuffer(double *X)
{
	for (int m = 0; m < M; m++) {
		for (int n = 0; n < N; n++) {
			X[(n + m*N) * 2 + 0] = 1 - ((double) rand()) / (double) (RAND_MAX/2);
			X[(n + m*N) * 2 + 1] = 1 - ((double) rand()) / (double) (RAND_MAX/2);
		}
	}

	return;
}

static void exercise_lib_entries ( fftx::point_t<2> *wcube )
{
	fftx::point_t<2> curr;
	transformTuple_t *tupl;
	double *X, *Y;

	for ( int iloop = 0; ; iloop++ ) {
		//  loop thru all the sizes in the library...
		curr = wcube[iloop];
		if ( curr.x[0] == 0 && curr.x[1] == 0 ) break;

		printf ( "Size { %d, %d } is available\n", curr.x[0], curr.x[1] );
		tupl = fftx_dftbat_Tuple ( wcube[iloop] );
		if ( tupl == NULL ) {
			printf ( "Failed to get tuple for size { %d, %d }\n", curr.x[0], curr.x[1] );
		}
		else {
			M = curr.x[0], N = curr.x[1];
			printf ( "M (batches) = %d, N (size) = %d, malloc sizes = %d * sizeof(double)\n", M, N, M*N*2 );
		
			X = (double *) malloc ( M * N * 2 * sizeof(double) );
			Y = (double *) malloc ( M * N * 2 * sizeof(double) );

			//  Call the init function
			( * tupl->initfp )();
 
			// set up data in input buffer and run the transform
			buildInputBuffer(X);

			for ( int kk = 0; kk < 100; kk++ ) {
				( * tupl->runfp ) ( Y, X );
			}
			
			// Tear down / cleanup
			( * tupl->destroyfp ) ();

			free ( X );
			free ( Y );
		}
	}

	return;
}


int main() {

	fftx::point_t<2> *wcube, curr;
	int iloop = 0;
	double *X, *Y;
	transformTuple_t *tupl;

	wcube = fftx_dftbat_QuerySizes();
	if (wcube == NULL) {
		printf ( "Failed to get list of available sizes from dftbat library\n" );
		exit (-1);
	}

	printf ( "Run all sizes from the dftbat library\n" );
	exercise_lib_entries ( wcube );

	wcube = fftx_idftbat_QuerySizes();
	if (wcube == NULL) {
		printf ( "Failed to get list of available sizes from idftbat library\n" );
		exit (-1);
	}

	printf ( "Run all sizes from the idftbat library\n" );
	exercise_lib_entries ( wcube );

	wcube = fftx_prdftbat_QuerySizes();
	if (wcube == NULL) {
		printf ( "Failed to get list of available sizes from prdftbat library\n" );
		exit (-1);
	}

	printf ( "Run all sizes from the prdftbat library\n" );
	exercise_lib_entries ( wcube );

	wcube = fftx_iprdftbat_QuerySizes();
	if (wcube == NULL) {
		printf ( "Failed to get list of available sizes from iprdftbat library\n" );
		exit (-1);
	}

	printf ( "Run all sizes from the iprdftbat library\n" );
	exercise_lib_entries ( wcube );


	//  Find specific entries in the library based on the size parameters...
	fftx::point_t<2> szs[] = { {1, 32}, {1, 60}, {1, 256}, {4, 64},
							   {16, 256}, {16, 1024}, {16, 400}, {0, 0} };
	for ( iloop = 0; ; iloop++) {
		curr = szs[iloop];
		if ( curr.x[0] == 0 && curr.x[1] == 0 ) break;

		M = curr.x[0], N = curr.x[1];
		printf ( "Get function pointers for batch = %d, size = %d", M, N);
		tupl = fftx_dftbat_Tuple ( curr);
		if ( tupl != NULL ) {
			//  allocate memory
			printf ( "\nAllocate buffers of size %ld for input/output", M * N * 2 * sizeof(double) );
			X = (double *) malloc ( M * N * 2 * sizeof(double) );
			Y = (double *) malloc ( M * N * 2 * sizeof(double) );

			//  Call the init function
			( * tupl->initfp )();
 
			// set up data in input buffer and run the transform
			buildInputBuffer(X);

			for ( int kk = 0; kk < 100; kk++ ) {
				( * tupl->runfp ) ( Y, X );
			}
			printf ( " -- Run the transform\n" );
			
			// Tear down / cleanup
			( * tupl->destroyfp ) ();

			free ( X );
			free ( Y );
		}
		else {
			printf ( " -- No entry in library for this size\n" );
		}
	}
		
}
