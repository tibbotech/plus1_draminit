#ifndef _COMMON_INC_
#define _COMMON_INC_

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
/*
 * global variables
 */
extern union storage_buf   g_io_buf;
extern u8                  g_xboot_buf[];

#endif /* _COMMON_INC_ */
