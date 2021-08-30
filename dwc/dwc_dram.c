/*
 * C code entry: dram_init_main();
 */

#include <types.h>
#include <common.h>
#include <config.h>
#include <dwc_dram_param.h>
#include <dwc_ddrphy_phyinit.h>

#include <bootmain.h>
#include <nand_boot/nfdriver.h>
#include <nand_boot/nandop.h>
#include <nand_boot/hal_nand_error.h>
#include <fat/common.h>
#include <fat/fat.h>


#define SPI_FLASH_BASE      0xF0000000
#define SPI_XBOOT_OFFSET    (96 * 1024)

#define EMMC_BOOT               0x1F
#define SPINAND_BOOT            0x1D
#define USB_ISP                 0x1B
#define SDCARD_ISP              0x19
#define SPI_NOR_BOOT            0x17
#define UART_ISP                0x15
#define AUTO_SCAN               0x11
#define USB_BOOT                0xfd
#define SDCARD_BOOT             0xfe
#define NAND_LARGE_BOOT         0xff

#define rsize 512
#define IM1D_HDR_MAGIC   0x64316d69
#define DM1D_HDR_MAGIC   0x64316d64
#define IM2D_HDR_MAGIC   0x64326d69
#define DM2D_HDR_MAGIC   0x64326d64

typedef unsigned int        u32;

#define MO3_REG ((volatile struct int_regs *)RF_GRP(3, 0))
struct int_regs {
    UINT32  mo3_reserved[32];
};

#define CSTAMP(value)      { *(volatile unsigned int *)RF_GRP(0, 0) = (unsigned int)ADDRESS_CONVERT(value); }

// #define DRAM_INIT_DEBUG 0 // defined in Makefile: please use "make debug"

#ifdef	CSIM_ASIC
#define	ASIC_CSIM
#elif	defined(CSIM_FPGA)
#define	SDRAM_FPGA
#else
#error Please define => simulation type
#endif

struct sp_registers {
	unsigned int sp_register[1024][32];
};
static volatile struct sp_registers *sp_reg_ptr = (volatile struct sp_registers *)(RF_GRP(0, 0));
#define SP_REG(GROUP, OFFSET)		(sp_reg_ptr->sp_register[GROUP][OFFSET])
struct dwc_phy_regs {
	UINT16 dwc_phy_reg[4096];	/* change the size here, (area >> 2) */
};
static volatile struct dwc_phy_regs *dwc_phy_reg_ptr = (volatile struct dwc_phy_regs *)(DWC_PHY_REG_Base);
#define DWC_PHY_REG(OFFSET)		(dwc_phy_reg_ptr->dwc_phy_reg[OFFSET ])

#define TEST_LEN_0		(4 << 10)

#ifdef SDRAM0_SIZE_256Mb
#define TEST_LEN_ALL		(32 << 20)
#elif defined(SDRAM0_SIZE_512Mb)
#define TEST_LEN_ALL		(64 << 20)
#elif defined(SDRAM0_SIZE_1Gb)
#define TEST_LEN_ALL		(128 << 20)
#elif defined(SDRAM0_SIZE_2Gb)
#define TEST_LEN_ALL		(256 << 20)
#elif defined(SDRAM0_SIZE_4Gb)
#define TEST_LEN_ALL		(512 << 20)
#elif defined(SDRAM0_SIZE_8Gb)
#define TEST_LEN_ALL		(1024 << 20)
#else
#error Please assign TEST_LEN_ALL
#endif

#define SDRAM0_SIZE		TEST_LEN_ALL
#define SDRAM1_SIZE		SDRAM0_SIZE
#if defined(PLATFORM_PENTAGRAM) || defined(PLATFORM_Q645)
static const unsigned int dram_base_addr[] = {0, SDRAM0_SIZE};
#else
static const unsigned int dram_base_addr[] = {0x20000000, SDRAM0_SIZE};
#endif
//static const unsigned int dram_size[] = {SDRAM0_SIZE, SDRAM1_SIZE};

#define DRAM_0_SDC_REG_BASE	33
#define DRAM_0_PHY_REG_BASE	768
#define DRAM_1_SDC_REG_BASE	0	/* N/A */
#define DRAM_1_PHY_REG_BASE	0	/* N/A */

#define SCAN_TRIM_LEN		5

static unsigned int rgst_value = 0;
//static unsigned int aphy_select_value = 0;
static unsigned int ckobd_training_flag = 0;
//static unsigned int ckobd_re_training_number = 0;

//static unsigned int data_byte_0_RDQSG_left_total_tap = 0;
//static unsigned int data_byte_0_RDQSG_right_total_tap = 0;
//static unsigned int data_byte_1_RDQSG_left_total_tap = 0;
//static unsigned int data_byte_1_RDQSG_right_total_tap = 0;
static unsigned int gAC, gACK, gCK;

static unsigned int bootdevice;

static unsigned int XBOOT_len  = 0;
static unsigned int IMEM1d_len = 0;
static unsigned int DMEM1d_len = 0;
static unsigned int IMEM2d_len = 0;
static unsigned int DMEM2d_len = 0;

#ifdef CONFIG_DRAM_SIZE_USE_OTP
static unsigned int DRAM_SIZE_FLAG;
#define DRAM_SIZE_512Mb  0x0
#define DRAM_SIZE_1Gb    0x1
#define DRAM_SIZE_2Gb    0x2
#define DRAM_SIZE_4Gb    0x3
#endif

#if (defined(DRAMSCAN) || defined(SISCOPE))
static unsigned int scan_val_190;
#endif
u32 mp;
#ifdef PLATFORM_PENTAGRAM
#define CHIP_WARM_RESET
#endif
//#define SDRAM_WATCHDOG
#ifdef SDRAM_WATCHDOG
#define WATCHDOG_CMD_CNT_WR_UNLOCK  0xAB00
#define WATCHDOG_CMD_CNT_WR_LOCK    0xAB01
#define WATCHDOG_CMD_CNT_WR_MAX     0xDEAF
#define WATCHDOG_CMD_PAUSE          0x3877
#define WATCHDOG_CMD_RESUME         0x4A4B
#define WATCHDOG_CMD_INTR_CLR       0x7482
#endif
void get_sdc_phy_addr(unsigned int dram_id, unsigned int *sdc, unsigned int *phy)
{
	const unsigned int dram_sdc_reg_addr[] = {DRAM_0_SDC_REG_BASE, DRAM_1_SDC_REG_BASE};
	const unsigned int dram_phy_reg_addr[] = {DRAM_0_PHY_REG_BASE, DRAM_1_PHY_REG_BASE};

	if (dram_id < (sizeof(dram_sdc_reg_addr) / sizeof(dram_sdc_reg_addr[0]))) {
		*sdc = dram_sdc_reg_addr[dram_id];
		*phy = dram_phy_reg_addr[dram_id];
	} else {
		prn_string("Err: get_sdc_phy_addr, invalid dram_id\n");
		while (1);
	}
}

