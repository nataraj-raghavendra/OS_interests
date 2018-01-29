#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
void main(int argc,char *argv[]){
  int uid = atoi(argv[1]);
  int euid = atoi(argv[2]);
  if(setuid(uid)){
    perror("Error User ID");
   }
  if(seteuid(euid)){
    perror(" Error Effective User ID");
   }
    printf("UID = %d,EUID = %d\n",getuid(),geteuid());
}
