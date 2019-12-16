#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DEFAULT_M 16
#define DEFAULT_N 16

#define DEBUG      0
#define SHOWRESULT 0

void initRand( double*, int );
void initZero( double*, int );
void initUnit( double*, int, int );
void printMatrix( double*, int, int );
void printMatrixOctave( double*, int, int );
void householder( double*, double*, double*, int, int );

int main( int argc, char** argv ){

    int M, N;
    double* A;
    double* Q;
    double* R;

    /* Init */
    
    if( argc < 3 ) {
        M = DEFAULT_M;
        N = DEFAULT_N;
    } else {
        M = atoi( argv[1] );
        N = atoi( argv[2] );
    }

    A = (double*) malloc( M * N * sizeof( double ) );
    Q = (double*) malloc( M * N * sizeof( double ) );
    R = (double*) malloc( N * N * sizeof( double ) );

    initRand( A, M*N );

    /* Here comes the fun */

    householder( A, Q, R, M, N );

    /* Finalize */

    free( A );
    free( Q );
    free( R );
    return EXIT_SUCCESS;
}

void initRand( double* mat, int size ){
    int i;
    srand( 0 );
    for( i = 0 ; i < size ; i++ ) {
        mat[i] = (double) rand() / (double) ( RAND_MAX + 1 );
    }
}

void initZero( double* mat, int size ){
    memset( mat, (char)0, size*sizeof( double ) );
}

void initUnit( double* mat, int l, int c ){
    int i;
    initZero( mat, l*c );
    for( i = 0 ; i < l && i < c ; i++ ) {
        mat[ i*c + i ] = 1;
    }
}

inline double getnorm( double* vec, int len ) {
    int i;
    double n = vec[0]*vec[0];
    for( i = 1 ; i < len ; i++ ) {
      n += ( vec[i] * vec[i] );
    }
    return sqrtl( n );
}

void printMatrix( double* mat, int M, int N ){
    int i, j;
    printf( "-------------------------------------\n" );
    for( i = 0 ; i < M ; i++ ){
        for( j = 0 ; j < N ; j++ ){
            printf( "%.2lf \t ", mat[ i*N + j ] );   
        }
        printf( "\n" );
    }
    printf( "-------------------------------------\n" );
}

void printMatrixOctave( double* mat, int M, int N ){
    int i, j;
    printf( "-------------------------------------\n" );
    printf( "[ " );
    for( i = 0 ; i < M ; i++ ){
        printf( "[ " );
        for( j = 0 ; j < N ; j++ ){
            printf( "%.2lf, ", mat[ i*N + j ] );   
        }
        printf( " ];" );
    }
    printf( "]\n" );
    printf( "-------------------------------------\n" );
}

void transpose( double* A, int M, int N ) {
    int i, j;
    double tmp;
    for( i = 0 ; i < M ; i++ ) {
        for( j = i ; j < N ; j++ ) {
            tmp = A[ i * N + j ];
            A[ i * N + j ] = A[ j * N + i ];
            A[ j * N + i ] = tmp;
        }
    }
}

inline double getsign( double d ) {
    return ( d > 0 ) ? 1.0 : -1.0;       
}

inline void normalize( double* vec, double len, double div ) {
    int i;
    for( i = 0 ; i < len ; i++ ) {
        vec[i] /= div;
    }
}

inline void normalize2( double* in, double* out, double len, double div ) {
    int i;
    for( i = 0 ; i < len ; i++ ) {
        out[i] = in[i] / div;
    }
}

inline void matmul( double* out, double* A, double* B, int M, int K, int N ){
    int i, j, k;
    for( i = 0 ; i < M ; i++ ){ 
        for( j = 0 ; j < N ; j++ ){
	  //            out[ i * N + j ] = 0.0;
            for( k = 0 ; k < K ; k++ ){
                out[ i * N + j ] += A[ i * K + k ] * B[ k * N + j ];
            }
        }
    }
}