void wait_loop(unsigned int wait_counter)
{
	unsigned int i;

	for (i = 0; i < wait_counter; i++) {
		__asm__("nop");
	}
}

void dram_fill_zero(unsigned int test_size, unsigned int dram_id)
{
	int idx;
	volatile unsigned int *ram = (volatile unsigned int *)ADDRESS_CONVERT(dram_base_addr[dram_id]);

	for (idx = 0; idx < (test_size / sizeof(unsigned int)); idx++) {
		ram[idx] = 0;
	}
}

int memory_rw_check(unsigned int value, unsigned int answer, int debug)
{
	int ret = 0;

	if (value != answer) {
		if (debug) {
			prn_string("\tvalue: ");
			prn_dword0(value);
			prn_string(", expected: ");
			prn_dword0(answer);
			prn_string("\n");
		}
		ret = -1;
	}
	return ret;
}

const unsigned int pattern[] = {0xAAAAAAAA, 0x55555555, 0xAAAA5555, 0x5555AAAA, 0xAA57AA57, 0xFFDDFFDD, 0x55D755D7};
int memory_rw_test_cases(int test_case, unsigned int start_addr, unsigned int test_size, int debug)
{
	int ret = 0;
	unsigned int i;
	unsigned int test_size_word = test_size >> 2;
	const int num_pattern = sizeof(pattern) / sizeof(pattern[0]);
//	volatile unsigned int *ram = (volatile unsigned int *)(dram_base_addr[0]);
	volatile unsigned int *ram = (volatile unsigned int *)ADDRESS_CONVERT(start_addr);

	// TODO: Use CBDMA.

	//dram_fill_zero(test_size, 0);

	debug = 1;
	if (debug) {
		prn_string("\t memory_rw_test(");
	}
	switch (test_case) {
	case 0:
		if (debug) {
			prn_string("seq)");
		}
		for (i = 0; i < test_size_word; i++) {
			ram[i] = i;
			prn_string("1");
		}
		break;
	default:
		if (debug) {
			prn_string("patterns)");
		}
		for (i = 0; i < test_size_word; i++) {
			ram[i] = pattern[i % num_pattern];
		}
		break;
	}

	for (i = 0; i < test_size_word; i++) {
		switch (test_case) {
		case 0:
			prn_string("2");
			prn_string("5");
			ret = memory_rw_check(ram[i], i, debug);
			prn_string("3");
			break;

		default:
			ret = memory_rw_check(ram[i], pattern[i % num_pattern], debug);
			break;
		}

		if (ret < 0) {
			if (debug) {
				prn_string(" fails\n");
			}
			ret = -1;
			break;
		}
	}

	if (ret == 0) {
		if (debug) {
			prn_string(" pass\n");
		}
	}

	return ret;
}

#define MEMORY_RW_FLAG_DBG	(1 << 0)
#define MEMORY_RW_FLAG_LOOP	(1 << 1)
#define MEMORY_RW_FLAG_EXIT	(1 << 2)
int memory_rw_test(unsigned int start_addr, unsigned int test_len, int flag)
{
	int ret;
	int is_dbg = flag & MEMORY_RW_FLAG_DBG;
	int exit = flag & MEMORY_RW_FLAG_EXIT;
	int test_case = 0;

	do {
		do {
			ret = memory_rw_test_cases(test_case, start_addr, test_len, is_dbg);
			if ((ret < 0) && exit) {
				return ret;
			}
			test_case++;
			test_case %= 2;
		} while (test_case != 0);
	} while (flag & MEMORY_RW_FLAG_LOOP);

	return 0;
}

//////////////////////////////////////////////////////
///////////////LPDDR4////////////////////////////////
//////////////////////////////////////////////////////
void dwc_ddrphy_apb_wr(UINT32 adr, UINT32 dat)
{
	//dwc_ddrphy_phyinit_print ("dwc_ddrphy_apb_wr(12'h%x, 32'h%x);\n", adr, dat);
	#if 0
	if(mp == 1)
	{
		prn_string("APB W  PUB  ");
		prn_dword0(adr);
		prn_string("  ");
		prn_dword0(dat);
		prn_string("\n");
	}
	#endif
	DWC_PHY_REG(adr)=dat;
}

UINT16 dwc_ddrphy_apb_rd(UINT32 adr)
{
	UINT16 value;
	//dwc_ddrphy_phyinit_print ("dwc_ddrphy_apb_rd(12'h%x, rd_data);\n", adr);
	value= DWC_PHY_REG(adr);
	return value;
}

#define IMEM_ADDR 0x50000
#define DMEM_ADDR 0x54000
#define mem_size 128
unsigned int sum = 0;
unsigned int mem[mem_size];
void tcpsum(unsigned int StartNo, unsigned int EndNo, unsigned int *buf, unsigned char flag)
{
	unsigned short word16_h,word16_l;
	int i;

	/* Accumulate checksum */
	for (i = StartNo; i < EndNo; i++) {
		word16_h = (buf[i]>>16)&0xFFFF;
		word16_l = buf[i]&0xFFFF;
		sum += word16_l;
		sum += word16_h;
	}

	if (flag == 0)
		return;
	/* Fold to get the ones-complement result */
	while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);

	/* Invert to get the negative in ones-complement arithmetic */
	sum = ~sum;

}

void DwcCheckSum(unsigned int magic, unsigned int checksum)
{
	if ((sum&0x0000FFFF) != checksum) {
		if (magic == IM1D_HDR_MAGIC)
			prn_string("1D IMEM ");
		else if (magic == DM1D_HDR_MAGIC)
			prn_string("1D DMEM ");
		else if (magic == IM2D_HDR_MAGIC)
			prn_string("2D IMEM ");
		else if (magic == DM2D_HDR_MAGIC)
			prn_string("2D DMEM ");

		prn_string("checksum error!!!!\n");
		prn_string("sum="); prn_dword(sum);
		prn_string("checksum="); prn_dword((checksum));
	} else {
		if (magic == IM1D_HDR_MAGIC)
			prn_string("1D IMEM checksum ok!!!!\n");
		else if (magic == DM1D_HDR_MAGIC)
			prn_string("1D DMEM checksum ok!!!!\n");
		else if (magic == IM2D_HDR_MAGIC)
			prn_string("2D IMEM checksum ok!!!!\n");
		else if (magic == DM2D_HDR_MAGIC)
			prn_string("2D DMEM checksum ok!!!!\n");
	}
	sum = 0;    // Reset DWC checksum
}

