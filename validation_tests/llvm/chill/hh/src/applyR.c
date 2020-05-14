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

