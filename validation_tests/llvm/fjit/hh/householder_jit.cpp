#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#include <iostream>

#define DEFAULT_M 16
#define DEFAULT_N 16

#define DEBUG      0
#define SHOWRESULT 0
#define CHECKPRECI 1e-8

template <typename T, unsigned int, unsigned int>[[clang::jit]] void printMatrix( T** );
template <typename T, unsigned int, unsigned int>[[clang::jit]] T** allocateMatrix( void );
template <typename T>[[clang::jit]] T** allocateMatrix( unsigned int, unsigned int );
template <typename T, unsigned int>[[clang::jit]] T* allocateVector( void );
template <typename T>[[clang::jit]] T* allocateVector( unsigned int );
template <typename T, unsigned int>[[clang::jit]] void freeMatrix( T**& );
template <typename T>[[clang::jit]] void freeMatrix( T**&, unsigned int );
template <typename T> [[clang::jit]] void freeVector( T*& );

template <typename T, unsigned int, unsigned int>[[clang::jit]] void initRand( T** );
template <typename T>[[clang::jit]] void initRand( T**, unsigned int, unsigned int );

template <typename T, unsigned int, unsigned int>[[clang::jit]] void initZero( T** );
template <typename T>[[clang::jit]] void initZero( T**, unsigned int, unsigned int );

template <typename T, unsigned int, unsigned int>[[clang::jit]] void initUnit( T** );
template <typename T>[[clang::jit]] void initUnit( T**, unsigned int, unsigned int );

template <typename T, unsigned int>[[clang::jit]] T getnorm( T* );
template <typename T> [[clang::jit]] T getnorm( unsigned int, T* );

template <typename T, unsigned int, unsigned int>[[clang::jit]] bool isEqual( T**, T** );
template <typename T>[[clang::jit]] bool isEqual( unsigned int, unsigned int, T**, T** );

template <typename T>[[clang::jit]] T getsign( T );

template <typename T, unsigned int> [[clang::jit]] void normalize( T, T* );
template <typename T> [[clang::jit]] void normalize( unsigned int, T, T* );

template <typename T, unsigned int, unsigned int>[[clang::jit]] void printMatrix( T** );

template <typename T> [[clang::jit]] void matmul( unsigned int, unsigned int, unsigned int, T**, T**, T** );
template <typename T, unsigned int, unsigned int, unsigned int>[[clang::jit]] void matmul( T**, T**, T** );

template <typename T>[[clang::jit]] bool checkCorrect( unsigned int, unsigned int, T**, T**, T** );
template <typename T, unsigned int, unsigned int>[[clang::jit]] bool checkCorrect( T**, T**, T** );

template <typename T, unsigned int, unsigned int>[[clang::jit]] bool checkUnitary( T** );
template <typename T>[[clang::jit]] bool checkUnitary( T**, unsigned int, unsigned int );

template <typename T, unsigned int, unsigned int>[[clang::jit]] bool check( T**, T**, T** );
template <typename T>[[clang::jit]] bool check( unsigned int, unsigned int, T**, T**, T** );

template <typename T>[[clang::jit]] void transpose( unsigned int, unsigned int, T** );
template <typename T, unsigned int, unsigned int>[[clang::jit]] void transpose( T** );

template <typename T, unsigned int, unsigned int>[[clang::jit]] void copyR( T**, T** );
template <typename T>[[clang::jit]] void copyR( unsigned int, unsigned int, T**, T** );

template <typename T> [[clang::jit]] void applyR( unsigned int, T**, T*, T, unsigned int );
template <typename T, unsigned int> [[clang::jit]] void applyR( T**, T*, T, unsigned int );

template <typename T> [[clang::jit]] void applyQ( unsigned int, T**, T*, T, unsigned int );
template <typename T, unsigned int> [[clang::jit]] void applyQ( T**, T*, T, unsigned int );

