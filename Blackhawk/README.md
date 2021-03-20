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
blackhawk_old_mtd0.bin
blackhawk_old_mtd1.bin
blackhawk_old_mtd2.bin
blackhawk_old_mtd3.bin
blackhawk_old_mtd4.bin
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

The zImage may be test run on the Blackhawk board without reprogramming the 
flash by using a usb-blaster and running `nios2-download -g path/to/zImage` 
in an embedded_command_shell.sh.  If you are using a usb-blasterII you will
need to turn down the clock frequency first with 
`jtagconfig --setparam 1 JtagClock 6M` .


## Create flash image
Enter the `make_flash` subdirectory of the top level Blackhawk directory.
Inside an `embedded_command_shell.sh` type `make`.  You must have already 
built the zImage as described
above, and you will need various external files to be present in the
`files` subdirectory as mentioned earlier.

You should now have a `blackhawk_mtd0.bin` which can be copied to the
/dev/mtd0 partition of the Blackhawk board.

## Appendix: External Files
### fluke_dist.tar
The `fluke_dist.tar` file comes from the "Open Source Software and Licenses
5730A" CD version 1.01.

### blackhawk_old_mtd[0-4].bin
These are images of the original mtd partitions.  They were taken from
a live Blackhawk A20 board by copying the device files like /dev/mtdblock0
to a usb thumb drive.  The jffs2 file system was unmounted prior to
copying /dev/mtdblock2 to insure it was in a consistent state.

Alternatively, someone must have the full flash image used to manufacture
the original boards, in which a case pristine image of the jffs2
filesystem partition
could be extracted from that.  The partition map is as follows
(note the partitions appear in a different order than they are
numbered):
```
Creating 5 MTD partitions on "physmap-flash.0":
0x000000180000-0x000000780000 : "FLUKE-A9 Compressed Kernel Image"
0x000000000000-0x000000180000 : "FLUKE-A9 FPGA configuration"
0x000000780000-0x000000ec0000 : "FLUKE-A9 romfs/jffs2"
0x000000ec0000-0x000000f60000 : "FLUKE-A9 unmanaged sectors for splash screen (1)."
0x000000f60000-0x000001000000 : "FLUKE-A9 unmanaged sectors for splash screen (2)."

```

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
# ...snipped out a bunch of uninteresting output...
2654128       0x287FB0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/lib/klist.c
3058568       0x2EAB88        gzip compressed data, maximum compression, from Unix, last modified: 2013-11-13 19:06:01

$ # extract gzipped initramfs cpio archive from end of kernel
$ dd if=mykernel bs=3058568 skip=1 of=blackhawk_initramfs.cpio.gz
0+1 records in
0+1 records out
1132332 bytes (1.1 MB, 1.1 MiB) copied, 0.00541767 s, 209 MB/s

```