void printf_offset_value(unsigned short offset, unsigned short value)
{
	prn_string("mem_offset="); prn_dword(offset);
	prn_string("value="); prn_dword(value);
}

void printf_sectorNo(unsigned short sectorNo0, unsigned short sectorNo1)
{
	prn_string("sectorNo0="); prn_dword(sectorNo0);
	prn_string("sectorNo1="); prn_dword(sectorNo1);
}

void LoadBinCode(unsigned char Train2D, unsigned int offset, unsigned int MEM_ADDR)
{
	struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET+offset);//xboot start addr
	unsigned short i, j, addr, num0, num1,word16;

	prn_string("mg=");
	prn_dword(xhdr->magic);
	prn_string("len=");
	prn_dword(xhdr->length);
	prn_string("chk=");
	prn_dword(xhdr->checksum);
	// checksum verify
	num0 = (xhdr->length/rsize);
	num1 = (xhdr->length%rsize);

	for (i=0; i<num0; i++) {
		for (addr=0; addr<mem_size; addr++)
			mem[addr]=0;

		unsigned int *src = (unsigned int*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET+offset+32+(rsize*i));
		memcpy32(mem, src, rsize/4); //copy data 512 bytest
		tcpsum(0, 128, mem, 0);//checksum
		for (j=0; j<128; j++) {
			/*****write register *********/
			word16 = mem[j]&0xFFFF;
			dwc_ddrphy_apb_wr(MEM_ADDR+ (256*i)+(j*2), word16);
			//printf_offset_value((256*i)+(j*2),word16);
			word16 = (mem[j]>>16)&0xFFFF;
			dwc_ddrphy_apb_wr(MEM_ADDR+ (256*i)+(j*2)+1, word16);
			//printf_offset_value((256*i)+(j*2)+1,word16);
		}
	}

	for (addr=0; addr<mem_size; addr++) {
		mem[addr]=0;
	}

	unsigned int *src =  (unsigned int*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET+offset+32+(rsize*num0));
	memcpy32(mem, src, num1/4);//copy data
	tcpsum(0,num1/4, mem, 1);//checksum
	for (j=0; j<(num1/4); j++) {
		/*****write register *********/
		word16 = mem[j]&0xFFFF;
		dwc_ddrphy_apb_wr(MEM_ADDR+ (256*num0)+(j*2), word16);
		//printf_offset_value((256*num0)+(j*2),word16);
		word16 = (mem[j]>>16)&0xFFFF;
		dwc_ddrphy_apb_wr(MEM_ADDR+ (256*num0)+(j*2)+1, word16);
		//printf_offset_value((256*num0)+(j*2)+1,word16);
	}

	DwcCheckSum(xhdr->magic, xhdr->checksum);
	sum = 0;
}

void LoadBinCodeForSectorMode(unsigned char Train2D, unsigned int offset, unsigned int MEM_ADDR)
{
	unsigned short i, j, addr, word16;
	unsigned short mem_offset, img_length,cnt;
	unsigned int img_name, img_sum;

	mem_offset = 0;
	img_name = 0;
	img_length = 0;
	img_sum = 0;

	//Read first block
	for (addr = 0; addr < mem_size; addr++)
		mem[addr]=0;
	ReadSDSector(offset, 1, mem);
	for (j = 0; j < 128; j++) {
		if((mem[j] == IM1D_HDR_MAGIC) || (mem[j] == DM1D_HDR_MAGIC)
			|| (mem[j] == IM2D_HDR_MAGIC) || (mem[j] == DM2D_HDR_MAGIC)) //j is array number
		{
			//prn_string("mem[j]=");
			//prn_dword(mem[j]);
		    img_name = mem[j];
			if(mem[j] == IM1D_HDR_MAGIC)
				IMEM1d_len = mem[j+2];
			else if(mem[j] == DM1D_HDR_MAGIC)
				DMEM1d_len = mem[j+2];
			else if(mem[j] == IM2D_HDR_MAGIC)
				IMEM2d_len = mem[j+2];
			else if(mem[j] == DM2D_HDR_MAGIC)
				DMEM2d_len = mem[j+2];
			//prn_string("leng=");
			//prn_dword(mem[j+2]);
			img_length = mem[j+2];
			//prn_string("checksum=");
			//prn_dword(mem[j+3]);
			img_sum =  mem[j+3];
			//prn_string("j=");
			//prn_dword(j);
			if((j+8-1) > 127) //add header length 8*4=32bytes overlap block size
			{
				i = (j+8-1)-127;//i is counter.
				for (addr = 0; addr < mem_size; addr++)
					mem[addr]=0;
				offset++;
				ReadSDSector(offset, 1, mem);
				tcpsum(i, 128, mem, 0);//checksum
			}
			else
			{
				i = j+8;
				tcpsum(i, 128, mem, 0);//checksum
			}
			for (j = i; j < 128; j++) {
				/*****write register *********/
				word16 = mem[j]&0xFFFF;
				dwc_ddrphy_apb_wr(MEM_ADDR+mem_offset, word16);
				//printf_offset_value(mem_offset,word16);
				mem_offset++;
				word16 = (mem[j]>>16)&0xFFFF;
				dwc_ddrphy_apb_wr(MEM_ADDR+mem_offset, word16);
				//printf_offset_value(mem_offset,word16);
				mem_offset++;
				img_length -= 4;
			}
			break;
		}
	}

	//Read middle block
	while(img_length > 512)
	{
		for (addr = 0; addr < mem_size; addr++)
			mem[addr]=0;
		offset++;
		ReadSDSector(offset, 1, mem);
		tcpsum(0, 128, mem, 0);//checksum
		for (j = 0; j < 128; j++) {
			/*****write register *********/
			word16 = mem[j]&0xFFFF;
			dwc_ddrphy_apb_wr(MEM_ADDR+mem_offset, word16);
			//printf_offset_value(mem_offset,word16);
			mem_offset++;
			word16 = (mem[j]>>16)&0xFFFF;
			dwc_ddrphy_apb_wr(MEM_ADDR+mem_offset, word16);
			//printf_offset_value(mem_offset,word16);
			mem_offset++;
		}
		img_length -= 512;
		//prn_string("img_length");
		//prn_dword(img_length);
	}

	//Read last block
	for (addr = 0; addr < mem_size; addr++)
		mem[addr]=0;
	offset++;
	ReadSDSector(offset, 1, mem);
	cnt = img_length/4;
	tcpsum(0, cnt, mem, 1);//checksum
	//prn_string("cnt=");
	//prn_dword(cnt);
	for (j = 0; j < cnt; j++){
		/*****write register *********/
		word16 = mem[j]&0xFFFF;
		dwc_ddrphy_apb_wr(MEM_ADDR+mem_offset, word16);
		//printf_offset_value(mem_offset,word16);
		mem_offset++;
		word16 = (mem[j]>>16)&0xFFFF;
		dwc_ddrphy_apb_wr(MEM_ADDR+mem_offset, word16);
		//printf_offset_value(mem_offset,word16);
		mem_offset++;
	}

	DwcCheckSum(img_name, img_sum);
	sum = 0;
}

