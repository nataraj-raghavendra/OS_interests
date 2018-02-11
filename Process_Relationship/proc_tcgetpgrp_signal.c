#include <stdio.h>
#include <signal.h>
#include <unistd.h>
void signal_fn(int sig){
  printf("Received Signal to main process %d\n",sig);
}

void main(){
  printf("PID=>%d\nSID=>%d\nPGRP=>%d\nTC_PGRP=>%d\n",getpid(),getsid(0),getpgrp(),tcgetpgrp(STDIN_FILENO));
  signal(SIGINT,signal_fn);
  sleep(5);
}
