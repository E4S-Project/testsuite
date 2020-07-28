#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define TABSIZE 32

int main(){

    double tab[TABSIZE];
    double* tab2;
    double k;
    int i, nt, tid;

    for( i = 0 ; i < TABSIZE ; i++ ) {
        tab[i] = 1/i;
    }
    nt = omp_get_num_threads();
    tab2 = (double*) malloc( nt * sizeof( double ));
    k = 0;

    double* ptab = &tab2[0];

#pragma acc parallel loop private( tid ) reduction( max: ptab ) reduction( +: k )
    for( i = 0 ; i < TABSIZE ; i++ ) {
        tab[i] *= 2;
        tid = omp_get_thread_num();
        tab2[tid] = fmax( tab2[tid], tab[i] );
        k += tab[i];
    }

#pragma acc parallel loop private( tid ) reduction( +: ptab )
    for( i = 0 ; i < TABSIZE ; i++ ) {
        tab[i] *= 2;
        tid = omp_get_thread_num();
        tab2[tid] += tab[i];
        }

    printf( "%lf %lf\n", k, tab2[0] );

    free( tab2 );
    return EXIT_SUCCESS;
}
