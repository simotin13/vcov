#include <iostream>

#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage vcov <target_module>");
        return 1;
    }

    std::string cmd = "./create_dev_file " + argv[1]
    int ret = system(cmd.c_str());
    if (ret != 0) {
        fprintf(stderr, "create dev file failed\n");
        return 1;
    }
    return 0;
}