/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <exports.h>
#include <malloc.h>
#include "Tri_Clamp.h"

/*
 *  altfb.c -- Altera framebuffer driver
 *
 *  Based on vfb.c -- Virtual frame buffer device
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Hacked up beyond all recognition by John Witters for use by U-Boot.
 *
 */

#include <asm/io.h>
#define CONFIG_FB_DE1_SOC (0x00044000) /* Altera VIP frame buffer controller. */
#define ROTATE_FB_MACNICA (0x0004A700) /* Macnica rotator. */
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE (480*272*4) /* 5790B Frame Buffer Size */

typedef struct vidinfo {
        ushort  vl_col;         /* Number of columns (i.e. 160) */
        ushort  vl_row;         /* Number of rows (i.e. 100) */

        u_char  vl_bpix;        /* Bits per pixel, 0 = 1 */

        ushort  *cmap;          /* Pointer to the colormap */

        void    *priv;          /* Pointer to driver-specific data */
} vidinfo_t;

struct vidinfo panel_info;

/*
 *  Initialization - VIPFrameReader Based
 */

/*
 * Use CONFIG_FB_DE1_SOC for the frame buffer controller address.
 * The frame buffer controller can be located anywhere within the
 * FPGA's address space.  Most frame buffer controllers have a fixed
 * address, but the Altera frame buffer controller's address is not
 * fixed.
 *
 * With Linux kernels, this address is passed in through the device
 * tree.  Since U-Boot doesn't support device tree (yet,) the address
 * must be defined somewhere else.  I chose to define it within
 * ../../include/configs/socfpga_cyclone5.h.  However, it could be
 * argued that it should be defined elsewhere in a board specific file.
 *
 * The CONFIG_FB_DE1_SOC is relative to the base address of FPGA
 * peripherals.  For the ARM CPU this would be relative to H2F_LW_AXI,
 * (0xFF200000.)
 *
 * -John Witters
 */

#define PACKET_BANK_ADDRESSOFFSET 12
#define PB0_BASE_ADDRESSOFFSET 16
#define PB0_WORDS_ADDRESSOFFSET 20
#define PB0_SAMPLES_ADDRESSOFFSET 24
#define PB0_WIDTH_ADDRESSOFFSET 32
#define PB0_HEIGHT_ADDRESSOFFSET 36
#define PB0_INTERLACED_ADDRESSOFFSET 40

#define H2F_LW_AXI          0xFF200000

#define BITS_PER_PIXEL	(32)
#define BITS_PER_WORD	(128)
#define BITS_PER_BURST (4096*BITS_PER_PIXEL)
#define BYTES_PER_ROTATOR (1<<23) /* To align rotator on 8MB boundary. */

/* Rotator register offsests. */
#define ROTATOR_CTRL 0
#define ROTATOR_BUFFER 4
#define ROTATOR_FRAMES 8
#define ROTATOR_STATUS 12
#define ROTATOR_FRAME_0_DIMENSIONS 16
#define ROTATOR_FRAME_1_DIMENSIONS 20
#define ROTATOR_FRAME_2_DIMENSIONS 24
#define ROTATOR_FRAME_3_DIMENSIONS 28

/* Rotator control register commands */
#define ROTATOR_OFF 0
#define ROTATOR_ON 1
#define ROTATOR_0 0
#define ROTATOR_90 2
#define ROTATOR_180 4
#define ROTATOR_270 6

void lcd_ctrl_init(void *lcdbase)
{
    unsigned long base;
/*    base = (unsigned long)ioremap_nocache(H2F_LW_AXI, VIP_MIXER_EXTENT); */
    base = (unsigned long)(H2F_LW_AXI);

    printf("Turning off frame buffer controller.\n");
    writel(0, base + CONFIG_FB_DE1_SOC);

    printf("Writing frame buffer address %x\n", lcdbase);
    writel(lcdbase, base + CONFIG_FB_DE1_SOC + PB0_BASE_ADDRESSOFFSET);
    printf("Writing number of words in frame buffer.\n");
    writel(panel_info.vl_col * panel_info.vl_row/(BITS_PER_WORD/BITS_PER_PIXEL), \
            base + CONFIG_FB_DE1_SOC + PB0_WORDS_ADDRESSOFFSET);
    printf("Writing number of pixels in frame buffer.\n");
    writel(panel_info.vl_col * panel_info.vl_row, \
            base + CONFIG_FB_DE1_SOC + PB0_SAMPLES_ADDRESSOFFSET);
    printf("Writing number of columns in frame buffer.\n");
    writel(panel_info.vl_col, base + CONFIG_FB_DE1_SOC + PB0_WIDTH_ADDRESSOFFSET);
    printf("Writing number of rows in frame buffer.\n");
    writel(panel_info.vl_row, base + CONFIG_FB_DE1_SOC + PB0_HEIGHT_ADDRESSOFFSET);
    printf("Writing interlace in frame buffer.\n");
    writel(0, base + CONFIG_FB_DE1_SOC + PB0_INTERLACED_ADDRESSOFFSET);
    printf("Writing frame select of frame buffer.\n");
    writel(0, base + CONFIG_FB_DE1_SOC + PACKET_BANK_ADDRESSOFFSET);
    //Go
    printf("Starting frame buffer controller.\n");
    writel(1, base + CONFIG_FB_DE1_SOC);
    printf("Frame buffer should have started.\n");

    return 0;
}

