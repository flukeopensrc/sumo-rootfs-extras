#!/bin/bash
export BLACKHAWK_TOP=$(realpath $(dirname $(realpath ${BASH_SOURCE[0]}))/..)
export ARCH=nios2
export CROSS_COMPILE=nios2-linux-gnu-
export KBUILD_OUTPUT=$BLACKHAWK_TOP/fluke_dist/nios2-linux/kernel_build
export INSTALL_MOD_PATH=$BLACKHAWK_TOP/fluke_dist/nios2-linux/romfs
export PATH=$PATH:$BLACKHAWK_TOP/fluke_dist/nios2-linux/toolchain-mmu/x86-linux2/bin
#export PATH=$PATH:$BLACKHAWK_TOP/fluke_dist/nios2-linux/sourceryg++-2013.05/bin

for MYVAR in BLACKHAWK_TOP ARCH CROSS_COMPILE KBUILD_OUTPUT INSTALL_MOD_PATH PATH
do
	echo $MYVAR=$(printenv $MYVAR)
done