#ifdef CONFIG_HAVE_SPI_NAND
void LoadBinForNAND(int Train2D, int mem_type, u32 pg_off, UINT32 *buf)
{
	u32 read_bytes = 0;
	int sz_sect = GetNANDPageCount_1K60(g_bootinfo.sys_nand.u16PyldLen) * 1024;
	int res;
	u32 mem_addr = 0, mem_offset = 0;
	u32 img_name = 0, img_length = 0, img_sum = 0;
	u32 i, j, cnt, word16;

	//prn_string("LoadBinForNAND\n");

	sum = 0;    // Reset DWC checksum
	cnt = sz_sect / 4;
	mem_addr = (mem_type == 0)? IMEM_ADDR : DMEM_ADDR;

	/* Read first page of BIN file */
	//prn_string("Read first page\n");
	prn_string("1st pg_off="); prn_decimal(pg_off); prn_string("\n");
	res = SPINANDReadNANDPage_1K60(NAND_CS0, pg_off, (u32 *)buf, &read_bytes);
	if (res != ROM_SUCCESS) {
		prn_string("failed at pg="); prn_dword(pg_off);
		//break;
	}

	// Search BIN header
	//prn_string("Search BIN header\n");
	for (i = 0; i < cnt; i++) {
		if ((buf[i] == IM1D_HDR_MAGIC) || (buf[i] == DM1D_HDR_MAGIC) ||
			(buf[i] == IM2D_HDR_MAGIC) || (buf[i] == DM2D_HDR_MAGIC)) {
			prn_string("Found BIN at i="); prn_dword(i);

			img_name   = buf[i];
			img_length = buf[i+2];
			img_sum    = buf[i+3];
			prn_string("img_name="); prn_dword(img_name);
			prn_string("img_length="); prn_dword(img_length);
			prn_string("img_sum="); prn_dword(img_sum);

			// Check if BIN image is correct
			if (((Train2D == 0) && (mem_type == 0) && (img_name != IM1D_HDR_MAGIC)) ||
				((Train2D == 0) && (mem_type == 1) && (img_name != DM1D_HDR_MAGIC)) ||
				((Train2D == 1) && (mem_type == 0) && (img_name != IM2D_HDR_MAGIC)) ||
				((Train2D == 1) && (mem_type == 1) && (img_name != DM2D_HDR_MAGIC))) {
				prn_string("Wrong image! img_name="); prn_dword(img_name);
			}

			// Check first data position
			j = i + 8;
			//prn_string("j="); prn_dword(j);
			if (j >= cnt) {
				// The first page of BIN files spans two NAND pages
				j = j - cnt;
				pg_off++;
				//prn_string("pg_off="); prn_decimal(pg_off); prn_string("\n");
				res = SPINANDReadNANDPage_1K60(NAND_CS0, pg_off, (u32 *)buf, &read_bytes);
				if (res != ROM_SUCCESS) {
					prn_string("failed at pg="); prn_dword(pg_off);
					//break;
				}

				// Get BIN checksum and length again
				if (j > 4) {
					img_sum = buf[j-5];
					//prn_string("img_sum="); prn_dword(img_sum);
				}
				if (j > 5) {
					img_length = buf[j-4];
					//prn_string("img_length="); prn_dword(img_length);
				}
			}
			tcpsum(j, cnt, buf, 0);

			// Store BIN file length. Add it to header length, 32 bytes.
			if (img_name == IM1D_HDR_MAGIC)
				IMEM1d_len = img_length + 32;
			else if (img_name == DM1D_HDR_MAGIC)
				DMEM1d_len = img_length + 32;
			else if (img_name == IM2D_HDR_MAGIC)
				IMEM2d_len = img_length + 32;
			else if (img_name == DM2D_HDR_MAGIC)
				DMEM2d_len = img_length + 32;

			// Write DWC PHY register
			//prn_string("Write DWC PHY register\n");
			//prn_string("mem_addr="); prn_dword(mem_addr);
			for (; j < cnt; j++) {
				//prn_string("j="); prn_dword(j);
				word16 = buf[j]&0xFFFF;
				dwc_ddrphy_apb_wr(mem_addr+mem_offset, word16);
				//printf_offset_value(mem_offset,word16);
				mem_offset++;
				word16 = (buf[j]>>16)&0xFFFF;
				dwc_ddrphy_apb_wr(mem_addr+mem_offset, word16);
				//printf_offset_value(mem_offset,word16);
				mem_offset++;
				img_length -= 4;
			}
			//prn_string("img_length="); prn_dword(img_length);
			break;
		}
	}

	/* Read middle pages of BIN file */
	//prn_string("Read middle pages\n");
	while(img_length > sz_sect) {
		pg_off++;
		//prn_string("pg_off="); prn_decimal(pg_off); prn_string("\n");
		res = SPINANDReadNANDPage_1K60(NAND_CS0, pg_off, (u32 *)buf, &read_bytes);
		if (res != ROM_SUCCESS) {
			prn_string("failed at pg="); prn_dword(pg_off);
			//break;
		}
		tcpsum(0, cnt, buf, 0);

		// Write DWC PHY register
		//prn_string("Write DWC PHY register\n");
		for (j = 0; j < cnt; j++) {
			//prn_string("j="); prn_dword(j);
			word16 = buf[j]&0xFFFF;
			dwc_ddrphy_apb_wr(mem_addr+mem_offset, word16);
			//printf_offset_value(mem_offset,word16);
			mem_offset++;
			word16 = (buf[j]>>16)&0xFFFF;
			dwc_ddrphy_apb_wr(mem_addr+mem_offset, word16);
			//printf_offset_value(mem_offset,word16);
			mem_offset++;
			img_length -= 4;
		}
		//prn_string("img_length ="); prn_dword(img_length);
	}

	/* Read last page of BIN file */
	//prn_string("Read last page\n");
	pg_off++;
	//prn_string("pg_off="); prn_decimal(pg_off); prn_string("\n");
	res = SPINANDReadNANDPage_1K60(NAND_CS0, pg_off, (u32 *)buf, &read_bytes);
	if (res != ROM_SUCCESS) {
		prn_string("failed at pg="); prn_dword(pg_off);
		//break;
	}
	i = img_length / 4;
	tcpsum(0, i, buf, 1);
	//prn_string("i="); prn_dword(i);

	// Write DWC PHY register
	//prn_string("Write DWC PHY register\n");
	for (j = 0; j < i; j++) {
		//prn_string("j="); prn_dword(j);
		word16 = buf[j] & 0xFFFF;
		dwc_ddrphy_apb_wr(mem_addr+mem_offset, word16);
		//printf_offset_value(mem_offset,word16);
		mem_offset++;
		word16 = (buf[j] >> 16) & 0xFFFF;
		dwc_ddrphy_apb_wr(mem_addr+mem_offset, word16);
		//printf_offset_value(mem_offset,word16);
		mem_offset++;
		img_length -= 4;
	}

	DwcCheckSum(img_name, img_sum);
}

