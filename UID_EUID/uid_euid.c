#include <stdio.h>
#include <unistd.h>

void main(){
  printf("UID = %d,EUID = %d\n",getuid(),geteuid());
}
