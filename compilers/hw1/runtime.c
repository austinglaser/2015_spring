#include <stdio.h>
#include <stdlib.h>

#include "runtime.h"

void print_int_nl(int x)
{
  printf("%d\n", x);
}

int input()
{
  int i;
  scanf("%d", &i);
  return i;
}

void nameExcept(void)
{
    fprintf(stderr, "NameError: variable is not defined");
    exit(1);
}
