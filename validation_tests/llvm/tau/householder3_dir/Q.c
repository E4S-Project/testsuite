void applyQ( int len, double Q[len][len], double w[len], double tau, int start ){
    int i, j;
    double tmpM[len][len];
    double tmpV[len];

    /* Q(:,j:end) = Q(:,j:end)-(Q(:,j:end)*w)*(tau*w)’; */

    /* tmpV = (Q(:,j:end)*w) */

    for( j = 0 ; j < len ; j++ ) {
      tmpV[j] = 0.0;
      for( i = start ; i < len ; i++ ) {
    tmpV[j] += Q[ j ][i ] * w[i];
      }
    }

    /* tmpM = tmpV * (tau*w) */

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
}
