#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <iostream>

#define DEFAULT_M 16
#define DEFAULT_N 16

#define CHECKPRECI_D 1e-8
#define CHECKPRECI_F 1e-4

template<typename T> void initRand( int, int, T** );
template<typename T> void initZero( int, int, T** );
template<typename T> void initUnit( int, int, T** );
template<typename T> void transpose( int, int, T** );
template<typename T> T getnorm( int, T* );
template<typename T> double getsign( T ) ;
template<typename T> void normalize( int, T, T* );
template<typename T> void printMatrix( int, int, T** );
template<typename T> void printMatrixOctave( int, int, T** );
template<typename T> void matmul( int, int, int, T**, T**, T** );
template<typename T> bool check( int, int, T**, T**, T**, double );
double getTime( void );
void printPerf( int, int, double );
template<typename T> void applyR( int, T**, T*, T, int );
template<typename T> void applyQ( int, T**, T*, T, int );
template<typename T> void copyR( int, int, T**, T** );

template<typename T> void householder( int, int, T**, T**, T** );

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

    /* Double precision */
    
    //    double A[M][N], R[N][N], Q[M][N];
    auto A = new double*[M];
    for( auto i = 0 ; i < M ; i++ ) A[i] = new double[N];
    auto R = new double*[N];
    for( auto i = 0 ; i < N ; i++ ) R[i] = new double[N];
    auto Q = new double*[M];
    for( auto i = 0 ; i < M ; i++ ) Q[i] = new double[N];

    initRand( M, N, A );

    time_start = getTime();
    householder( M, N, A, Q, R );
    time_end   = getTime();

    printPerf( M, N, (time_end - time_start ) );

    /* Checks: A ?= QR and orthogonality of Q */

    if( true == check( M, N, A, Q, R, CHECKPRECI_D ) ){
        printf( "[PASS]\n" );
    } else {
        printf( "[FAIL]\n" );
    }

    /* Single precision */

    auto A_f = new float*[M];
    for( auto i = 0 ; i < M ; i++ ) A_f[i] = new float[N];
    auto R_f = new float*[N];
    for( auto i = 0 ; i < N ; i++ ) R_f[i] = new float[N];
    auto Q_f = new float*[M];
    for( auto i = 0 ; i < M ; i++ ) Q_f[i] = new float[N];

    initRand( M, N, A_f );

    time_start = getTime();
    householder( M, N, A_f, Q_f, R_f );
    time_end   = getTime();

    printPerf( M, N, (time_end - time_start ) );

    /* Checks: A ?= QR and orthogonality of Q */

    if( true == check( M, N, A_f, Q_f, R_f, CHECKPRECI_F ) ){
        printf( "[PASS]\n" );
    } else {
        printf( "[FAIL]\n" );
    }

    for( auto i = 0 ; i < M ; i++ ) delete[] A[i];
    delete[] A;
    for( auto i = 0 ; i < M ; i++ ) delete[] Q[i];
    delete[] Q;
    for( auto i = 0 ; i < N ; i++ ) delete[] R[i];
    delete[] R;
    return EXIT_SUCCESS;
}

template<typename T> void initRand( int lines, int col, T** mat ){
    int i, j;
    srand( 0 );
    for( i = 0 ; i < lines ; i++ ) {
        for( j = 0 ; j < col ; j++ ) {
            mat[i][j] = (T) rand() / (T) ( (unsigned int)RAND_MAX + 1 );
        }
    }
}

template<typename T> void initZero( int lines, int col, T** mat ){
    int i, j;
    for( i = 0 ; i < lines ; i++ ) {
        for( j = 0 ; j < col ; j++ ) {
            mat[i][j] = 0;
        }
    }
}

template<typename T> void initUnit( int l, int c,  T** mat ){
    int i;
    initZero( l, c, mat );
    for( i = 0 ; i < l && i < c ; i++ ) {
        mat[i][i] = 1;
    }
}

double getTime(){
    struct timespec tv;
    clock_gettime( CLOCK_REALTIME, &tv );
    return ( tv.tv_nsec + tv.tv_sec*1e9 );
}

void printPerf( int M, int N, double time ) {
    double flops = 2.0 * (double) M * (double) N * (double) N / 3.0;
    time *= 1e-3;
    printf( "%d \t %d \t %.0lf usec \t %.3lf Mflops\n", M, N, time, flops / time );
}

template<typename T> T getnorm( int len, T* vec ) {
    int i;
    T n = vec[0]*vec[0];
    for( i = 1 ; i < len ; i++ ) {
      n += ( vec[i] * vec[i] );
    }
    return sqrtl( n );
}

