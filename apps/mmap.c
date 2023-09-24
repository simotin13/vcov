#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define DEVNAME	"/dev/vmm"
int main(int argc, char **argv)
{
    char buf[128];
    char *address;
    int fd;
    int ret;
    fd = open(DEVNAME, O_RDWR);
    if (fd<=0) {
        perror(DEVNAME);
        exit(1);
    }

    address = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (address == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }
    printf("address:[%p]\n", address);
    printf("[%s]\n", address);

    ret = read(fd, buf, 32);
    fprintf(stdout, "read result:[%d], buf:[%s]\n", ret, buf);

    strcpy(address, "write test");

    ret = read(fd, buf, 32);
    fprintf(stdout, "read result:[%d], buf:[%s]\n", ret, buf);

    fprintf(stdout, "finish...\n");
    close(fd);
    return 0;
}
