#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
int main(int argc, char *argv[]) {
	int fd = open(argv[1],O_CREAT | O_TRUNC | O_RDWR, S_IRWXU | S_IRWXG);
	if(fd<0){
	    printf("%s\n",strerror(errno));
	}
	uint8_t *data = (uint8_t*)malloc(sizeof(uint8_t) * 2048 * 400);
	if(data == NULL)
	    return -1;
	memset(data,atoi(argv[2]),2048*400);
	printf("%d\n",data[0]);
	uint64_t n = write(fd, data, 2048 * 400);
	printf("%ld\n",n);
	if(n!= 2048 * 400)
	    return -1;
	free(data);
	close(fd);
	return 0;
}