template<typename T> void printMatrix( int M, int N, T** mat ){
    int i, j;
    std::cout << "-------------------------------------" << std::endl;
    for( i = 0 ; i < M ; i++ ){
        for( j = 0 ; j < N ; j++ ){
            std::cout << mat[ i ][ j ] << "\t";
        } 
        std::cout  << std::endl;
    }
    std::cout << "-------------------------------------" << std::endl;
}

template<typename T> void printMatrixOctave( int M, int N, T** mat ){
    int i, j;
    std::cout << "-------------------------------------" << std::endl;
    std::cout << "[ " << std::endl;
    for( i = 0 ; i < M ; i++ ){
        std::cout << "[ " << std::endl;
        for( j = 0 ; j < N ; j++ ){
            std::cout << mat[ i ][ j ] << ", ";
            
        }
        std::cout << "];" << std::endl;
    }
    std::cout << "]" << std::endl;
    std::cout << "-------------------------------------" << std::endl;
}

template<typename T> bool isEqual( int M, int N, T** A, T** B, double precision ) {
    int i, j;
    bool eq = true;

    for( i = 0 ; i < M ; i++ ) {
        for( j = 0 ; j < N ; j++ ) {
            if( fabs( B[i][j] - A[i][j] ) > precision ) {
                eq = false;
            }
        }
    }
    return eq;
}

template<typename T> bool checkCorrect( int M, int N, T** A, T** Q, T** R, double precision ) {

    bool eq = true;

    /* We want QR == A */

   auto tmp = new T*[M];
   for( auto i = 0 ; i < M ; i++ ) tmp[i] = new T[N];

    // memset( tmp, (char)0, M*N*sizeof( double  ) );
    for( int i = 0 ; i < N ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            tmp[i][j] = 0.0;
        }
    }
    matmul( M, N, N, tmp, Q, R );
    eq = isEqual( M, N, tmp, A, precision );
    
    for( auto i = 0 ; i < N ; i++ ) delete[] tmp[i];
    delete[] tmp;
    return eq;
}

template<typename T> bool checkUnitary( int M, int N, T** Q, double precision ) {
  auto tmp = new T*[M];
  for( auto i = 0 ; i < M ; i++ ) tmp[i] = new T[N];
  auto res = new T*[N];
  for( auto i = 0 ; i < N ; i++ ) res[i] = new T[N];
  auto I = new T*[M];
  for( auto i = 0 ; i < M ; i++ ) I[i] = new T[M];

  bool ret = true;

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

    if( false == isEqual( M, M, I, res, precision ) )
        return false;

    // memset( res, (char)0, M*N*sizeof( double ) );
    for( int i = 0 ; i < N ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            res[i][j] = 0.0;
        }
    }
    matmul( M, N, N, res, tmp, Q );

    ret = isEqual( M, M, I, res, precision );
    
    for( auto i = 0 ; i < M ; i++ ) delete[] tmp[i];
    delete[] tmp;
    for( auto i = 0 ; i < N ; i++ ) delete[] res[i];
    delete[] res;
    for( auto i = 0 ; i < M ; i++ ) delete[] I[i];
    delete[] I;

    return ret;
}

template<typename T> bool check( int  M, int N, T** A, T** Q, T** R, double precision ) {
    if( false == checkCorrect( M, N, A, Q, R, precision ) ) {
        printf( "Incorrect result: A != QR (precision requested: %e)\n", precision );
        return false;
    }
    if( false == checkUnitary( M, N, Q, precision ) ) {
        printf( "Incorrect result: Q is not unitary (precision requested: %e)\n", precision );
        return false;
    }

    return true;
}

template<typename T> void transpose( int M, int N, T** A ) {
    int i, j;
    T tmp;

    for( i = 0 ; i < M ; i++ ) {
        for( j = i ; j < N ; j++ ) {
            tmp = A[ i ][ j ];
            A[ i ][ j ] = A[ j ][ i ];
            A[ j ][ i ] = tmp;
        }
    }
}

template<typename T> double getsign( T d ) {
    return ( d > 0 ) ? 1.0 : -1.0;       
}

template<typename T> void normalize( int len, T div, T* vec ) {
    int i;
    for( i = 0 ; i < len ; i++ ) {
        vec[i] /= div;
    }
}

template<typename T> T min( T a, T b ){
    return ( (a < b) ? a : b );
}

