unsigned int min( unsigned int, unsigned int );

void copyR( int M, int N, double dst[N][N], double orig[M][N] ){
    for( int i = 0 ; i < min( M, N ) ; i++ ) {
        for( int j = 0 ; j < min( M, N ) ; j++ ) {
            dst[i][j] = orig[i][j];
        }
    }
}

void applyR( int len, double R[len][len], double w[len], double tau, int start ){
    int i, j;
    double tmpM[len][len];
    double tmpV[len];

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

}

