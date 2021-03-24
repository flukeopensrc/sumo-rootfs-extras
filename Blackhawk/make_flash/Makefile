SHELL=/bin/bash

BOOTLOADER_SREC=$(SOPC_KIT_NIOS2)/components/altera_nios2/boot_loader_cfi.srec
ZIMAGE=../fluke_dist/nios2-linux/kernel_build/arch/nios2/boot/zImage

blackhawk_mtd0.bin: mergedOS.bin
	srec_cat mergedOS.bin -Binary \
		-Fill 0xff 0x0 0x600000 \
		-Output blackhawk_mtd0.bin -Binary

mergedOS.bin: blackhawk_mtd0_preamble.bin Blackhawk_OG_vmlinux_nios.srec
	srec_cat blackhawk_mtd0_preamble.bin -Binary \
		Blackhawk_OG_vmlinux_nios.srec -Motorola -Offset 0xe0000 \
		-Output mergedOS.bin -Binary

Blackhawk_OG_vmlinux_nios.srec: $(ZIMAGE) $(BOOTLOADER_SREC)
	elf2flash --base=0x20260000 --end=0x21ffffff \
		--reset=0x20260000 --input=$(ZIMAGE) --output=Blackhawk_OG_vmlinux_nios.srec \
		--boot=$(BOOTLOADER_SREC)

blackhawk_mtd0_preamble.bin: ../files/blackhawk_old_mtd0.bin
	dd if=../files/blackhawk_old_mtd0.bin of=blackhawk_mtd0_preamble.bin bs=$$((0xe0000)) count=1

.SILENT: $(BOOTLOADER_SREC)
$(BOOTLOADER_SREC):
	test -e $(BOOTLOADER_SREC) || echo "boot_loader_cfi.srec not found, you should be running make from a Quartus embedded_command_shell.sh"
	
.PHONY: clean
clean:
	$(RM) Blackhawk_OG_vmlinux_nios.srec blackhawk_mtd0.bin blackhawk_mtd0_preamble.bin