template <typename T>[[clang::jit]] void householder( unsigned int, unsigned int, T**, T**, T** );
template <typename T, unsigned int, unsigned int>[[clang::jit]] void householder( T**, T**, T** );

unsigned int min( unsigned int, unsigned int );
double getTime( void );
void printPerf( int, int, double );


int main( int argc, char** argv ){

    unsigned int M, N;
    double time_start, time_end;

    /* Init */
    
    if( argc < 3 ) {
        M = DEFAULT_M;
        N = DEFAULT_N;
    } else {
        M = atoi( argv[1] );
        N = atoi( argv[2] );
    }

    auto A = allocateMatrix<double, M, N>();
    auto R = allocateMatrix<double, N, N>();
    auto Q = allocateMatrix<double, M, N>();

    initRand<double, M, N>( A );

    /* Here comes the fun */

    time_start = getTime();
    householder<double, M, N>( A, Q, R );
    //  householder<double>( M, N, A, Q, R );
    time_end   = getTime();

    printPerf( M, N, (time_end - time_start ) );

    /* Checks */

#if SHOWRESULT  
    printf( "R \n" );
    printMatrix<double, N, N>( R );
    printf( "Q \n" );
    printMatrix<double, M, N>( Q );
#endif

    /* Checks: A ?= QR and orthogonality of Q */

    if( true == check<double, M, N>( A, Q, R ) ){
        printf( "[PASS]\n" );
    } else {
        printf( "[FAIL]\n" );
    }

#if DEBUG
    printf( "Initial matrix:\n" );
    printMatrix<double, M, N>( A );

    printf( "Result check: QR = A\n" );

    //    memset( A, (char)0, M*N*sizeof( double ));
    for( int i = 0 ; i < M ; i++ ) {
        for( int j = 0 ; j < N ; j++ ) {
            A[i][j] = 0.0;
        }
    }
    matmul<double, M, N, N>( A, Q, R );
    printMatrix<double, M, N>( A );

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
    transpose<double, M, N>( A );
    matmul<double, M, N, N>( R, Q, A );
    printMatrix<double, M, N>( R );
#endif
    
    freeMatrix<double, M>( A );
    freeMatrix<double, M>( Q );
    freeMatrix<double, N>( R );

    return EXIT_SUCCESS;
}

template <typename T, unsigned int lines, unsigned int cols >[[clang::jit]]
void initRand( T** mat ){
    srand( 0 );
    for( auto i = 0 ; i < lines ; i++ ) {
        for( auto j = 0 ; j < cols ; j++ ) {
            mat[i][j] = (double) rand() / (double) ( (unsigned int)RAND_MAX + 1 );
        }
    }
}

template <typename T>[[clang::jit]]
void initRand( T** mat, unsigned int lines, unsigned int cols ){
    srand( 0 );
    for( auto i = 0 ; i < lines ; i++ ) {
        for( auto j = 0 ; j < cols ; j++ ) {
            mat[i][j] = (double) rand() / (double) ( (unsigned int)RAND_MAX + 1 );
        }
    }
}

template <typename T, unsigned int lines, unsigned int cols >[[clang::jit]]
void initZero( T** mat ){
    for( auto i = 0 ; i < lines ; i++ ) {
        for( auto j = 0 ; j < cols ; j++ ) {
            mat[i][j] = 0;
        }
    }
}

template <typename T >[[clang::jit]]
void initZero( unsigned int lines, unsigned int cols, T** mat ){
    for( auto i = 0 ; i < lines ; i++ ) {
        for( auto j = 0 ; j < cols ; j++ ) {
            mat[i][j] = 0;
        }
    }
}

template <typename T, unsigned int lines, unsigned int cols >[[clang::jit]]
void initUnit( T** mat ){
  initZero<T, lines, cols>( mat );
  for( auto i = 0 ; i < lines && i < cols ; i++ ) {
    mat[i][i] = 1;
  }
}

