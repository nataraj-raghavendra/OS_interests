#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
void main() {
  pid_t pid = fork();
  pid_t pid1;
  if(pid < 0){
    perror("failed fork");
  }
  if(pid==0){
    printf("Createing new session Child1. Return value = %d\n",setsid());
    usleep(100); 
    printf("Child1:\n\tPID = %d\n\tPPID = %d\n\tPGID = %d\n\tSID = %d\n",getpid(),getppid(),getpgid(0),getsid(getpid()));
          getchar();
  }else {
    pid1 = fork();
    if(pid1==0){
      usleep(500);
      printf("Child2:\n\tPID = %d\n\tPPID = %d\n\tPGID = %d\n\tSID = %d\n",getpid(),getppid(),getpgid(0),getsid(getpid()));
      getchar();
    }else {
      usleep(900);
      printf("Createing new session parent. Return value = %d\n",setsid());
      printf("Parent:\n\tPID = %d\n\tPPID = %d\n\tPGID = %d\n\tSID = %d\n",getpid(),getppid(),getpgid(0),getsid(getpid()));
      getchar();
    }
    waitpid(pid,NULL,0);
    waitpid(pid1,NULL,0);
  }
}
