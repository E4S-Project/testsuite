void matmul(  int M, int K, int N, double out[M][N], double A[M][K], double B[K][N] ){
    for( int i = 0 ; i < M ; i++ ){ 
        for( int j = 0 ; j < N ; j++ ){
	  //            out[ i * N + j ] = 0.0;
            for( int k = 0 ; k < K ; k++ ){
                out[ i ][ j ] += A[ i ][ k ] * B[ k ][ j ];
            }
        }
    }
}

