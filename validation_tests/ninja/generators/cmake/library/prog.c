#include <stdlib.h>
#include <stdio.h>
#include "libadd.h"

int main(){
  int a1, a2, res;
  a1 = 32;
  a2 = 128;
  res = myaddition( a1, a2 );
  printf( "%d + %d = %d\n", a1, a2, res );
  return EXIT_SUCCESS;
}
