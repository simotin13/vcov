#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <iostream>

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage vcov <vmm_major_number> <target_module>\n");
        return 1;
    }

    std::string cmd = "./create_dev_file " + std::string(argv[1]);
    printf("%s\n", cmd.c_str());
    int ret = system(cmd.c_str());
    if (ret != 0) {
        fprintf(stderr, "create dev file failed\n");
        return 1;
    }

    int fd = open("./vmm0", O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "./vmm0 open failed!\n");
    }

    close(fd);

    cmd = "sudo rm vmm0";
    system(cmd.c_str());
   return 0;
}