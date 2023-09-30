#!/bin/bash
export PATH=/home/meig/aarch64-himix200-linux/bin:$PATH
make clean
#make CROSS_COMPILE=arm-hisiv500-linux-
make CROSS_COMPILE=aarch64-himix200-linux-