void LoadMEMForNAND(int Train2D, int mem_type)
{
	struct BootProfileHeader *ptr = Get_Header_Profile_Ptr();
	u32 read_bytes = 0, pg_off = 0, pg_cnt = 0;
	u8 *buf = g_io_buf.nand.data;
	int res;
	int sz_sect = GetNANDPageCount_1K60(g_bootinfo.sys_nand.u16PyldLen) * 1024;
	int xbsize;

	//prn_string("LoadMEMForNAND\n");
	prn_string("Train2D="); prn_decimal(Train2D); prn_string("; ");
	prn_string("mem_type="); prn_decimal(mem_type); prn_string("\n");

	// Get NAND infor from Boot Profile header
	pg_off = ptr->xboot_pg_off; 	// Xboot offset
	//prn_string("pg_off="); prn_dword(pg_off);
	//prn_string("sz_sect="); prn_dword(sz_sect);

	if (XBOOT_len == 0) {
		// Get Xboot length from NAND
		//prn_string("Get Xboot length from NAND\n");
		res = SPINANDReadNANDPage_1K60(NAND_CS0, pg_off, (u32 *)buf, &read_bytes);
		if (res != ROM_SUCCESS) {
			prn_string("failed at pg="); prn_dword(pg_off);
			//break;
		}

		xbsize = get_xboot_size(buf);
		XBOOT_len = xbsize;
	}
	//prn_string("XBOOT_len="); prn_dword(XBOOT_len);

	if (mem_type == 0) {
		if (Train2D == 0) {
			pg_cnt = XBOOT_len + sz_sect - 1;
		} else {
			//prn_string("DMEM1d_len="); prn_dword(DMEM1d_len);
			pg_cnt = XBOOT_len + IMEM1d_len + DMEM1d_len + sz_sect - 1;
		}
	} else {
		if (Train2D == 0) {
			//prn_string("IMEM1d_len="); prn_dword(IMEM1d_len);
			pg_cnt = XBOOT_len + IMEM1d_len + sz_sect - 1;
		} else {
			//prn_string("IMEM2d_len="); prn_dword(IMEM2d_len);
			pg_cnt = XBOOT_len + IMEM1d_len + DMEM1d_len + IMEM2d_len + sz_sect - 1;
		}
	}
	pg_cnt /= sz_sect;
	pg_off = pg_off + pg_cnt - 1;

	LoadBinForNAND(Train2D, mem_type, pg_off, (u32 *)buf);
}
#endif


void dwc_ddrphy_phyinit_D_loadIMEM_of_SP(int Train2D)
{
	mp = 0;
	if (bootdevice == SPI_NOR_BOOT)
	{
		struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET);//xboot start addr
		unsigned int offset;
		offset = sizeof(struct xboot_hdr) + xhdr->length; //xboot length
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
		if (Train2D == 0)
			LoadBinCode(0,offset,IMEM_ADDR);
		offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d  length
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
		offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d+dm1d  length
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
		if (Train2D == 1)
			LoadBinCode(1,offset,IMEM_ADDR);
		offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d+dm1d+im2d  length
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
	}
	else if ((bootdevice == EMMC_BOOT) || (bootdevice == SDCARD_ISP))
	{
		unsigned int sectorNo0, total_length, addr;
		u8 *buf = (u8 *) g_io_buf.usb.draminit_tmp;
		struct xboot_hdr *xhdr = (struct xboot_hdr *)buf;
		int ret;

		if (bootdevice == EMMC_BOOT)
		{
			for (addr = 0; addr < mem_size; addr++)
				mem[addr]=0;
			ReadSDSector(0, 1, mem);
			XBOOT_len = mem[2];
		}
		else if (bootdevice == SDCARD_ISP)
		{
			ret = fat_read_file(0, &g_finfo, g_io_buf.usb.sect_buf, 0, 32, buf); //for get xboot's sector No
			if (ret == FAIL) {
				prn_string("load xboot hdr failed\n");
				return;
			}
			//prn_string("xboot len=");
			//prn_dword(32 + xhdr->length);
			XBOOT_len = xhdr->length;
		}
		prn_string("XBOOT_len="); prn_dword(XBOOT_len);
		if (Train2D == 0)
		{
			total_length = 32 + XBOOT_len + 32; //xboot header lenght + xboot length + IMEM header length
			sectorNo0 = total_length / 512;
			LoadBinCodeForSectorMode(0,(xboot_start_secotr+sectorNo0),IMEM_ADDR);
		}
		else if (Train2D == 1)
		{
			total_length = 32 + XBOOT_len + 32 + IMEM1d_len + 32 + DMEM1d_len + 32; //xboot header lenght + xboot length + IMEM header length
			sectorNo0 = total_length / 512;
			LoadBinCodeForSectorMode(0,(xboot_start_secotr+sectorNo0),IMEM_ADDR);
		}
	}
#ifdef CONFIG_HAVE_SPI_NAND
	else if (bootdevice == SPINAND_BOOT)
	{
		LoadMEMForNAND(Train2D, 0);
	}
#endif
	mp = 1;
}

