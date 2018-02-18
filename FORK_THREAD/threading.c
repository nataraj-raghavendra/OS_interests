#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <pthread.h>
int un_init;
int init = 5;
int *int_heap;
void print(int incr){
  int local = 5;
  local+=incr;
  printf("Process : %d => init = %d,un_init=%d,local=%d,int_heap=%d\n",getpid(),init,un_init,local,*int_heap);
}
void *incr_function(void *args){
  init+=1;
  un_init+=1;
  *int_heap +=1;
  print(1);
}
void main(){
  pthread_t thread;
  un_init = 5;
  int_heap = (int*)malloc(sizeof(int));
  *int_heap = 5;
  pthread_create(&thread,NULL,incr_function,NULL);
  sleep(1);
  print(2);
}
