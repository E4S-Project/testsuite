
// code from CHiLL manual reverse example

void mm(float **A, float **B, float **C, int ambn, int an, int bm) {
   int i, j, n;
   for(i = 0; i < an; i++) {
      for(j = 0; j < bm; j++) {
         C[i][j] = 0.0f;
         for(n = 0; n < ambn; n++) {
            C[i][j] += A[i][n] * B[n][j];
         }
      }
   }
}
