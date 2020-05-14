void matmul(  int M, int K, int N, double out[M][N], double A[M][K], double B[K][N] ){
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

