#include <stdio.h>
#include <stdlib.h>

#define M 256
#define N 256
#define K 256


int main(){

	double A[M][K], B[K][N], C[M][N];

#pragma clang loop(j2) pack array(A)
#pragma clang loop(i1) pack array(B)
#pragma clang loop(i1,j1,k1,i2,j2,k2) interchange \
	permutation(j1,k1,i1,j2,i2,k2)
#pragma clang loop(i,j,k) tile sizes(96,2048,256) \
	pit_ids(i1,j1,k1) tile_ids(i2,j2,k2)

#pragma clang loop id(i)
	for (int i = 0; i < M; i+=1)
#pragma clang loop id(j)
		for (int j = 0; j < N; j+=1)
#pragma clang loop id(k)
			for (int k = 0; k < K; k+=1)
				C[i][j] += A[i][k] * B[k][j];


	return EXIT_SUCCESS;
}
