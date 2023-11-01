#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <iostream>
#include <string>

#include "cpu.h"
#include "common.h"

#include "../kernelmodule/kernelmod.h"

#define VMM_DEV_FILE_PATH           "/dev/vmm"
#define INTEL_CPU_ID                "GenuineIntel"
int main(int argc, char **argv)
{
    int fd = open(VMM_DEV_FILE_PATH, O_RDWR);
    if (fd < 0)
    {
        std::cerr << StringHelper::strprintf("%s open failed!\n", VMM_DEV_FILE_PATH) << std::endl;
        exit(1);
    }

    char buf[16];
    memset(buf, 0, 16);
    _cpuid_id(buf);
    std::string cpuid(buf);
    std::cout << "cpuid:" << cpuid << std::endl;
    if (cpuid != INTEL_CPU_ID)
    {
        std::cerr << StringHelper::strprintf("cpu is not intel, cpuid:[%s]", cpuid) << std::endl;
        close(fd);
        exit(1);
    }

    int ret = _is_support_vmx();
    if (ret == 0)
    {
        std::cerr << StringHelper::strprintf("VT-x not supported, ret:[%d]", ret) << std::endl;
        close(fd);
        exit(1);
    }
    std::cout << StringHelper::strprintf("VT-x is supported, ret:[%d]", ret) << std::endl;

    ret = ioctl(fd, VMM_ENABLE_VMXE);
    std::cout << StringHelper::strprintf("ioctl VMM_ENABLE_VMXE ret:[%d]", ret) << std::endl;

    close(fd);
    exit(0);
}
