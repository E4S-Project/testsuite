#include <stdio.h>
#include <stdlib.h>

int main(){

#pragma omp parallel
  {
    printf( "Hello World\n" );
  }
  return EXIT_SUCCESS;
}
