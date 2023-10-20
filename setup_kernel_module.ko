#!/bin/bash
cd kernelmodule
make clean;
make
if [ $? -ne 0 ]; then
  echo "build vmm.ko failed"
  exit 1
fi

sudo rmmod vmm
sudo insmod vmm.ko
if [ $? -ne 0 ]; then
  echo "insmod vmm.ko failed"
  exit 1
fi

MAJOR_NUM=$(cat /proc/devices | grep vmm | awk '{print $1}')
if [ ${MAJOR_NUM} = "" ]; then
  echo "vmm not found"
  exit 1
fi

cd ..
sudo mknod -m 666 vmm c ${MAJOR_NUM} 2

