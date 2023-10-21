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
echo "vmm driver major number ${MAJOR_NUM}"
if [ ${MAJOR_NUM} = "" ]; then
  echo "vmm not found"
  exit 1
fi

cd ..
sudo rm -f /dev/vmm
sudo mknod -m 666 /dev/vmm c ${MAJOR_NUM} 1
