void foo( int M, double A[M][M][M], double B[M][M][M] ){
  int i, j, k;
  for( i = 0 ; i < M ; i++ ){
    for( j = 0 ; j < M ; j++ ){
      for( k = 0 ; k < M ; k++ ){
	A[i][j][k] = B[i][k][j] * B[j][k][i];
      }
    }
  }
}
