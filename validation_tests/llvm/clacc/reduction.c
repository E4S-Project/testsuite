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
    for( i = 0 ; i < nt ; i++ ) {
        tab2[i] = 0;
    }
    k = 0;

    double* ptab2 = &tab2[0];
    
#pragma acc parallel loop private( tid ) reduction( max: ptab2 ) reduction( +: k )
    for( i = 0 ; i < TABSIZE ; i++ ) {
        tab[i] *= 2;
        tid = omp_get_thread_num();
        tab2[tid] = fmax( tab2[tid], tab[i] );
        k += tab[i];
    }
    printf( "%lf %lf\n", k, tab2[0] );

    for( i = 0 ; i < nt ; i++ ) {
        tab2[i] = 0;
    }
    k = 0;
    /* #pragma acc parallel loop private( tid ) reduction( max: tab2 ) 
    for( i = 0 ; i < TABSIZE ; i++ ) {
        tab[i] *= 2;
        tid = omp_get_thread_num();
        tab2[tid] = fmax( tab2[tid], tab[i] );
        }*/

   /*#pragma acc parallel loop private( tid ) reduction( +: ptab2 )
    for( i = 0 ; i < TABSIZE ; i++ ) {
        tab[i] *= 2;
        tid = omp_get_thread_num();
        ptab2[tid] += tab[i];
        }*/

    printf( "%lf %lf\n", k, tab2[0] );

    free( tab2 );
    return EXIT_SUCCESS;
}
