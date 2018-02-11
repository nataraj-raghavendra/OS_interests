#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void main(){
    printf("PID=>%d\nSID=>%d\nPGRP=>%d\nTC_PGRP=>%d\n",getpid(),getsid(0),getpgrp(),tcgetpgrp(STDIN_FILENO));
}
