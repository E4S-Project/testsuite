#include "rect.h"
#include <cstdio>

int main() {
    Rect r=create_screen();
    printf("%d %d\n", r.x, r.y);
    return 0;
}
