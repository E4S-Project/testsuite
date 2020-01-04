#include <iostream>
#include <cstdlib>

#define DEFAULT_M 32
#define DEFAULT_N 32

template <typename T, unsigned int lines, unsigned int cols >
[[clang::jit]] T** allocateMatrix( ){
  T** ptr = new T*[lines];
  for( auto i = 0 ; i < lines ; i++ ) {
    ptr[i] = new T[cols];
  }
  return ptr;
}

template <typename T, unsigned int lines>
[[clang::jit]] void freeMatrix( T** ptr ){
  for( auto i = 0 ; i < lines ; i++ ) {
    delete [] ptr[i];
  }
  delete[] ptr;
}

template <typename T, unsigned int lines, unsigned int cols >
[[clang::jit]] void initMatrix(T** ptr, T value ){
  for( auto i = 0 ; i < lines ; i++ ) {
    for( auto j = 0 ; j < cols ; j++ ) {
      ptr[i][j] = value;
    }
  }
}

template <typename T, unsigned int lines, unsigned int cols >
[[clang::jit]] void printMatrix( T** ptr ){
  for( auto i = 0 ; i < lines ; i++ ) {
    for( auto j = 0 ; j < cols ; j++ ) {
      std::cout << ptr[i][j] << " ";
    }
    std::cout << std::endl;
  }
}


int main( int argc, char** argv ) {
  
  int M, N;

  if( argc < 3 ){
    std::cout << "Missing argument" << std::endl;
    M = DEFAULT_M;
    N = DEFAULT_N;
  } else {
    M = atoi( argv[ 1 ] );
    N = atoi( argv[ 2 ] );
  }

  auto p = allocateMatrix<double, M, N> ();

  initMatrix<double, M, N>( p, 5 );
  printMatrix<double, M, N>( p );

  freeMatrix<double, M>(  p );
  return EXIT_SUCCESS;
}
