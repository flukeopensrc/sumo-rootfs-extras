# Blackhawk Update Instructions
This file contains instructions on how to updated the kernel on Blackhawk
to support the new version of the display.  Frank Hess 2021-03-17

## External Files
You will need the following files from outside of this repo to complete our task.
If you don't have some of these files, see the appendices on how to
obtain them.  The instructions will assume the following files have been
copied or linked into the "files" subdirectory:
```
fluke_dist.tar
blackhawk_initramfs.cpio.gz
```

## Prepare repos from Blackhawk open source cd
```
cd fluke_dist
tar -xf ../files/fluke_dist.tar
```

Enter the nios2-linux subdirectory and locally checkout the git repos
```
cd nios2-linux/
./checkout
```

## Kernel Build

Prepare the initramfs image.  You can ignore any 
"Cannot mknod: Operation not permitted" errors, the device nodes will be
automatically recreated by the kernel build based on the contents of the
file `uClinux-dist/vendors/Altera/nios2/romfs_list`
```
mkdir romfs
(cd romfs && zcat ../../../files/blackhawk_initramfs.cpio.gz | cpio -idv --no-absolute-filenames)

```

Prepare a Linux build directory:
```
mkdir kernel_build
cp ../../files/blackhawk_kernel_config kernel_build/.config
```

Patch the kernel:
```
cd linux-2.6
git am ../../../files/kernel_patches/*

```

Build the kernel:
```
source ../../../files/blackhawk_linux_env.sh

# select "ddr2_sdram" as the "device to execute kernel from"
make hwselect SYSPTF=../../fluke_HW/Blackhawk_OG_soc.linux.ptf

make oldconfig
make

# install kernel modules into our initramfs directory before making zImage
make modules_install

make zImage

```
## Appendix: External Files
### fluke_dist.tar
The `fluke_dist.tar` file comes from the "Open Source Software and Licenses
5730A" CD version 1.01.
### blackhawk_initramfs.cpio.gz
