#include <stdio.h>
#include <string.h>
#include "cpu.h"

int main(int argc, char **argv)
{
    char buf[16];
    memset(buf, 0, 16);
    _cpuid_id(buf);
    printf("%s\n", buf);
    int vmx = _is_support_vmx();
    printf("%d\n", vmx);
    return 0;
}
