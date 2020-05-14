long double sqrtl(long double x);


void initZero( int lines, int col, double mat[ lines ][ col ] ){
    int i, j;
    for( i = 0 ; i < lines ; i++ ) {
        for( j = 0 ; j < col ; j++ ) {
            mat[i][j] = 0;
        }
    }
}

void initUnit( int l, int c,  double mat[ l ][ c ] ){
    int i;
    initZero( l, c, mat );
    for( i = 0 ; i < l && i < c ; i++ ) {
        mat[i][i] = 1;
    }
}

double getnorm( int len, double vec[len] ) {
    int i;
    double n = vec[0]*vec[0];
    for( i = 1 ; i < len ; i++ ) {
      n += ( vec[i] * vec[i] );
    }
    return sqrtl( n );
}

void transpose( int M, int N, double A[M][N] ) {
    int i, j;
    double tmp;

    for( i = 0 ; i < M ; i++ ) {
        for( j = i ; j < N ; j++ ) {
            tmp = A[ i ][ j ];
            A[ i ][ j ] = A[ j ][ i ];
            A[ j ][ i ] = tmp;
        }
    }
}

double getsign( double d ) {
    return ( d > 0 ) ? 1.0 : -1.0;       
}

void normalize( int len, double div, double vec[len] ) {
    int i;
    for( i = 0 ; i < len ; i++ ) {
        vec[i] /= div;
    }
}

unsigned int min( unsigned int a, unsigned int b ){
    return ( (a < b) ? a : b );
}

void copyR( int M, int N, double dst[N][N], double orig[M][N] ){
    for( int i = 0 ; i < min( M, N ) ; i++ ) {
        for( int j = 0 ; j < min( M, N ) ; j++ ) {
            dst[i][j] = orig[i][j];
        }
    }
}

double getnorm( int, double[] );
double getsign( double ) ;
void normalize( int, double, double[] );
void applyR( int, double[][*], double[], double, int );
void applyQ( int, double[][*], double[], double, int );


void householder( int M, int N, double A[M][N], double Q[M][N], double R[N][N] ){

    int i, j; 
    double u1, tau, sign, norm;
    double w[N];

    //memcpy( R, A, ( ( M > N ) ? N : M ) * N * sizeof( double ) );
    copyR( M, N, R, A );
    initUnit( M, N, Q );

    for( i = 0 ; i < M  ; i++ ) {
      
      // memset( w, (char)0, N*sizeof( double ) );
        for( j = 0 ; j < N ; j++ ){
            w[j] = 0.0;
        }
      
      /* H = I - tau * w * w' */
      
	///#pragma acc parallel loop copyin( R ) copyout( w )
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
  
}
