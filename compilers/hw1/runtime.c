#include <stdio.h>

#include "runtime.h"

void print_int_nl(int x) {
  printf("%d\n", x);
}

int input() {
  int i;
  scanf("%d", &i);
  return i;
}