void dwc_ddrphy_phyinit_F_loadDMEM_of_SP(int pstate, int Train2D)
{
	mp = 0;
	if(bootdevice == SPI_NOR_BOOT)
	{
		struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET);//xboot start addr
		unsigned int offset;
		offset = sizeof(struct xboot_hdr) + xhdr->length; //xboot length
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
		offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d  length
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
		if (Train2D == 0)
			LoadBinCode(0,offset,DMEM_ADDR);
		offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d+dm1d  length
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
		offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d+dm1d+im2d  length
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
		if (Train2D == 1)
			LoadBinCode(1,offset,DMEM_ADDR);
	}
	else if((bootdevice == EMMC_BOOT) || (bootdevice == SDCARD_ISP))
	{
		int sectorNo0, total_length;
		if (Train2D == 0)
		{
			total_length = 32 + XBOOT_len + 32 + IMEM1d_len +32 ; //xboot header lenght + xboot length + IMEM header length
			sectorNo0 = total_length / 512;
			LoadBinCodeForSectorMode(0,(xboot_start_secotr+sectorNo0),DMEM_ADDR);
		}
		else if(Train2D == 1)
		{
			total_length = 32 + XBOOT_len + 32 + IMEM1d_len + 32 + DMEM1d_len + 32 + IMEM2d_len + 32; //xboot header lenght + xboot length + IMEM header length
			sectorNo0 = total_length / 512;
			LoadBinCodeForSectorMode(0,(xboot_start_secotr+sectorNo0),DMEM_ADDR);
		}
	}
#ifdef CONFIG_HAVE_SPI_NAND
	else if (bootdevice == SPINAND_BOOT)
	{
		LoadMEMForNAND(Train2D, 1);
	}
#endif
	mp = 1;
}

dwc_ddrphy_phyinit_saveRetention()
{
	volatile unsigned int *addr;
	unsigned int *beg  = (unsigned int *)ADDRESS_CONVERT(0x100000);
	prn_string("save retention value: ");
	prn_dword0((unsigned int)ADDRESS_CONVERT(0x100000));
	prn_string(" - ");
	prn_dword((unsigned int)ADDRESS_CONVERT(0x110000));

	int regIndx=0;
	addr = beg;
	for (regIndx = 0; regIndx < NumRegSaved; regIndx++)
	{
		prn_string("regIndx: ");
		prn_dword0(regIndx);
		prn_string("; Address: ");
		prn_dword0((unsigned int)RetRegList[regIndx].Address);
		*addr = (unsigned int) RetRegList[regIndx].Address;
		addr++;
		prn_string("; Value: ");
		prn_dword0(RetRegList[regIndx].Value);
		prn_string("\n");
		*addr = RetRegList[regIndx].Value;
		addr++;
	}
}



void dwc_ddrphy_phyinit_main(void)
{
   //#include <dwc_ddrphy_phyinit_out_lpddr4_train1d2d.txt>
   //#include <dwc_ddrphy_phyinit_out_lpddr4_skiptrain.txt>
   //#include <dwc_ddrphy_phyinit_out_lpddr4_devinit_skiptrain.txt>
   //#include <dwc_devinit_skiptrain_zebu.txt>
   //#include <dwc_ddrphy_phyinit_out_lpddr4_devinit_skiptrain_7Fto6F.txt>
   prn_string("dwc_ddrphy_phyinit_main ver.20\n");
   mp = 1;
   //runtimeConfig.RetEn = 1;
   dwc_ddrphy_phyinit_sequence(2,0,0);
}

// ***********************************************************************
// * FUNC      : startClockResetPhy_of_SP
// * PARAM     : dram_id
// * PURPOSE   :
// ***********************************************************************
void startClockResetPhy_of_SP(void)
{
	//prn_string("startClockResetPhy_of_SP");
	//prn_string("\n");

	//dbg_stamp(0xA000);
	SP_REG(0, 22) = RF_MASK_V_SET(1 << 10);	// presetn MO_UMCTL2_RST_B APB BUS reset
	SP_REG(0, 25) = RF_MASK_V_SET(1 << 9);	// aresetn_0 MO_DDRCTL_RST_B AXI bus reset
	SP_REG(0, 22) = RF_MASK_V_SET(1 << 4);	// core_ddrc_rstn CLKSDRAM0_SDCTRL0_RST_B uMCTL2 core reset
	SP_REG(0, 25) = RF_MASK_V_SET(1 << 8);	// PRESETn_APB MO_DDRPHY_RST_B APB bus reset ; CLKDFI_DDRPHY_RST_B dfi_reset
	SP_REG(3, 24) = RF_MASK_V_SET(1 << 12);	// PwrOkIn MO_DDRPHY_PWROKIN ddrphy pwrokin
	wait_loop(1000);
	//MO3_REG->mo3_reserved[24] = 0x10001000; //PwrOKIn MO_DDRPHY_PWROKIN ddrphy pwrokin
	//SP_REG(0, 22) = RF_MASK_V_CLR(1 << 10);	// presetn MO_UMCTL2_RST_B APB BUS reset
	//SP_REG(0, 25) = RF_MASK_V_CLR(1 << 8);	// PRESETn_APB MO_DDRPHY_RST_B APB bus reset ; CLKDFI_DDRPHY_RST_B dfi_reset
	SP_REG(0, 25) = RF_MASK_V_CLR(1 << 0); //CM4 Hardware IP Reset Disable tonyh add 20210608
} // end of startClockResetPhy_of_SP

void startClockResetUmctl2_of_SP(void)
{
	//prn_string("startClockResetUmctl2_of_SP");
	//prn_string("\n");

	dbg_stamp(0xA001);
	MO3_REG->mo3_reserved[24] = 0x10001000; //PwrOKIn MO_DDRPHY_PWROKIN ddrphy pwrokin
	SP_REG(0, 22) = RF_MASK_V_CLR(1 << 10);	// presetn MO_UMCTL2_RST_B APB BUS reset
	SP_REG(0, 22) = RF_MASK_V_CLR(1 << 4);	// core_ddrc_rstn CLKSDRAM0_SDCTRL0_RST_B uMCTL2 core reset
	wait_loop(1000);
	SP_REG(0, 25) = RF_MASK_V_CLR(1 << 8);	// PRESETn_APB MO_DDRPHY_RST_B APB bus reset ; CLKDFI_DDRPHY_RST_B dfi_reset
	wait_loop(1000);

	dbg_stamp(0xA002);
	dwc_umctl2_init_before_ctl_rst();
	SP_REG(0, 25) = RF_MASK_V_CLR(1 << 9);	// aresetn_0 MO_DDRCTL_RST_B AXI bus reset
	wait_loop(1000);
	wait_loop(1000);
	dwc_umctl2_init_after_ctl_rst();
	//SP_REG(0, 25) = RF_MASK_V_CLR(1 << 8);	// PRESETn_APB MO_DDRPHY_RST_B APB bus reset ; CLKDFI_DDRPHY_RST_B dfi_reset
 	wait_loop(1000);
}