template<typename T> void copyR( int M, int N, T** dst, T **orig ){
    int i, j;
    for( i = 0 ; i < min( M, N ) ; i++ ) { 
        for( j = 0 ; j < min( M, N ) ; j++ ) {
            dst[i][j] = orig[i][j];
        }
    }
}

template<typename T> void applyR( int len, T** R, T* w, T tau, int start ){
    int i, j;
    auto tmpM = new T*[len];
    for( i = 0 ; i < len ; i++ ) tmpM[i] = new T[len];
    auto  tmpV = new T[len];

    /* R(j:end,:) = R(j:end,:)-(tau*w)*(w’*R(j:end,:)); */

     /* tmpV = w'*R(j:end,:) */

    //memset( tmpV, (char) 0, len*sizeof( double ) );
    for( i = 0 ; i < len ; i++ ) {
        tmpV[i] = 0.0;
    }
    for( j = 0 ; j < len ; j++ ){
      for( i = start ; i < len ; i++ ) {
    tmpV[j] += w[i]*R[ i ][ j ];
      }
    }

    /* tmpM = tau * w * tmpV */

    for( i = 0 ; i < len ; i++ ){
      for( j = start ; j < len ; j++ ){
      tmpM[ j ][ i ] = tau * w[ j ] * tmpV[ i ];
    }
    }

    /* R = R - tmpM */

    for( i = start ; i < len ; i++ ){
        for( j = 0 ; j < len ; j++ ){
            R[ i ][ j] -= tmpM[ i ][ j ];
        }
    }

    delete[] tmpV;
    for( i = 0 ; i < len ; i++ ) delete[] tmpM[i];
    delete[] tmpM;
}

template<typename T> void applyQ( int len, T** Q, T* w, T tau, int start ){
    int i, j;
    auto tmpM = new T*[len];
    for( i = 0 ; i < len ; i++ ) tmpM[i] = new T[len];
    auto tmpV = new T[len];

    /* Q(:,j:end) = Q(:,j:end)-(Q(:,j:end)*w)*(tau*w)’; */

    /* tmpV = (Q(:,j:end)*w) */

    for( j = 0 ; j < len ; j++ ) {
      tmpV[j] = 0.0;
      for( i = start ; i < len ; i++ ) {
          tmpV[j] += Q[ j ][i ] * w[i];
      }
    }
    
    /* tmpM = tmpV * (tau*w)’; */
    
    for( j = 0 ; j < len ; j++ ) {
        for( i = start  ; i < len ; i++ ) {
            tmpM[ j ][ i ] = tmpV[j] * w[i] * tau;
        }
    }
    
    /* Q(:,j:end) -= tmpM */
    
    for( j = 0 ; j < len ; j++ ) {
        for( i = start ; i < len ; i++ ) {
            Q[ j ][ i ] -= tmpM[ j ][ i ];
        }
    }

    for( i = 0 ; i < len ; i++ ) delete[] tmpM[i];
    delete[] tmpM;
    delete[] tmpV;
}

template<typename T> void matmul(  int M, int K, int N, T** out, T** A, T** B ){
    int i, j, k;

    for( i = 0 ; i < M ; i++ ){
        for( j = 0 ; j < N ; j++ ){
      //            out[ i * N + j ] = 0.0;
            for( k = 0 ; k < K ; k++ ){
                out[ i ][ j ] += A[ i ][ k ] * B[ k ][ j ];
            }
        }
    }
}

template<typename T> void householder( int M, int N, T** A, T** Q, T** R ){

    int i, j; 
    T u1, tau, sign, norm;
    T* w = new T[N]();

    //memcpy( R, A, ( ( M > N ) ? N : M ) * N * sizeof( double ) );
    copyR( M, N, R, A );
    initUnit( M, N, Q );

    for( i = 0 ; i < M  ; i++ ) {
      
      // memset( w, (char)0, N*sizeof( double ) );
        for( j = 0 ; j < N ; j++ ){
            w[j] = 0.0;
        }
      
      /* H = I - tau * w * w' */
      
      for( j = i ; j < N ; j++ ) {
	w[j] = R[ j ][ i ];
      }
      norm = getnorm( N-i, w+i );
      sign = -1 * getsign( R[ i ][ i ] );
      u1 = R[ i ][ i ] - sign * norm;
      
      normalize( N-i-1, u1, w+i+1 ); // w = R(j:end,j)/u1;
      w[ i ] = 1;
      tau = -1 * sign * u1 / norm;
      
        /* R = HR; Q = QH */

      if( i < N ) {
          applyR( N, R, w, tau, i );
      }
      applyQ( N, Q, w, tau, i );
    }
    delete[] w;
}
