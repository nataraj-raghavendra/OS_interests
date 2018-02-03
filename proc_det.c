#include <stdio.h>
#include <unistd.h>
void main(){
  printf("PID = %d\nPPID = %d\n",getpid(),getppid());
}
