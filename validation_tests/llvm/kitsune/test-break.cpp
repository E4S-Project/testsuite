#include <cstdio>
int main() {
  forall(int i = 0; i < 5; ++i) {
    int j = i;
    if (j == 2) {
      printf("continue: %d\n", j);
      continue;
    } else if (j == 3) {
      printf("break: %d\n", j);
      break;
    } else {
      printf("ordinary loop iteration: %d\n", j);
    }
    printf("loop end: %d\n", j);
  }
  return 0;
}


