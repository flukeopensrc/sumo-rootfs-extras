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
2497888       0x261D60        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/arch/nios2/kernel/dma-mapping.c
2498400       0x261F60        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/arch/nios2/mm/memory.c
2498828       0x26210C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/sched_rt.c
2499088       0x262210        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/sched_fair.c
2501552       0x262BB0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/fork.c
2502220       0x262E4C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/printk.c
2502604       0x262FCC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/linux/ptrace.h
2503152       0x2631F0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/softirq.c
2503596       0x2633AC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/resource.c
2504060       0x26357C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/capability.c
2504484       0x263724        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/signal.c
2504740       0x263824        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/sys.c
2504964       0x263904        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/kmod.c
2505152       0x2639C0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/workqueue.c
2505424       0x263AD0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/rcupdate.c
2505948       0x263CDC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/params.c
2506260       0x263E14        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/kfifo.c
2506480       0x263EF0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/hrtimer.c
2507048       0x264128        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/cred.c
2507324       0x26423C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/async.c
2508104       0x264548        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/futex.c
2508248       0x2645D8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/rtmutex_common.h
2508396       0x26466C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/rtmutex.c
2508820       0x264814        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/module.c
2510332       0x264DFC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/softlockup.c
2511048       0x2650C8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/irq/manage.c
2511980       0x26546C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/irq/chip.c
2512540       0x26569C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/kernel/irq/devres.c
2512852       0x2657D4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/bootmem.c
2513284       0x265984        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/filemap.c
2514296       0x265D78        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/page_alloc.c
2515244       0x26612C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/linux/gfp.h
2515488       0x266220        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/page-writeback.c
2515704       0x2662F8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/readahead.c
2515960       0x2663F8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/vmscan.c
2516124       0x26649C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/shmem.c
2517452       0x2669CC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/page_isolation.c
2518280       0x266D08        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/mm/dmapool.c
2519976       0x2673A8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/linux/dcache.h
2520108       0x26742C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/open.c
2520260       0x2674C4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/file_table.c
2520800       0x2676E0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/exec.c
2521008       0x2677B0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/pipe.c
2521348       0x267904        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/dcache.c
2521640       0x267A28        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/inode.c
2521780       0x267AB4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/attr.c
2521988       0x267B84        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/filesystems.c
2522108       0x267BFC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/namespace.c
2522788       0x267EA4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/linux/highmem.h
2522944       0x267F40        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/libfs.c
2523100       0x267FDC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/fs-writeback.c
2523608       0x2681D8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/buffer.c
2524044       0x26838C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/bio.c
2524328       0x2684A8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/block_dev.c
2524656       0x2685F0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/direct-io.c
2524948       0x268714        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/aio.c
2525120       0x2687C0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/locks.c
2527156       0x268FB4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/proc/generic.c
2528208       0x2693D0        Unix path: /dev/vc/0
2529824       0x269A20        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/sysfs/inode.c
2530032       0x269AF0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/sysfs/file.c
2530204       0x269B9C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/sysfs/dir.c
2530344       0x269C28        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/sysfs/symlink.c
2530576       0x269D10        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/sysfs/bin.c
2530916       0x269E64        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/devpts/inode.c
2531060       0x269EF4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/fat/cache.c
2531508       0x26A0B4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/fat/dir.c
2532596       0x26A4F4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/fat/inode.c
2534332       0x26ABBC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/fat/namei_vfat.c
2534808       0x26AD98        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/nfs/file.c
2536560       0x26B470        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/nfs/direct.c
2536812       0x26B56C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/nfs/proc.c
2537288       0x26B748        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/lockd/clntproc.c
2537796       0x26B944        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/lockd/host.c
2538268       0x26BB1C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/lockd/svc.c
2538616       0x26BC78        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/lockd/svcsubs.c
2539728       0x26C0D0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/file.c
2539880       0x26C168        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/nodelist.c
2540880       0x26C550        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/nodemgmt.c
2541896       0x26C948        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/readinode.c
2544788       0x26D494        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/write.c
2546076       0x26D99C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/scan.c
2547684       0x26DFE4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/gc.c
2550704       0x26EBB0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/erase.c
2550852       0x26EC44        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/background.c
2551388       0x26EE5C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/super.c
2551988       0x26F0B4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/debug.c
2552304       0x26F1F0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/fs/jffs2/wbuf.c
2554176       0x26F940        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/block/elevator.c
2554664       0x26FB28        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/block/blk-core.c
2555228       0x26FD5C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/block/blk-tag.c
2555596       0x26FECC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/block/blk-sysfs.c
2555760       0x26FF70        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/block/blk-barrier.c
2555912       0x270008        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/block/blk-settings.c
2556232       0x270148        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/linux/scatterlist.h
2556752       0x270350        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/block/genhd.c
2557120       0x2704C0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/block/noop-iosched.c
2557704       0x270708        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/lib/idr.c
2558048       0x270860        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/lib/kobject.c
2558592       0x270A80        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/lib/kref.c
2559120       0x270C90        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/lib/string.c
2559392       0x270DA0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/lib/bitmap.c
2563008       0x271BC0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/gpib/eastwood/eastwood_gpib.c
2565172       0x272434        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/char/random.c
2565536       0x2725A0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/char/tty_io.c
2566728       0x272A48        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/char/n_tty.c
2567060       0x272B94        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/char/tty_ldisc.c
2567288       0x272C78        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/char/tty_buffer.c
2567508       0x272D54        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/char/pty.c
2568316       0x27307C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/char/vt.c
2569084       0x27337C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/serial/serial_core.c
2569796       0x273644        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/serial/8250.c
2570292       0x273834        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/base/core.c
2570588       0x27395C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/base/sys.c
2570960       0x273AD0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/base/bus.c
2571264       0x273C00        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/base/dd.c
2571628       0x273D6C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/base/class.c
2572004       0x273EE4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/base/devres.c
2572256       0x273FE0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/base/transport_class.c
2573136       0x274350        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/scsi/scsi.c
2573392       0x274450        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/scsi/hosts.c
2574576       0x2748F0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/scsi/scsi_error.c
2574944       0x274A60        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/scsi/scsi_lib.c
2576108       0x274EEC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/scsi/scsi_scan.c
2580720       0x2760F0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/scsi/sd.c
2584244       0x276EB4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/scsi/sg.c
2585924       0x277544        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/net/phy/mdio_bus.c
2587260       0x277A7C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/mtd/mtdcore.c
2587720       0x277C48        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/mtd/mtdpart.c
2588384       0x277EE0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/mtd/mtd_blkdevs.c
2588900       0x2780E4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/linux/mtd/cfi.h
2590720       0x278800        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/mtd/chips/cfi_cmdset_0001.c
2591304       0x278A48        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/mtd/chips/gen_probe.c
2591672       0x278BB8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/mtd/maps/physmap.c
2592140       0x278D8C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/linux/usb.h
2594408       0x279668        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/usb/core/hub.c
2602564       0x27B644        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/usb/host/sls-hcd.c
2609356       0x27D0CC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/usb/gadget/sls_usb.c
2611992       0x27DB18        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/input/input.c
2615580       0x27E91C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/rtc/hctosys.c
2615836       0x27EA1C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/rtc/class.c
2616056       0x27EAF8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/rtc/rtc-dev.c
2616800       0x27EDE0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/i2c/i2c-core.c
2617988       0x27F284        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/i2c/i2c-dev.c
2618464       0x27F460        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/hid/hid-core.c
2620260       0x27FB64        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/hid/hid-input.c
2628500       0x281B94        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/hid/usbhid/hid-core.c
2629952       0x282140        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/drivers/hid/usbhid/hid-quirks.c
2630524       0x28237C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/socket.c
2633132       0x282DAC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/core/request_sock.c
2633740       0x28300C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/core/datagram.c
2633968       0x2830F0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/core/gen_estimator.c
2634184       0x2831C8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/core/net_namespace.c
2636096       0x283940        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/core/ethtool.c
2636356       0x283A44        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/core/neighbour.c
2637056       0x283D00        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/core/rtnetlink.c
2637500       0x283EBC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/core/filter.c
2638356       0x284214        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sched/sch_generic.c
2638572       0x2842EC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/netlink/af_netlink.c
2638968       0x284478        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/netlink/genetlink.c
2640072       0x2848C8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/inetpeer.c
2640336       0x2849D0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/ip_fragment.c
2640476       0x284A5C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/ip_output.c
2640676       0x284B24        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/inet_hashtables.c
2641512       0x284E68        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/tcp_input.c
2642028       0x28506C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/tcp_output.c
2642288       0x285170        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/tcp_timer.c
2642740       0x285334        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/tcp_ipv4.c
2642876       0x2853BC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/net/tcp.h
2643172       0x2854E4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/tcp_cong.c
2643716       0x285704        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/include/net/udp.h
2644704       0x285AE0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/icmp.c
2644840       0x285B68        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/devinet.c
2645076       0x285C54        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/af_inet.c
2645612       0x285E6C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/igmp.c
2646076       0x28603C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/fib_semantics.c
2646456       0x2861B8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/ipv4/fib_hash.c
2648868       0x286B24        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/unix/af_unix.c
2649180       0x286C5C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/unix/garbage.c
2649612       0x286E0C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sunrpc/clnt.c
2650524       0x28719C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sunrpc/xprtsock.c
2650824       0x2872C8        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sunrpc/sched.c
2651284       0x287494        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sunrpc/svc.c
2651676       0x28761C        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sunrpc/svcsock.c
2652348       0x2878BC        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sunrpc/svcauth_unix.c
2652932       0x287B04        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sunrpc/rpc_pipe.c
2653380       0x287CC4        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/net/sunrpc/svc_xprt.c
2654128       0x287FB0        Unix path: /home/halfdome/niosDev/kernel/nios2-linux-20100621/nios2-linux/linux-2.6/lib/klist.c
3058568       0x2EAB88        gzip compressed data, maximum compression, from Unix, last modified: 2013-11-13 19:06:01

$ # extract gzipped initramfs cpio archive from end of kernel
$ dd if=mykernel bs=3058568 skip=1 of=blackhawk_initramfs.cpio.gz
0+1 records in
0+1 records out
1132332 bytes (1.1 MB, 1.1 MiB) copied, 0.00541767 s, 209 MB/s



```