template <typename T >[[clang::jit]]
void initUnit( unsigned int l, unsigned int c,  T** mat ){
  initZero<T>( l, c, mat );
  for( auto i = 0 ; i < l && i < c ; i++ ) {
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

template <typename T, unsigned int len>
[[clang::jit]] T getnorm( T* vec ) {
  T n = vec[0]*vec[0];
  for( auto i = 1 ; i < len ; i++ ) {
    n += ( vec[i] * vec[i] );
  }
  return sqrtl( n );
}

template <typename T>
[[clang::jit]] T getnorm( unsigned int len, T* vec ) {
  T n = vec[0]*vec[0];
  for( auto i = 1 ; i < len ; i++ ) {
    n += ( vec[i] * vec[i] );
  }
  return sqrtl( n );
}

template <typename T, unsigned int lines, unsigned int cols >
[[clang::jit]] void printMatrix( T** ptr ){
  for( auto i = 0 ; i < lines ; i++ ) {
    for( auto j = 0 ; j < cols ; j++ ) {
      std::cout << ptr[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

template <typename T, unsigned int lines, unsigned int cols >
[[clang::jit]] T** allocateMatrix( ){
  T** ptr = new T*[lines];
  for( auto i = 0 ; i < lines ; i++ ) {
    ptr[i] = new T[cols];
  }
  return ptr;
}

template <typename T>
[[clang::jit]] T** allocateMatrix( unsigned int lines, unsigned int cols ){
  T** ptr = new T*[lines];
  for( auto i = 0 ; i < lines ; i++ ) {
    ptr[i] = new T[cols];
  }
  return ptr;
}

template <typename T, unsigned int size>
[[clang::jit]] T* allocateVector( ){
  T* ptr = new T[size];
  return ptr;
}

template <typename T>
[[clang::jit]] T* allocateVector( unsigned int size ){
  T* ptr = new T[size];
  return ptr;
}

template <typename T, unsigned int lines>
[[clang::jit]] void freeMatrix( T**& ptr ){
  for( auto i = 0 ; i < lines ; i++ ) {
    delete [] ptr[i];
  }
  delete[] ptr;
}

template <typename T>
[[clang::jit]] void freeMatrix( T**& ptr, unsigned int lines ){
  for( auto i = 0 ; i < lines ; i++ ) {
    delete [] ptr[i];
  }
  delete[] ptr;
}

template <typename T>
[[clang::jit]] void freeVector( T*& ptr ){
  delete[] ptr;
}

template <typename T, unsigned int M, unsigned int N>
[[clang::jit]] bool isEqual( T** A, T** B ) {
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

template <typename T>
[[clang::jit]] bool isEqual( unsigned int M, unsigned int N, T** A, T** B ) {
    unsigned int i, j;
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

template <typename T, unsigned int M, unsigned int N>
[[clang::jit]] bool checkCorrect( T** A, T** Q, T** R ) {

    bool eq = true;

    /* We want QR == A */

    auto tmp = allocateMatrix<double, M, N>();

    // memset( tmp, (char)0, M*N*sizeof( double  ) );
    for( auto i = 0 ; i < N ; i++ ) {
        for( auto j = 0 ; j < N ; j++ ) {
            tmp[i][j] = 0.0;
        }
    }
    matmul<T, M, N, N>( tmp, Q, R );  
    eq = isEqual<T, M, N>( tmp, A );
    
    freeMatrix<T, M>( tmp );
    return eq;
}

template <typename T>
[[clang::jit]] bool checkCorrect( unsigned int M, unsigned int N, T** A, T** Q, T** R ) {

    bool eq = true;

    /* We want QR == A */

    auto tmp = allocateMatrix<T, M, N>();

    // memset( tmp, (char)0, M*N*sizeof( double  ) );
    for( auto i = 0 ; i < N ; i++ ) {
        for( auto j = 0 ; j < N ; j++ ) {
            tmp[i][j] = 0.0;
        }
    }
    matmul<T>( M, N, N, tmp, Q, R );
    eq = isEqual<T, M, N>( M, N, tmp, A );
    
    freeMatrix<T, M>( tmp );
    return eq;
}

template <typename T>
[[clang::jit]] bool checkUnitary( unsigned int M, unsigned int N, T** Q ) {

  auto tmp = allocateMatrix<T, M, N>();
  auto res = allocateMatrix<T, N, N>();
  auto I = allocateMatrix<T, M, N>();

  bool ret = true;

    /* We want Q*Q' = Q'*Q = I */

  initUnit<T>( M, M, I ); 

//    memset( res, (char)0, M*N*sizeof( double ) );
    for( auto i = 0 ; i < N ; i++ ) {
        for( auto j = 0 ; j < N ; j++ ) {
            res[i][j] = 0.0;
        }
    }
//    memcpy( tmp, Q, M*N*sizeof( double ));
    for( auto i = 0 ; i < M ; i++ ) {
        for( auto j = 0 ; j < N ; j++ ) {
            tmp[i][j] = Q[i][j];
        }
    }
    transpose<T>( M, N, tmp ); 
    matmul<T>( M, N, N, res, Q, tmp );  

    if( false == isEqual<T>( M, M, I, res ) )
        return false;

    // memset( res, (char)0, M*N*sizeof( double ) );
    for( auto i = 0 ; i < N ; i++ ) {
        for( auto j = 0 ; j < N ; j++ ) {
            res[i][j] = 0.0;
        }
    }
    matmul<T>( M, N, N, res, tmp, Q ); 

    ret = isEqual<T>( M, M, I, res );
    
    freeMatrix<T, M>( tmp );
    freeMatrix<T, N>( res );
    freeMatrix<T, M>( I );
    
    return ret;
}

template <typename T, unsigned int M, unsigned int N>
[[clang::jit]] bool checkUnitary( T** Q ) {

  auto tmp = allocateMatrix<T, M, N>();
  auto res = allocateMatrix<T, N, N>();
  auto I = allocateMatrix<T, M, N>();

  bool ret = true;

    /* We want Q*Q' = Q'*Q = I */

  initUnit<T, M, M>( I ); 

//    memset( res, (char)0, M*N*sizeof( double ) );
    for( auto i = 0 ; i < N ; i++ ) {
        for( auto j = 0 ; j < N ; j++ ) {
            res[i][j] = 0.0;
        }
    }
//    memcpy( tmp, Q, M*N*sizeof( double ));
    for( auto i = 0 ; i < M ; i++ ) {
        for( auto j = 0 ; j < N ; j++ ) {
            tmp[i][j] = Q[i][j];
        }
    }
    transpose<T, M, N>( tmp ); 
    matmul<T, M, N, N>( res, Q, tmp );

    if( false == isEqual<T, M, M>( I, res ) )
        return false;

    // memset( res, (char)0, M*N*sizeof( double ) );
    for( auto i = 0 ; i < N ; i++ ) {
        for( auto j = 0 ; j < N ; j++ ) {
            res[i][j] = 0.0;
        }
    }
    matmul<T, M, N, N>( res, tmp, Q );

    ret = isEqual<T, M, M>( I, res );
    
    freeMatrix<T, M>( tmp );
    freeMatrix<T, N>( res );
    freeMatrix<T, M>( I );
    
    return ret;
}

template <typename T>
[[clang::jit]] bool check( unsigned int  M, unsigned int N, T** A, T** Q, T** R ) {
  if( false == checkCorrect<T>( M, N, A, Q, R ) ) {
    printf( "Incorrect result: A != QR (precision requested: %e)\n", CHECKPRECI );
    return false;
  }
  if( false == checkUnitary<T>( M, N, Q ) ) {
    printf( "Incorrect result: Q is not unitary (precision requested: %e)\n", CHECKPRECI );
    return false;
  }
  
  return true;
}

template <typename T, unsigned int M, unsigned int N>
[[clang::jit]] bool check( T** A, T** Q, T** R ) {
  if( false == checkCorrect<T, M, N>( A, Q, R ) ) {
        printf( "Incorrect result: A != QR (precision requested: %e)\n", CHECKPRECI );
        return false;
    }
  if( false == checkUnitary<T, M, N>( Q ) ) {
        printf( "Incorrect result: Q is not unitary (precision requested: %e)\n", CHECKPRECI );
        return false;
    }

    return true;
}

template <typename T>
[[clang::jit]] void transpose( unsigned int M, unsigned int N, T** A ) {
  T tmp;

    for( auto i = 0 ; i < M ; i++ ) {
        for( auto j = i ; j < N ; j++ ) {
            tmp = A[ i ][ j ];
            A[ i ][ j ] = A[ j ][ i ];
            A[ j ][ i ] = tmp;
        }
    }
}

template <typename T, unsigned int M, unsigned int N>
[[clang::jit]] void transpose( T** A ) {
  T tmp;

    for( auto i = 0 ; i < M ; i++ ) {
        for( auto j = i ; j < N ; j++ ) {
            tmp = A[ i ][ j ];
            A[ i ][ j ] = A[ j ][ i ];
            A[ j ][ i ] = tmp;
        }
    }
}

template <typename T>
[[clang::jit]] T getsign( T d ) {
    return ( d > 0 ) ? 1.0 : -1.0;       
}

template <typename T>
[[clang::jit]] void normalize( unsigned int len, T div, T* vec ) {
    for( auto i = 0 ; i < len ; i++ ) {
        vec[i] /= div;
    }
}

template <typename T, unsigned int len>
[[clang::jit]] void normalize( T div, T* vec ) {
    for( auto i = 0 ; i < len ; i++ ) {
        vec[i] /= div;
    }
}

unsigned int min( unsigned int a, unsigned int b ){
    return ( (a < b) ? a : b );
}

template <typename T, unsigned int M, unsigned int N>
[[clang::jit]] void copyR( T** dst, T** orig ){
    for( auto i = 0 ; i < min( M, N ) ; i++ ) {
        for( auto j = 0 ; j < min( M, N ) ; j++ ) {
            dst[i][j] = orig[i][j];
        }
    }
}

template <typename T>
[[clang::jit]] void copyR( unsigned int M, unsigned int N, T** dst, T **orig ){
    for( auto i = 0 ; i < min( M, N ) ; i++ ) {
        for( auto j = 0 ; j < min( M, N ) ; j++ ) {
            dst[i][j] = orig[i][j];
        }
    }
}

template <typename T>
[[clang::jit]] void matmul( unsigned int M, unsigned int K, unsigned int N, T** out, T** A, T** B ){
    for( auto i = 0 ; i < M ; i++ ){ 
        for( auto j = 0 ; j < N ; j++ ){
            for( auto k = 0 ; k < K ; k++ ){
                out[ i ][ j ] += A[ i ][ k ] * B[ k ][ j ];
            }
        }
    }
}

template <typename T, unsigned int M, unsigned int N, unsigned int K>
[[clang::jit]] void matmul( T** out, T** A, T** B ){
    for( auto i = 0 ; i < M ; i++ ){ 
        for( auto j = 0 ; j < N ; j++ ){
            for( auto k = 0 ; k < K ; k++ ){
                out[ i ][ j ] += A[ i ][ k ] * B[ k ][ j ];
            }
        }
    }
}

template <typename T>
[[clang::jit]] void applyR( unsigned int len, T** R, T* w, T tau, unsigned int start ){
    unsigned int i, j;
    auto tmpM = allocateMatrix<T>( len, len );
    auto tmpV = allocateVector<T>( len );

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

    freeMatrix<T>( tmpM, len );
    freeVector<T>( tmpV );
}

template <typename T, unsigned int len>
[[clang::jit]] void applyR( T** R, T* w, T tau, unsigned int start ){
    unsigned int i, j;
    auto tmpM = allocateMatrix<T, len, len>();
    auto tmpV = allocateVector<T, len>();

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

    freeMatrix<T, len>( tmpM );
    freeVector<T>( tmpV );
}

template <typename T>
[[clang::jit]] void applyQ( unsigned int len, T** Q, T* w, T tau, unsigned int start ){
    unsigned int i, j;
    auto tmpM = allocateMatrix<T>( len, len );
    auto tmpV = allocateVector<T>( len );

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

    freeMatrix<T>( tmpM, len );
    freeVector<T>( tmpV );
}

template <typename T, unsigned int len> 
[[clang::jit]] void applyQ( T** Q, T* w, T tau, unsigned int start ){
    unsigned int i, j;
    auto tmpM = allocateMatrix<T, len, len>();
    auto tmpV = allocateVector<T, len>();

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

    freeMatrix<T, len>( tmpM );
    freeVector<T>( tmpV );
}

template <typename T, unsigned int M, unsigned int N>
[[clang::jit]] void householder( T** A, T** Q, T** R ){

  unsigned int i, j, len; 
    T u1, tau, sign, norm;
    T* w = allocateVector<T, N>();

    //memcpy( R, A, ( ( M > N ) ? N : M ) * N * sizeof( double ) );
    copyR<T, M, N>( R, A );
    initUnit<T, M, N>( Q );

    for( i = 0 ; i < M  ; i++ ) {
      
      // memset( w, (char)0, N*sizeof( double ) );
        for( j = 0 ; j < N ; j++ ){
            w[j] = 0.0;
        }
      
      /* H = I - tau * w * w' */
      
      for( j = i ; j < N ; j++ ) {
	w[j] = R[ j ][ i ];
      }
      len = N - i;
      // norm = getnorm<T, len>( w+i );
      norm = getnorm<T>( len, w+i );  // TODO
      sign = -1 * getsign<T>( R[ i ][ i ] );
      u1 = R[ i ][ i ] - sign * norm;
      
      len = N - i - 1;
      //      normalize<T, len>( u1, w+i+1 ); // w = R(j:end,j)/u1;
      normalize<T>( len, u1, w+i+1 ); // w = R(j:end,j)/u1; TODO
      w[ i ] = 1;
      tau = -1 * sign * u1 / norm;
      
        /* R = HR; Q = QH */

     if( i < N ) {
       applyR<T, N>( R, w, tau, i ); 
      }
     applyQ<T, N>( Q, w, tau, i ); 
    }
    freeVector<T>( w );
}

template <typename T>
[[clang::jit]] void householder( unsigned int M, unsigned int N, T** A, T** Q, T** R ){

    unsigned int i, j; 
    T u1, tau, sign, norm;
    T* w = allocateVector<T>( N );

    //memcpy( R, A, ( ( M > N ) ? N : M ) * N * sizeof( double ) );
    copyR<T>( M, N, R, A );
    initUnit<T>( M, N, Q );

    for( i = 0 ; i < M  ; i++ ) {
      
      // memset( w, (char)0, N*sizeof( double ) );
        for( j = 0 ; j < N ; j++ ){
            w[j] = 0.0;
        }
      
      /* H = I - tau * w * w' */
      
      for( j = i ; j < N ; j++ ) {
	w[j] = R[ j ][ i ];
      }
      norm = getnorm<T>( N-i, w+i );
      sign = -1 * getsign<T>( R[ i ][ i ] );
      u1 = R[ i ][ i ] - sign * norm;
      
      normalize<T>( N-i-1, u1, w+i+1 ); // w = R(j:end,j)/u1;
      w[ i ] = 1;
      tau = -1 * sign * u1 / norm;
      
        /* R = HR; Q = QH */

     if( i < N ) {
       applyR<T>( N, R, w, tau, i );
      }
     applyQ<T>( N, Q, w, tau, i );
    }
    freeVector<T>( w );
}
