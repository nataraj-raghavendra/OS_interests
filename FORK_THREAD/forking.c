#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int un_init;
int init = 5;
int *int_heap;
void incr_function(){
  init+=1;
  un_init+=1;
  *int_heap +=1;
}
void print(int incr){
  int local = 5;
  local+=incr;
  printf("Process : %d => init = %d,un_init=%d,local=%d,int_heap=%d\n",getpid(),init,un_init,local,*int_heap);
}
void main(){
  un_init = 5;
  int_heap = (int*)malloc(sizeof(int));
  *int_heap = 5;
   if(fork()==0){
     incr_function();
     print(1);
   } else {
     sleep(1);
     print(2);
   }
}
