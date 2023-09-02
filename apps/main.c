#include <stdio.h>

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf("Usage vcov <target_module>");
        return -1;
    }
    open("/dev");
}