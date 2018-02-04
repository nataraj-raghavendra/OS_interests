#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
void main() {
  pid_t pid = fork();
  if(pid < 0){
    perror("failed fork");
  }
  if(pid==0){
    printf("Child:\n\tPID = %d\n\tPPID = %d\n\tPGID=%d\n",getpid(),getppid(),getpgrp());
  }else {
    printf("Parent:\n\tPID = %d\n\tPPID = %d\n\tPGID=%d\n",getpid(),getppid(),getpgrp());
  }
  wait(NULL);
}