void applyR( double* R, double* w, double tau, int len, int start ){
    int i, j;
    double* tmpM;
    double* tmpV;

    tmpM = (double*) malloc( len * len * sizeof( double ) );
    tmpV = (double*) malloc( len * sizeof( double ) );

    /* R(j:end,:) = R(j:end,:)-(tau*w)*(w’*R(j:end,:)); */

     /* tmpV = w'*R(j:end,:) */
    
    memset( tmpV, (char) 0, len*sizeof( double ) );
    for( j = 0 ; j < len ; j++ ){
      for( i = start ; i < len ; i++ ) {
	tmpV[j] += w[i]*R[ i*len + j ];
      }
    }

    /* tmpM = tau * w * tmpV */

    for( i = 0 ; i < len ; i++ ){
      for( j = start ; j < len ; j++ ){
	  tmpM[ j * len + i ] = tau * w[ j ] * tmpV[ i ];
	}
    }

    /* R = R - tmpM */

    for( i = start ; i < len ; i++ ){
        for( j = 0 ; j < len ; j++ ){
            R[ i * len + j] -= tmpM[ i * len + j ];
        }
    }

    free( tmpM );
    free( tmpV );
}

void applyQ( double* Q, double* w, double tau, int len, int start ){
    int i, j;
    double* tmpM;
    double* tmpV;

    tmpM = (double*) malloc( len * len * sizeof( double ) );
    tmpV = (double*) malloc( len * sizeof( double ) );
    memset( tmpM, (char)0, len * len * sizeof( double ) );

    /* Q(:,j:end) = Q(:,j:end)-(Q(:,j:end)*w)*(tau*w)’; */

    /* tmpV = (Q(:,j:end)*w) */

    for( j = 0 ; j < len ; j++ ) {
      tmpV[j] = 0.0;
      for( i = start ; i < len ; i++ ) {
	tmpV[j] += Q[ j * len + i ] * w[i];
      }      
    }

    /* tmpM = tmpV * (tau*w)’; */

    for( j = 0 ; j < len ; j++ ) {
      for( i = start  ; i < len ; i++ ) {
	tmpM[ j*len + i ] = tmpV[j] * w[i] * tau;
      }      
    }

    /* Q(:,j:end) -= tmpM */

    for( j = 0 ; j < len ; j++ ) {
      for( i = start ; i < len ; i++ ) {
	Q[ j * len + i ] -= tmpM[ j* len + i ];
      }
    }

    free( tmpM );
    free( tmpV );
}

void householder( double* A, double* Q, double* R, int M, int N ){

  int i, j;
    double u1, tau, sign, norm;
    double* w;
    
    w = (double*) malloc( N* sizeof( double ) );
    memcpy( R, A, ( ( M > N ) ? N : M ) * N * sizeof( double ) );
    initUnit( Q, M, N );

    for( i = 0 ; i < M  ; i++ ) {
      
      memset( w, (char)0, N*sizeof( double ) );
      
      /* H = I - tau * w * w' */
      
      for( j = i ; j < N ; j++ ) {
	w[j] = R[ j * N + i ];
      }
      norm = getnorm( w+i, N - i );
      sign = -1 * getsign( R[ i + i*N ] );
      u1 = R[ i * N + i ] - sign * norm;
      
      normalize( w +i+1, N-i-1, u1 ); // w = R(j:end,j)/u1;
      w[ i ] = 1;
      tau = -1 * sign * u1 / norm;
      
        /* R = HR; Q = QH */

      if( i < N ) {
	applyR( R, w, tau, N, i );
      }
      applyQ( Q, w, tau, N, i );
    }
  
#if SHOWRESULT  
    printf( "R \n" );
    printMatrix( R, N, N );
    printf( "Q \n" );
    printMatrix( Q, M, N );
#endif

    /* Checks: A ?= QR and orthogonality of Q */

#if DEBUG
    printf( "Initial matrix:\n" );
    printMatrix( A, M, N );

    printf( "Result check: QR = A\n" );

    memset( A, (char)0, M*N*sizeof( double ));
    matmul( A, Q, R, M, N, N );
    printMatrix( A, M, N );

    printf( "Unitarity check: Q*Q' = Q'*Q = I\n" );

    memset( R, (char)0, M*N*sizeof( double ));
    memcpy( A, Q, M*N*sizeof( double ));
    transpose( A, M, N );
    matmul( R, Q, A, M, N, N );
    printMatrix( R, M, N );
#endif

    free( w );
}

