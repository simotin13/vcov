#include <stdio.h>
#include <string.h>
#include "cpu.h"
#include "../kernelmodule/kernelmod.h"
int main(int argc, char **argv)
{
    char buf[16];
    memset(buf, 0, 16);
    _cpuid_id(buf);
    printf("%s\n", buf);

    return 0;
}
