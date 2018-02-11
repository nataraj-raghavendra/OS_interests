#include <stdio.h>
#include <signal.h>
#include <unistd.h>
void signal_fn_parent(int sig){
    printf("Received Signal to main process %d\n",sig);
}

void signal_fn_child(int sig){
    printf("Received Signal to child process %d\n",sig);
}

void main(){
  pid_t pid = fork();
  if(pid==0){
    usleep(500);
    signal(SIGINT,signal_fn_child);
    printf("Child:PID=>%d\nSID=>%d\nPGRP=>%d\nTC_PGRP=>%d\n\n",getpid(),getsid(0),getpgrp(),tcgetpgrp(STDIN_FILENO));
  }else{
    printf("Parent:PID=>%d\nSID=>%d\nPGRP=>%d\nTC_PGRP=>%d\n\n",getpid(),getsid(0),getpgrp(),tcgetpgrp(STDIN_FILENO));
    setpgid(pid,pid);
    tcsetpgrp(STDIN_FILENO,pid);
    signal(SIGINT,signal_fn_parent);
    printf("Parent:PID=>%d\nSID=>%d\nPGRP=>%d\nTC_PGRP=>%d\n\n",getpid(),getsid(0),getpgrp(),tcgetpgrp(STDIN_FILENO));
  }
  sleep(5);
}
