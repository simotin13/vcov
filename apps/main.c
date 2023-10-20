#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define VMM_DEV_FILE_PATH           "/dev/vmm"
int main(int argc, char **argv)
{
    int fd = open("/dev/vmm", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "%s open failed!\n", VMM_DEV_FILE_PATH);
        return -1;
    }

    close(fd);
    return 0;
}