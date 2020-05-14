void foo( double* A, double* B, int M ) {
  
  int i;
  for( i = 0 ; i < M ; i++ ) {
    A[i] = 2*B[i];
  }
}
