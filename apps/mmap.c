#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define DEVNAME	"/home/miyazaki/vcov/kernelmodule/vmm"
int main(int argc, char **argv)
{
    int *address;
    int fd;
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
    fprintf(stdout, "success!\n");
    close(fd);
    return 0;
}
