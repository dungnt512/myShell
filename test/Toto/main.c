#include <stdio.h>

extern int x, y;
extern void toto();

int main(int argc, char *argv[]){
  toto();
  printf("KQ: %d\n",x * y);
  return 0;
}

// gcc -c m1.c
// gcc -c m2.c
// gcc -c main.c
// gcc m1.c m2.c main.c