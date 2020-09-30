#include <stdio.h>
#include <stdlib.h>

#define M 128
#define N 128

int main( ){

	int tab[M][N];
	
    //	printf( "hello world\n" );

#pragma clang loop( i ) reverse
#pragma clang loop id( i )
	for( int i = 0 ; i < M ; i+=1 ){
#pragma clang loop id( j )
		for( int j = 0 ; j < N ; j+=1 ){
			tab[i][j] = 0;
		}
	}

	return EXIT_SUCCESS;
}
