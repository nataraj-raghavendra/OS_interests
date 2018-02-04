#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
void main() {
  pid_t pid = fork();
  if(pid < 0){
    perror("failed fork");
  }
  if(pid==0){
    sleep(1);
    printf("Child:\n\tPID = %d\n\tPPID = %d\n",getpid(),getppid());
  }else {
    printf("Parent:\n\tPID = %d\n\tPPID = %d,%d\n",getpid(),getppid(),pid);
  }
}
