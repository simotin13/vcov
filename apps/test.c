#include <stdio.h>
#include "cpu.h"

int main(int argc, char **argv)
{
    char buf[8];
    _cpuid_id(buf);
    printf("%s\n", buf);
    return 0;
}
