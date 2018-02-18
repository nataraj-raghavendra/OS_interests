#include <stdio.h>
#include <stdlib.h>
int un_init;
static int un_init_stk;
int init = 20;
static int init_stk = 30;
const int constant = 30;
int *gl_heap;
void main(){
  int local;
  int local_init = 20;
  int static_local;
  int *lcl_heap = (int*)malloc(1);
  gl_heap = (int*)malloc(1);
}