// ***********************************************************************
// * FUNC      : dram_init
// * PARAM     : dram_id
// ***********************************************************************
int dram_init(unsigned int dram_id)
{
	unsigned int SDC_BASE_GRP = 0, PHY_BASE_GRP = 0;
	unsigned int max_init_fail_cnt = 15;
	unsigned int loop_time;
	unsigned int ret = 0;

	get_sdc_phy_addr(dram_id, &SDC_BASE_GRP, &PHY_BASE_GRP);

	loop_time = 0;
	prn_string("dram_init\n");

	for (loop_time = 0; loop_time < max_init_fail_cnt; loop_time++) {
		startClockResetPhy_of_SP();
		startClockResetUmctl2_of_SP();
		dwc_ddrphy_phyinit_main();
		ret = ctl_trigger_init_and_wait_normal();

		if (ret == WAIT_FLAG_FAIL) {
			prn_string("WAIT_FLAG_FAIL!!!!\n");
			return FAIL;
		} else if (ret == 0) {
			prn_string("loop_time = ");
			prn_decimal(loop_time);
			prn_string(" ***\n");
		}

		prn_string("lpddr4_training_OK\n");
		//dwc_ddrphy_phyinit_saveRetention();
		return SUCCESS;
	} // end of for loop :: loop_time for initial & training time control

	return SUCCESS;
} // end dram_init

#if (defined(DRAMSCAN) || defined(SISCOPE))

#define FAE_DEBUG
// #define DEBUG_BDD

void ddr_clk_info(void)
{
	const int clk = ((SP_REG(50, 12) & 0x7F) * 27) >> 1;

	prn_string("DDR CLOCK: ");
	prn_decimal(clk);
	prn_string(" MHz\n");
}

static int silent_dram_init(void)
{
	u32 mpb;

	mpb = mp;
	mp = 0;

	prn_string(" silent_dram_init\n");
	int ret = dram_init(0);
	mp = mpb;
	return ret;
}

void dram_scan(unsigned int dram_id)
{
	unsigned int dfi_tphy_wrdata, dfi_t_rddata_en;

	unsigned int idx;
	int ret;
	unsigned int rec_idx = 0;
	unsigned int SDC_BASE_GRP, PHY_BASE_GRP;
	int mpb;

	unsigned int cpu_test_result;
	unsigned int trim_test_result;

	unsigned int scan_pass_param[30];
	unsigned int scan_pass_acack[30];

	get_sdc_phy_addr(dram_id, &SDC_BASE_GRP, &PHY_BASE_GRP);

	memset((UINT8 *)scan_pass_param, 0, sizeof(scan_pass_param));
	memset((UINT8 *)scan_pass_acack, 0, sizeof(scan_pass_acack));

	for (dfi_tphy_wrdata = (UMCTL2_190_5 - 0); dfi_tphy_wrdata <= (UMCTL2_190_5 + 0); dfi_tphy_wrdata++) {
		for (dfi_t_rddata_en = (UMCTL2_190_3 - 0); dfi_t_rddata_en <= (UMCTL2_190_3 + 0); dfi_t_rddata_en++) {
			// UMCTL2_REG(0x190),
			//	[22:16] dfi_t_rddata_en
			//	[14: 8] dfi_tphy_wrdata
			scan_val_190 = UMCTL2_190 & (~((0x7F << 16) | (0x7F << 8)));
			scan_val_190 |= (dfi_t_rddata_en << 16) | (dfi_tphy_wrdata << 8);

			ckobd_training_flag = 1;
			ret = silent_dram_init();
			mpb = mp;
			mp = 0;
			prn_string("\nSCAN=>");
			prn_string("  dfi_t_rddata_en: ");
			prn_decimal(dfi_t_rddata_en);
			prn_string("  dfi_tphy_wrdata: ");
			prn_decimal(dfi_tphy_wrdata);
			prn_string("\n");
			mp = mpb;
			if (ret != SUCCESS) {
				continue;
			}

			dram_fill_zero(TEST_LEN_0, dram_id);
			cpu_test_result = memory_rw_test(dram_base_addr[0], TEST_LEN_0, MEMORY_RW_FLAG_EXIT);
			if (cpu_test_result == 0) {
				for (idx = 0; idx <= SCAN_TRIM_LEN; idx++) {
					trim_test_result = SDCTRL_TRIMMER_TEST(dram_id, dram_base_addr[0], 0x0100);
					if (trim_test_result) {
						if (idx == SCAN_TRIM_LEN) {
							mpb = mp;
							mp = 0;
							prn_string("\tSCAN=> Test Pass\n");
							scan_pass_param[rec_idx] = UMCTL2_REG(0x0190);
							scan_pass_acack[rec_idx] = SP_REG(PHY_BASE_GRP + 0, 17);
							rec_idx++;
							mp = mpb;
						}
					} else {
						prn_string("\tSCAN=> Test Fail\n");
						break;
					}
				}
			} else {
				prn_string("\tSCAN=> Test Fail\n");
			}
		}
	}

	mpb = mp;
	mp = 0;
	prn_string("\n\n==================================================================================\n");
	prn_string("DUMP DRAM-");
	prn_decimal(dram_id);
	prn_string("parameters:\n");
	for (idx = 0; idx < rec_idx; idx++) {
		if (scan_pass_param[idx] != 0) {
			prn_string("SCAN=> [");
			prn_decimal(idx);
			prn_string("]");
			prn_string(" ; dfi_t_rddata_en = ");
			prn_decimal((scan_pass_param[idx] >> 16) & 0x7F);
			prn_string(" ; dfi_tphy_wrdata = ");
			prn_decimal((scan_pass_param[idx] >> 8) & 0x7F);
			prn_string("; AC=");
			prn_decimal((scan_pass_acack[idx] >> 8) & 0x3F);
			prn_string("; ACK=");
			prn_decimal((scan_pass_acack[idx] >> 16) & 0x3F);
			prn_string("; CK=");
			prn_decimal((scan_pass_acack[idx] >> 0) & 0x3F);
			prn_string(";\n");
		}
	}
	prn_string("==================================================================================\n");
	mp = mpb;
}

void run_SiScope(void)
{
	prn_string("\n\n==================================run_SiScope START================================================\n");
	gAC = DPCU_AC0BD;
	gACK = DPCU_ACK0BD;
	gCK = DPCU_CK0BD;

	dram_scan(0);
	ddr_clk_info();

	prn_string("\n\n==================================run_SiScope END================================================\n");
	scan_val_190 = UMCTL2_190;
	silent_dram_init();
}

int getSquare(int base, int square)
{
	int value = 1;
	int i = 0;

	if (0 >= square) {
		// not support minus square
		value  = 1;
	} else {
		for (i = 1; i <= square; i++)
			value *= base;
	}
	return value;
}