int hello_world (int argc, char * const argv[])
{
	int i, row, col, color;
	void *lcdbase, *aligned_base;
	/*
	void *rotate_base, *rotate_aligned_base;
	*/
	unsigned long *base, *splash;

	/*
	   Allocate DDR2 SDRAM for the frame buffer controller.
	   The frame buffer must be aligned on the word boundary of the
	   frame reader's master port.
	*/
	lcdbase = malloc(((BITS_PER_BURST/8)-1) + CONFIG_SYS_VIDEO_LOGO_MAX_SIZE);

	/*
	   Allocate the DDR2 SDRAM for the Macnica rotator.
	   The rotator must be aligned on a 8MB boundary.

	rotate_base = malloc((BYTES_PER_ROTATOR-1) + CONFIG_SYS_VIDEO_LOGO_MAX_SIZE);
	*/
        if (lcdbase == 0 /* || rotate_base == 0 */ ) {
          printf("Unable to allocate framebuffer memory.\n");
	} else {
		printf("Base address %x\n", lcdbase);
		/*
		   Align the frame buffer pointer to the size of the frame
		   reader's master port.  The pointer arithmetic is needed
		   because neither aligned_alloc nor posix_memalign appear
		   to be available.
		*/
		aligned_base = (((uintptr_t)lcdbase + ((BITS_PER_BURST/8)-1))
		& ~ (uintptr_t)((BITS_PER_BURST/8)-1));
		printf("Aligned base address %x\n", aligned_base);


		/*
		   Align the rotator buffer on a 8MB boundary.
		printf("Rotated base address %x\n", rotate_base);
		rotate_aligned_base = (((uintptr_t)rotate_base + (BYTES_PER_ROTATOR-1))
		& ~ (uintptr_t)(BYTES_PER_ROTATOR-1));
		printf("Aligned Rotated base address %x\n", rotate_aligned_base);
		 */

		panel_info.vl_col = 480;
		panel_info.vl_row = 272;
		/*
		   Initialize the frame buffer to a red, green,
		   blue test pattern.  The Altera Frame Buffer
		   Controller uses 24 bits per pixel aligned on
		   32 bit word boundaries.  The MSB 8 bits are
		   not used.
		*/

		base = (unsigned long *) aligned_base;
		for (row = 0; row < 272; row++)
			for (color = 0; color < 8; color++)
				for (col = 0; col < 480; col+=8) {
					switch(color)
					{
						case 0: *base = 0x0; /* Black */
						break;
						case 1: *base = 0x00FF0000; /* Red */
						break;
						case 2: *base = 0x0000FF00; /* Green */
						break;
						case 3: *base = 0x00FFFF00; /* Yellow */
						break;
						case 4: *base = 0x000000FF; /* Blue */
						break;
						case 5: *base = 0x00FF00FF; /* Magenta */
						break;
						case 6: *base = 0x0000FFFF; /* Cyan */
						break;
						case 7: *base = 0x00FFFFFF; /* White */
						break;
						default: *base = 0x0; /* Black */
						break;
					}
					base++;
				};

		/* Start up the frame rotator */
		rotate_ctrl_init(/* rotate_aligned_base */);

		/*
		   Start up Altera frame buffer controller.  It is assumed
		   that the FPGA has been configured and the HPS bus to the
		   FPGA has been enabled.
		*/
		lcd_ctrl_init(aligned_base);

		/* Wait two seconds. */
		udelay(2000000);

		/* Put diagnostic image into frame buffer. */
/*
		base = (unsigned long *) aligned_base;
		for (row = 0; row < 272; row++)
			for (col = 0; col < 480; col++) {
				*base = 0x5a000000 | (row << 12) | col;
				base++;
			};
*/

		/* Copy splash screen image to frame buffer */
		base = (unsigned long *) aligned_base;
		splash = (unsigned long *) splash_image;
		printf("Splash image base address %x\n", splash_image);
		for (row = 0; row < 272; row++)
			for (col = 0; col < 480; col++) {
				*base = *splash;
				base++;
				splash++;
			}
		
	
		/* Print the ABI version */
		app_startup(argv);
		printf ("Example expects ABI version %d\n", XF_VERSION);
		printf ("Actual U-Boot ABI version %d\n", (int)get_version());
	
		printf ("Hello World\n");
	
		printf ("argc = %d\n", argc);
	
		for (i=0; i<=argc; ++i) {
			printf ("argv[%d] = \"%s\"\n",
				i,
				argv[i] ? argv[i] : "<NULL>");
		}
/*	
		printf ("Hit any key to exit ... ");
		while (!tstc())
*/
			;
		/* consume input */
/*
		(void) getc();
*/
	
		printf ("\n\n");
		/* Free frame buffer */
/*
		free(lcdbase);
*/
	};
	return (0);
}


/*
   Rotator initialization function located here so that entry point
   of hello_world won't change from previous versions which didn't
   have the rotator hardware.

   If this is located before "hello_world" then U-Boot startup script
   will need to jump to a different start address.
*/
void rotate_ctrl_init(/* void *rotatebase */)
{
    unsigned long base;
    base = (unsigned long)(H2F_LW_AXI);

    printf("Turning off rotator.\n");
    writel(ROTATOR_OFF, base + ROTATE_FB_MACNICA + ROTATOR_CTRL);

    /*
    printf("Writing rotator buffer address.\n");
    writel(rotatebase, base + ROTATE_FB_MACNICA + ROTATOR_BUFFER);
    */

    printf("Starting rotator.\n");
    writel((ROTATOR_ON + ROTATOR_180), base + ROTATE_FB_MACNICA + ROTATOR_CTRL);
    printf("Rotator should have started.\n");

    return 0;
}
