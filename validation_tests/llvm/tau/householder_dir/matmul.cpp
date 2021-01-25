void matmul(  int M, int K, int N, double** out, double** A, double** B );

void matmul(  int M, int K, int N, double** out, double** A, double** B ){
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