int get_int(int len, int start, char *str)
{
	int i = 0, rate = 0;
	int square = 0;

	for (i = len - 1; i >= start; --i) {
		rate = rate + ((str[i] - 48) * getSquare(10, square++));
	}
	return rate;
}

void check_run_siscope()
{
	char input = '\0';

	do {
		prn_string("\n\n*Would you like to run SISCOPE?(y/n)\n");
		input = sp_getChar();
		if ('y' == input || 'Y' == input) {
			run_SiScope();
		} else if ('n' == input || 'N' == input) {
			break;
		}
	} while (0);
}
#endif	/* (defined(DRAMSCAN) || defined(SISCOPE)) */

int dram_init_main(unsigned int gbootRom_boot_mode)
{
	unsigned int temp_value = 0;
#ifdef CHIP_WARM_RESET
	unsigned int cnt;
#endif
	bootdevice=gbootRom_boot_mode;
	prn_string("bootdevice=");
	prn_dword(bootdevice);
	// init params
	ckobd_training_flag = 1;
	gAC = DPCU_AC0BD;
	gACK = DPCU_ACK0BD;
	gCK = DPCU_CK0BD;

#if !(defined(DRAMSCAN) || defined(SISCOPE))
#ifdef CHIP_WARM_RESET
	if (((SP_REG(98, 1) & 0x20) == 0) && ((SP_REG(98, 2) &0x01) == 0)) {
		SP_REG(98, 1) |= (1 << 5); // G98.01[5] = 1(default = 0)
		SP_REG(98, 2) |= (1 << 0); // G98.03[0] = 1(default = 0)
		/* System reset */
		SP_REG(0, 21) = 0x00010001; //G0.21[16] = 1; G0, 21[0] = 1
	} else {
		SP_REG(98, 1) &= ~(1 << 5); // restore G98.01[5] = 0(default = 0)
		SP_REG(98, 2) &= ~(1 << 0); // restore G98.03[0] = 0(default = 0)
	}

	/* Monitor Chip temperature */
	SP_REG(5, 0) = 0xFFFF3101;
	SP_REG(5, 1) = 0xFFFF0000;
	SP_REG(5, 2) = 0xFFFF4007;
	SP_REG(5, 3) = 0xFFFF0022;
	for (cnt = 0; cnt < 0x400000; cnt++) {
		temp_value = SP_REG(5, 12);
		if (temp_value <= 1530) { // 1530 is experimental value
			break;
		}
	}
#endif
#ifdef SDRAM_WATCHDOG
	SP_REG(4, 29) = (0x00120000 | ((1 << 4) | (1 << 1)));
	/* STC Watch dog control */
	SP_REG(12, 12) = WATCHDOG_CMD_PAUSE;
	SP_REG(12, 12) = WATCHDOG_CMD_CNT_WR_UNLOCK;
	SP_REG(12, 13) = 0x1000; // time count
	SP_REG(12, 12) = WATCHDOG_CMD_RESUME;
#endif
#ifdef DRAM_INIT_DEBUG
	mp = 0;
	prn_string("Built at " __DATE__ " " __TIME__ "\n");
#endif
	dram_init(0);

#ifdef SDRAM_WATCHDOG
	SP_REG(4, 29) = 0x00120000; // Stop watch dog feature
#endif

#else
	prn_string("Built at " __DATE__ " " __TIME__ "\n");

	do {
		check_run_siscope();
	} while (1);
	while (1) {;}
#endif


#ifdef SDRAM_MPSD_VT_ON
	// SET MPSD_VT THREADHOLD
	rgst_value = SP_REG(DRAM_0_PHY_REG_BASE + 1, 8);
	rgst_value = rgst_value & ~(0x0FF0);
	rgst_value = rgst_value | (0xFF << 4);
	SP_REG(DRAM_0_PHY_REG_BASE + 1, 8) = rgst_value;

	// Number of MPSD_VT average
	rgst_value = SP_REG(DRAM_0_PHY_REG_BASE + 1, 9);
	rgst_value = rgst_value & ~(0x03);
	rgst_value = rgst_value | 0x03;
	SP_REG(DRAM_0_PHY_REG_BASE + 1, 9) = rgst_value;

	// MPSD_VT ON
	rgst_value = SP_REG(DRAM_0_PHY_REG_BASE + 2, 19) | 0x01;
	SP_REG(DRAM_0_PHY_REG_BASE + 2, 19) = rgst_value;

	// MPSD_VT ON
	rgst_value = SP_REG(DRAM_0_PHY_REG_BASE + 3, 19) | 0x01;
	SP_REG(DRAM_0_PHY_REG_BASE + 3, 19) = rgst_value;
#endif

#ifdef SDRAM_RDQSG_VT_ON
	// RGST_VT_DYN_DET_OFFSET
	temp_value = (((SP_REG(DRAM_0_PHY_REG_BASE + 2, 3) >> 8) & 0xFF) + ((SP_REG(DRAM_0_PHY_REG_BASE + 3, 3) >> 8) & 0xFF)) / 16; // avg, 1/16
	rgst_value = SP_REG(DRAM_0_PHY_REG_BASE + 1, 8);
	rgst_value = rgst_value & ~(0xFF << 12);
#ifdef MPEG_DRAM_DDR_1600
	temp_value = 1;
#else
	if (temp_value == 0)
		temp_value = 1;
#endif
	rgst_value = rgst_value | (temp_value << 12);
	SP_REG(DRAM_0_PHY_REG_BASE + 1, 8) = rgst_value;

	// DX0 RDQSG_VT
	rgst_value = SP_REG(DRAM_0_PHY_REG_BASE + 2, 19) | 0x02;
	SP_REG(DRAM_0_PHY_REG_BASE + 2, 19) = rgst_value;

	// DX1 RDQSG_VT
	rgst_value = SP_REG(DRAM_0_PHY_REG_BASE + 3, 19) | 0x02;
	SP_REG(DRAM_0_PHY_REG_BASE + 3, 19) = rgst_value;
#endif

#ifdef SSC_ENABLE
	SP_REG(DRAM_0_PHY_REG_BASE, 12) = SP_REG(DRAM_0_PHY_REG_BASE, 12) & ~(0x60000F00);
	SP_REG(DRAM_0_PHY_REG_BASE, 12) = SP_REG(DRAM_0_PHY_REG_BASE, 12) | (DPCU_RI_MPLL_DIV_S << 8) | (SSC_RATE << 29);
	SP_REG(DRAM_0_PHY_REG_BASE, 11) = SP_REG(DRAM_0_PHY_REG_BASE, 11) | (1 << 2);
	prn_string("DDR PLL SETTING:");
	prn_dword(SSC_RATE);
	prn_string("\n ");
#endif
	return 1;
}
