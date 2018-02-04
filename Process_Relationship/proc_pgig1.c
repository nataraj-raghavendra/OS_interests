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
    usleep(100);
    printf("Creating a new group for child1\n");
    setpgid(getpid(),0);
    printf("Child1:\n\tPID = %d\n\tPPID = %d\n\tPGID = %d\n",getpid(),getppid(),getpgid(0));
  }else {
    pid1 = fork();
    if(pid1==0){
      usleep(300);
      printf("Adding child2 to newly created group\n");
      setpgid(getpid(),pid);
      printf("Child2:\n\tPID = %d\n\tPPID = %d\n\tPGID = %d\n",getpid(),getppid(),getpgid(0));
    }else {
      printf("Parent:\n\tPID = %d\n\tPPID = %d\n\tPGID = %d\n",getpid(),getppid(),getpgid(0));
    }
    waitpid(pid,NULL,0);
    waitpid(pid1,NULL,0);
  }
}
