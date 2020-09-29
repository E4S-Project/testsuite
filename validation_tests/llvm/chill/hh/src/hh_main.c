#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>

#define DEFAULT_M 16
#define DEFAULT_N 16

#define DEBUG      0
#define SHOWRESULT 1
#define CHECKPRECI 1e-8

void initRand( int, int, double[][*] );
void initZero( int, int, double[][*] );
void initUnit( int, int, double[][*] );
void transpose( int, int, double[][*] );
double getnorm( int, double[] );
double getsign( double ) ;
void normalize( int, double, double[] );
void printMatrix( int, int, double[][*] );
void printMatrixOctave( int, int, double[][*] );
void matmul( int, int, int, double[][*], double[][*], double[][*] );
bool check( int, int, double[][*], double[][*], double[][*] );
double getTime( void );
void printPerf( int, int, double );

void householder( int, int, double[][*], double[][*], double [][*] );

int main( int argc, char** argv ){

    int M, N;
    double time_start, time_end;

    /* Init */
    
    if( argc < 3 ) {
        M = DEFAULT_M;
        N = DEFAULT_N;
    } else {
        M = atoi( argv[1] );
        N = atoi( argv[2] );
    }

    double A[M][N], R[N][N], Q[M][N];

    initRand( M, N, A );

    /* Here comes the fun */

    time_start = getTime();
    householder( M, N, A, Q, R );
    time_end   = getTime();

    printPerf( M, N, (time_end - time_start ) );

    /* Checks */

#if SHOWRESULT  
    printf( "R \n" );
    printMatrix( N, N, R );
    printf( "Q \n" );
    printMatrix( M, N, Q );
#endif

    /* Checks: A ?= QR and orthogonality of Q */

    if( true == check( M, N, A, Q, R ) ){
        printf( "[PASS]\n" );
    } else {
        printf( "[FAIL]\n" );
    }

#if DEBUG
    printf( "Initial matrix:\n" );
    printMatrix( M, N, A );

    printf( "Result check: QR = A\n" );

    //    memset( A, (char)0, M*N*sizeof( double ));
    for( int i = 0 ; i < M ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            A[i][j] = 0.0;
        }
    }
    matmul( M, N, N, A, Q, R );
    printMatrix( M, N, A );

    printf( "Unitarity check: Q*Q' = Q'*Q = I\n" );

    //    memset( R, (char)0, M*N*sizeof( double ));
    for( int i = 0 ; i < N ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            R[i][j] = 0.0;
        }
    }
    //   memcpy( A, Q, M*N*sizeof( double ));
    for( int i = 0 ; i < M ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            A[i][j] = Q[i][j];
        }
    }
    transpose( M, N, A );
    matmul( M, N, N, R, Q, A );
    printMatrix( M, N, R );
#endif

    return EXIT_SUCCESS;
}

void initRand( int lines, int col, double mat[ lines ][ col ] ){
    int i, j;
    srand( 0 );
    for( i = 0 ; i < lines ; i++ ) {
        for( j = 0 ; j < col ; j++ ) {
            mat[i][j] = (double) rand() / ( (double) RAND_MAX + 1.0 );
        }
    }
}

double getTime(){
  struct timespec tv;
  clock_gettime( CLOCK_MONOTONIC, &tv );
  return ( tv.tv_nsec + tv.tv_sec*1e9 );
}

void printPerf( int M, int N, double time ) {
    double flops = 2.0 * (double) M * (double) N * (double) N / 3.0;
    time *= 1e-3;
    printf( "%d \t %d \t %.0lf usec \t %.3lf Mflops\n", M, N, time, flops / time );
}

void printMatrix( int M, int N, double mat[M][N] ){
    int i, j;
    printf( "-------------------------------------\n" );
    for( i = 0 ; i < M ; i++ ){
        for( j = 0 ; j < N ; j++ ){
            printf( "%.2lf \t ", mat[ i ][ j ] );   
        }
        printf( "\n" );
    }
    printf( "-------------------------------------\n" );
}

void printMatrixOctave( int M, int N, double mat[M][N] ){
    int i, j;
    printf( "-------------------------------------\n" );
    printf( "[ " );
    for( i = 0 ; i < M ; i++ ){
        printf( "[ " );
        for( j = 0 ; j < N ; j++ ){
            printf( "%.2lf, ", mat[ i ][ j ] );   
        }
        printf( " ];" );
    }
    printf( "]\n" );
    printf( "-------------------------------------\n" );
}

bool isEqual( int M, int N, double A[M][N], double B[M][N] ) {
    int i, j;
    bool eq = true;

    for( i = 0 ; i < M ; i++ ) {
        for( j = 0 ; j < N ; j++ ) {
            if( fabs( B[i][j] - A[i][j] ) > CHECKPRECI ) {
                eq = false;
            }
        }
    }
    return eq;
}

bool checkCorrect( int M, int N, double A[M][N], double Q[M][N], double R[N][N] ) {

    bool eq = true;

    /* We want QR == A */

    double tmp[M][N];
    // memset( tmp, (char)0, M*N*sizeof( double  ) );
    for( int i = 0 ; i < N ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            tmp[i][j] = 0.0;
        }
    }
    matmul( M, N, N, tmp, Q, R );
    eq = isEqual( M, N, tmp, A );

    return eq;
}

bool checkUnitary( int M, int N, double Q[M][N] ) {
    double tmp[M][N];
    double res[M][M];
    double I[M][M];

    /* We want Q*Q' = Q'*Q = I */

    initUnit( M, M, I ); 

//    memset( res, (char)0, M*N*sizeof( double ) );
    for( int i = 0 ; i < N ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            res[i][j] = 0.0;
        }
    }
//    memcpy( tmp, Q, M*N*sizeof( double ));
    for( int i = 0 ; i < M ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            tmp[i][j] = Q[i][j];
        }
    }
    transpose( M, N, tmp );
    matmul( M, N, N, res, Q, tmp );

    if( false == isEqual( M, M, I, res ) )
        return false;

    // memset( res, (char)0, M*N*sizeof( double ) );
    for( int i = 0 ; i < N ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            res[i][j] = 0.0;
        }
    }
    matmul( M, N, N, res, tmp, Q );

    if( false == isEqual( M, M, I, res ) )
        return false;
    
    return true;
}

bool check( int  M, int N, double A[M][N], double Q[M][N], double R[N][N] ) {
    if( false == checkCorrect( M, N, A, Q, R ) ) {
        printf( "Incorrect result: A != QR (recision requested: %e)\n", CHECKPRECI );
        return false;
    }
    if( false == checkUnitary( M, N, Q ) ) {
        printf( "Incorrect result: Q is not unitary (recision requested: %e)\n", CHECKPRECI );
        return false;
    }

    return true;
}

