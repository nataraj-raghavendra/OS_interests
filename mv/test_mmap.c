#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
int main(int argc, char *argv[]) {
	int fdr = open(argv[1],O_RDONLY);
	if(fdr<0){
	    printf("%s\n",strerror(errno));
	}
	
	uint8_t *data =  (uint8_t*)mmap(NULL, 2048 * 400, MAP_SHARED, PROT_READ,
                  fdr, 0);
	while(1){
	    uint64_t index = rand()%(2048 * 400);
	    printf("num[%lld] = %d\n",index ,data[index]);
	    usleep(10000);
	}
	return 0;
}
