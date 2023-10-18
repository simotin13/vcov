#!/bin/bash
cd kernelmodule
make clean;
make
sudo rmmod vmm
sudo insmod vmm.ko
MAJOR_NUM=$(cat /proc/devices | grep vmm | awk '{print $1}')
echo ${MAJOR_NUM}

