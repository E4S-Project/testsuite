#include <stdio.h>
#include <stdlib.h>

#define M 256
#define N 256
#define K 256

void initRand( int, int, double[][*] );
void printMatrix( int, int, double[][*] );

int main(){

	double A[M][K], B[K][N], C[M][N];

    initRand( M, K, A );
    initRand( K, N, B );
    
	for (int i = 0; i < M; i+=1)
		for (int j = 0; j < N; j+=1)
			for (int k = 0; k < K; k+=1)
				C[i][j] += A[i][k] * B[k][j];
    
    printMatrix( M, N, C );

	return EXIT_SUCCESS;
}

void printMatrix( int m, int n, double mat[m][n] ){
    int i, j;
    printf( "-------------------------------------\n" );
    for( i = 0 ; i < m ; i++ ){
        for( j = 0 ; j < n ; j++ ){
            printf( "%.2lf \t ", mat[i][j] );   
        }
        printf( "\n" );
    }
    printf( "-------------------------------------\n" );
}

void initRand( int m, int n, double mat[m][n] ){
    int i,j;
    srand( 0 );
    for( i = 0 ; i < m ; i++ ){
        for( j = 0 ; j < n ; j++ ){
            mat[i][j] = (double) rand() / (double) ( RAND_MAX + 1 );
        }
    }
}
