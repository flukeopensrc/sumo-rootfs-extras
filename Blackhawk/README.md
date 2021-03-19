# Blackhawk Update Instructions
This file contains instructions on how to updated the kernel on Blackhawk
to support the new hardware version of the display.  The main issue was
the kernel and other code originally used on Blackhawk needs a
gcc 3 (cross) compiler to build.  However, gcc 3 itself doesn't build on our
current workstations.  So I ported the kernel to build with gcc 4, which
is available as a binary cross compiler from our 
"Open Source Software and Licenses 5730A" CD, and also (a slightly newer
version) from 
https://sourcery.mentor.com/GNUToolchain/release2499

Frank Hess 2021-03-17

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
We extracted the initramfs from partition0 of the old flash image using 
`binwalk` and `dd`
as follows:
```
$ # examine partition to find beginning of gzipped kernel data
$ binwalk -B blackhawk_old_mtd0.bin

DECIMAL       HEXADECIMAL     DESCRIPTION
--------------------------------------------------------------------------------
928538        0xE2B1A         gzip compressed data, maximum compression, from Unix, last modified: 2013-11-13 19:06:08

$ # use dd to copy out the gzipped file
$ dd if=blackhawk_old_mtd0.bin bs=928538 skip=1 of=mykernel.gz
$ gunzip mykernel.gz 

gzip: mykernel.gz: decompression OK, trailing garbage ignored

$ binwalk -B mykernel 

DECIMAL       HEXADECIMAL     DESCRIPTION
--------------------------------------------------------------------------------
2404732       0x24B17C        Linux kernel version 2.6.30
2443640       0x254978        CRC32 polynomial table, little endian
2494576       0x261070        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/init/main.c
# ...snipped out a bun of uninteresting output...
2654128       0x287FB0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/lib/klist.c
3058568       0x2EAB88        gzip compressed data, maximum compression, from Unix, last modified: 2013-11-13 19:06:01

$ # extract gzipped initramfs cpio archive from end of kernel
$ dd if=mykernel bs=3058568 skip=1 of=blackhawk_initramfs.cpio.gz
0+1 records in
0+1 records out
1132332 bytes (1.1 MB, 1.1 MiB) copied, 0.00541767 s, 209 MB/s

```
