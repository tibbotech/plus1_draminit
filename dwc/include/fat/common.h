#ifndef _COMMON_INC_
#define _COMMON_INC_

#include <spinand_boot/sp_spinand_q645.h>
#include <sdmmc_boot/hal_sd_mmc.h>

/*
 * storage_buf usage depends on storage driver
 */
union storage_buf {
	/* nand */
	struct nand_io {
		u8 data[8 * 1024];
		u8 spare[512];
#define NAND_MAX_DESC_SIZE 4
		desc_command_t descInfo[NAND_MAX_DESC_SIZE];    /* 32*4 bytes */
	} nand;

	/* usb */
	struct usb_io {
		u8           cmd_buf[1024];         /* for USB_vendorCmd(), usb descriptors */
#ifdef CONFIG_HAVE_SNPS_USB3_DISK
		xhci_usb     xhci;
#endif
		u8           draminit_tmp[7 * 1024];
		u8           sect_buf[512];         /* for FAT data read,  sector-size buffer */

#ifdef CONFIG_HAVE_USB_DISK
		ehci_usb     ehci;                  /* for all usb transfer, 320 bytes */
#endif

	} usb;

	/* uart bootstrap */
	struct bstrap_io {
		volatile u32  *UART_data;
		volatile u32  *UART_lsr;
		u32           timer_enable;
		u32           length;
		u32           flag;
		u8            checkSum;
		u8            count;
		u8            cmd;
	} bstrap;

	// generic
	//u8 g_storage_buf[STORAGE_BUF_SIZE];
};

#define SB_FLAG_ENABLE    1

struct bootinfo {
	u32     bootrom_ver;         // iboot version
	u32     hw_bootmode;         // hw boot mode (latched: auto, nand, usb_isp, sd_isp, etc)
	u32     gbootRom_boot_mode;  // sw boot mode (category: nand, sd, usb)
	u32     bootdev;             // boot device (exact device: sd0, sd1, ...)
	u32     bootdev_pinx;        // boot device pinmux
	u32     bootdev_port;        // usb0~1, sd0~1
	u32     app_blk_start;       // the block after xboot block(s)
	u32     mp_flag;             // mp machine flag
	u32     bootcpu;             // 0: B, 1: A
	u32     in_xboot;            // 0=in iboot, 1=in xboot
#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
	u32     hw_security;         // hw security
#endif
	u32     sb_flag;             // secure boot flag, bit0=1(secure boot)

	/*
	 * ROM code puts all C code global variables here so that :
	 * 1. xboot can continue driver operations (eg. skip nand init)
	 * 2. ROM functions may be reused in xboot
	 */

	/* common/bootmain.c */
	u32     sd_cur_base;

	/* nand/nandop.c */
	struct SysInfo sys_nand;
	UINT8 gInitNand_flag;
	UINT8 gheader_already_read_out_flag;
	UINT8 gRandomizeFlag;
	UINT8 reserved;
	SDev_t gsdev;

	/* nand/nfdriver.c */
	UINT32 g_int_wake_up_flag;
	UINT8 software_protect_byte[6];
	UINT8 gNANDIDLength;
	desc_command_t *nand_desc_comm;

	/* nand/spi_nand.c */
	struct sp_spinand_info our_spinfc;

	/* MMC */
	int gSD_HIGHSPEED_EN_SET_val[4];
	int gSD_RD_CLK_DELAY_TIME_val[4];
	struct STORAGE_DEVICE sd_Storage_dev;
	int mmc_active_part;

	/* fat */
	unsigned char fat_fileName[1][12];
};

/*
 * global variables
 */
extern union storage_buf   g_io_buf;
extern u8                  g_xboot_buf[];
extern struct bootinfo     g_bootinfo;

#endif /* _COMMON_INC_ */
