#include<kitsune.h>

/* This is a modified example from the Kitsune test suite.
 * The errors are fixed so the compiler is supposed to accept it.
 */

void f1() {
  int n;

  forall (n = 0; n < 10; n++);

}

