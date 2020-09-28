#include <stdio.h>
#include <string.h>

__attribute__((noinline))
void matmul(int M, int N, int K, double C[const restrict static M][N], double A[const restrict static M][K], double B[const restrict static K][N]) {
#pragma clang loop(j2) pack array(A) allocate(malloc)
#pragma clang loop(i2) pack array(B) allocate(malloc)
#pragma clang loop(i2,j2,k2) interchange permutation(k2,j2,i2)
#pragma clang loop(i,j,k) tile sizes(96,2048,256)  tile_ids(i2,j2,k2)
#pragma clang loop id(i)
  for (int i = 0; i < M; i += 1)
#pragma clang loop id(j)
    for (int j = 0; j < N; j += 1)
#pragma clang loop id(k)
      for (int k = 0; k < K; k += 1)
        C[i][j] += A[i][k] * B[k][j];
}

int main() {
  double C[128][256];
  double A[128][512];
  double B[512][256];
  memset(C, 0, sizeof(C));
  memset(A, 0, sizeof(A));
  memset(B, 0, sizeof(B));
  A[1][2] = 2;
  A[1][3] = 3;
  B[2][4] = 5;
  B[3][4] = 7;
  matmul(128,256,512,C,A,B);
  return 0;
}
