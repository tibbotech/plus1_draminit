/*
 * C code entry: dram_init_main();
 */

#include <types.h>
#include <common.h>
#include <config.h>
#include <dram_param.h>
//#include <dwc_ddrphy_phyinit.h>

#if 1
#define SPI_FLASH_BASE      0x98000000
#define SPI_XBOOT_OFFSET    (64 * 1024)
typedef unsigned int        u32;

struct xboot_hdr {
	u32 magic;
	u32 version;
	u32 length;       // exclude header
	u32 checksum;     // exclude header
	u32 img_flag;
	u32 reserved[3];
};
#endif 


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

struct umctl2_regs {
	unsigned int umctl2_reg[1024];	/* change the size here, (area >> 2) */
};
static volatile struct umctl2_regs *umctl2_reg_ptr = (volatile struct umctl2_regs *)(UMCTL2_REG_Base);
#define UMCTL2_REG(OFFSET)		(umctl2_reg_ptr->umctl2_reg[OFFSET >> 2])

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
#ifdef PLATFORM_PENTAGRAM
static const unsigned int dram_base_addr[] = {0, SDRAM0_SIZE};
#else
static const unsigned int dram_base_addr[] = {0x20000000, SDRAM0_SIZE};
#endif
//static const unsigned int dram_size[] = {SDRAM0_SIZE, SDRAM1_SIZE};

#define DRAM_0_SDC_REG_BASE	33
#define DRAM_0_PHY_REG_BASE	50
#define DRAM_1_SDC_REG_BASE	0	/* N/A */
#define DRAM_1_PHY_REG_BASE	0	/* N/A */

#define SCAN_TRIM_LEN		5

static unsigned int rgst_value = 0;
static unsigned int aphy_select_value = 0;
static unsigned int ckobd_training_flag = 0;
static unsigned int ckobd_re_training_number = 0;

//static unsigned int data_byte_0_RDQSG_left_total_tap = 0;
//static unsigned int data_byte_0_RDQSG_right_total_tap = 0;
//static unsigned int data_byte_1_RDQSG_left_total_tap = 0;
//static unsigned int data_byte_1_RDQSG_right_total_tap = 0;
static unsigned int gAC, gACK, gCK;

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


void assert_sdc_phy_reset(void)
{
#ifdef PLATFORM_PENTAGRAM
	SP_REG(0, 21) = RF_MASK_V_SET(1 << 14);	// SDCTRL0
	SP_REG(0, 22) = RF_MASK_V_SET(1 << 0);	// PHY
#elif defined(PLATFORM_I143)
	SP_REG(0, 21) = RF_MASK_V_SET(1 << 10);	// UMCTL2
	SP_REG(0, 21) = RF_MASK_V_SET(1 << 14);	// SDCTRL0
	SP_REG(0, 22) = RF_MASK_V_SET(1 << 0);	// PHY
#endif
}

void release_sdc_phy_reset(void)
{
#ifdef PLATFORM_PENTAGRAM
	SP_REG(0, 21) = RF_MASK_V_CLR(1 << 14);	// SDCTRL0
	SP_REG(0, 22) = RF_MASK_V_CLR(1 << 0);	// PHY
#elif defined(PLATFORM_I143)
	SP_REG(0, 21) = RF_MASK_V_CLR(1 << 10);	// UMCTL2
	SP_REG(0, 21) = RF_MASK_V_CLR(1 << 14);	// SDCTRL0
	SP_REG(0, 22) = RF_MASK_V_CLR(1 << 0);	// PHY
#endif
}
// ***********************************************************************
// * FUNC      : do_system_reset_flow
// * PARAM     : dram_id
// * PURPOSE   : do SDC & PHY reset flow
// ***********************************************************************
void do_system_reset_flow(unsigned int dram_id)
{
	assert_sdc_phy_reset();
	wait_loop(1000);
	release_sdc_phy_reset();
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

// ***********************************************************************
// * FUNC      : SDCTRL_TRIMMER_TEST
// * PARAM     : dram_id
// * PURPOSE   : trigger SDC.trimmer 4 modes to do random DRAM access test
// ***********************************************************************
int SDCTRL_TRIMMER_TEST(unsigned int dram_id, unsigned int start_addr, unsigned int TEST_DATA_LENGTH)
{
	// H/W trimmer has beem removed.
	// Just run memory test.

	return ((memory_rw_test(start_addr, TEST_DATA_LENGTH, MEMORY_RW_FLAG_EXIT) < 0) ? 0 : 1);
}

// ***********************************************************************
// * FUNC      : DPCU_CMD_ISSUE_SW_CMD
// * PARAM     : CMD, RANK,BANK,ADDR, DATA_MASK, DATA, TRIGGER
// * PURPOSE   : using CMD ISSUE issue
// ***********************************************************************
void DPCU_CMD_ISSUE_SW_CMD(unsigned int dram_id, unsigned int CMD, unsigned int RANK, unsigned int BANK, unsigned int ADDR,
			   unsigned int SW_WRDATA_MASK, unsigned int SW_WRDATA1_HIGH, unsigned int SW_WRDATA1_LOW,
			   unsigned int SW_WRDATA0_HIGH, unsigned int SW_WRDATA0_LOW, unsigned int CMD_TRIGGER)
{
	unsigned int        temp;
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0;
	get_sdc_phy_addr(dram_id, &SDC_BASE_GRP, &PHY_BASE_GRP);

	temp = SP_REG(PHY_BASE_GRP + 1, 27) & 0xF0000000;
	// fill cmd, RANK, BANK, ADDR infor
	SP_REG(PHY_BASE_GRP + 1, 27) = temp | (CMD << 24) | (RANK << 20) | (BANK << 16) | (ADDR);
	// fill WRdata mask
	SP_REG(PHY_BASE_GRP + 2, 21) = SW_WRDATA_MASK & 0xFF;
	SP_REG(PHY_BASE_GRP + 3, 21) = (SW_WRDATA_MASK >> 8) & 0xFF;
	// fill WRDATA
	SP_REG(PHY_BASE_GRP + 2, 22) = SW_WRDATA0_LOW;
	SP_REG(PHY_BASE_GRP + 2, 23) = SW_WRDATA0_HIGH;
	SP_REG(PHY_BASE_GRP + 3, 22) = SW_WRDATA1_LOW;
	SP_REG(PHY_BASE_GRP + 3, 23) = SW_WRDATA1_HIGH;
	// Commit the cmd setting to DPCU CMD CUE
	temp = SP_REG(PHY_BASE_GRP + 1, 10);
	wait_loop(10);
	SP_REG(PHY_BASE_GRP + 1, 10) = temp | (1 << 8); // commit
	wait_loop(10);
	// Trigger CMD if need
	temp = SP_REG(PHY_BASE_GRP + 1, 10);
	wait_loop(10);
	SP_REG(PHY_BASE_GRP + 1, 10) = temp | (CMD_TRIGGER << 9); // Trigger
}  // End    DPCU_CMD_ISSUE_SW_CMD

// ***********************************************************************
// * FUNC      : dram_booting_flow
// * PARAM     : dram_id
// * PURPOSE   : to do the following sequences
// *           : (1). DDR_APHY initial sequence (CTCAL->SSCPLL->PZQ)
// ***********************************************************************
int dram_booting_flow(unsigned int dram_id)
{
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0;
	unsigned int        wait_flag      = 0;	 // min
	unsigned int        aphy_select1_value = 0;
	unsigned int        aphy_select2_value = 0;
	prn_string(">>> enter dram_booting_flow for DRAM");
	prn_decimal(dram_id);
	prn_string("\n");
	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	get_sdc_phy_addr(dram_id, &SDC_BASE_GRP, &PHY_BASE_GRP);

#ifdef PLATFORM_PENTAGRAM
	// CBUS-MBUS Bridge setting
#ifdef CONFIG_DRAM_SIZE_USE_OTP
	DRAM_SIZE_FLAG = ((SP_REG(350,7) >> 16) & 0x3);
	if (DRAM_SIZE_FLAG == DRAM_SIZE_512Mb) {
		SP_REG(5, 6) = (0x000f << 16)  | (0 << 2) | (0 << 0);
	} else if (DRAM_SIZE_FLAG == DRAM_SIZE_1Gb) {
		SP_REG(5, 6) = (0x000f << 16)  | (0 << 2) | (0 << 0);
	} else if (DRAM_SIZE_FLAG == DRAM_SIZE_4Gb) {
		SP_REG(5, 6) = (0x000f << 16)  | (2 << 2) | (2 << 0);
	} else {
		DRAM_SIZE_FLAG = 0xFF;
		SP_REG(5, 6) = (0x000f << 16)  | (MO_SDRAM_B_SIZE << 2) | (MO_SDRAM_A_SIZE << 0);
	}
#else
	SP_REG(5, 6) = (0x000f << 16)  | (MO_SDRAM_B_SIZE << 2) | (MO_SDRAM_A_SIZE << 0);
#endif
#elif defined(PLATFORM_I143)
	SP_REG(5, 6) = (0x000f << 16)  | (MO_SDRAM_B_SIZE << 2) | (MO_SDRAM_A_SIZE << 0);
#endif

	// -------------------------------------------------------
	// 1. DPCU_APHY_INIT setting => a001
	// -------------------------------------------------------
#ifdef SDRAM_FPGA
	// There are no APHY circuit in FPGA platform, so bypass this flow
#else
	do_system_reset_flow(dram_id);
	dbg_stamp(0xA000);

	SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_DPCU);
	// set MPLL_DIV to operation freq.
	SP_REG(PHY_BASE_GRP + 0, 12) = MPLL_CFG1_DEF | MPLL_DIV(n_MPLL_DIV);
	// set MPLL_DIV to operation freq.
	SP_REG(PHY_BASE_GRP + 0, 3) = DPCU_INIT_TIMMER;
	// Set DDRIO CFG
	SP_REG(PHY_BASE_GRP + 0, 21) = DPCU_DDRIO_CFG3;
	// set PZQ to internal mode for Q571
#ifdef USING_INTERNAL_PZQ_CAL
	prn_string("\tDPCU initial : Using Internal PZQ!!\n");
	SP_REG(PHY_BASE_GRP + 0, 18) = DPCU_PZQ_CFG0 | PZQ_REGI_ZQ_INTR(n_PZQ_ZQ_INTR_EN);
#else
	prn_string("\tDPCU initial : Using External PZQ!!\n");
	SP_REG(PHY_BASE_GRP + 0, 18) = DPCU_PZQ_CFG0;
#endif
	// setting PZQ CFG1
	SP_REG(PHY_BASE_GRP + 0, 19) = DPCU_PZQ_CFG1;
	// setting AI CFG
#if (defined(PLATFORM_PENTAGRAM) || defined(PLATFORM_I143))
	SP_REG(PHY_BASE_GRP + 0, 14) = (SP_REG(PHY_BASE_GRP + 0, 14) & 0xFFFDFFFF) | 0x00020000;
#endif
	SP_REG(PHY_BASE_GRP + 0, 1) = DPCU_AI_CFG0_SELECT1;
	// enable APHY_INIT start
	SP_REG(PHY_BASE_GRP + 0, 1) = DPCU_AI_CFG0_SELECT1 | AI_INIT_START(n_AI_INIT_START_EN);

	// wait aphy init done
	wait_flag   =   0;
	do {
		wait_flag   =   SP_REG(PHY_BASE_GRP + 0, 2) & 0x00000001;
	} while ((wait_flag == 0));

	rgst_value = (SP_REG(PHY_BASE_GRP + 0, 2) >> 8) & 0x0F;

	aphy_select1_value = rgst_value;
	// Disable DDR IO PAD Retention flag
	// setting AI CFG
	SP_REG(PHY_BASE_GRP + 0, 1) = DPCU_AI_CFG0_SELECT2;
	// enable APHY_INIT start
	SP_REG(PHY_BASE_GRP + 0, 1) = DPCU_AI_CFG0_SELECT2 | AI_INIT_START(n_AI_INIT_START_EN);
	// wait aphy init done
	wait_flag   =   0;
	do {
		wait_flag   =   SP_REG(PHY_BASE_GRP + 0, 2) & 0x00000001;
	} while ((wait_flag == 0));

	rgst_value = (SP_REG(PHY_BASE_GRP + 0, 2) >> 8) & 0x0F;

	aphy_select2_value = rgst_value;
	aphy_select_value = (aphy_select1_value | aphy_select2_value);

#if (defined(PLATFORM_PENTAGRAM) || defined(PLATFORM_I143))
	SP_REG(PHY_BASE_GRP + 0, 0) = SP_REG(PHY_BASE_GRP + 0, 0) & 0xFFFFFFBF;
#endif

	if (rgst_value != 0) {
		prn_string("<<< leave dram_booting_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return 0;
	}
#endif
	prn_string("<<< leave dram_booting_flow for DRAM");
	prn_decimal(dram_id);
	prn_string("\n");
	return 1;
} // end of dram_booting_flow



//////////////////////////////////////////////////////
///////////////LPDDR4////////////////////////////////
//////////////////////////////////////////////////////

void SP_REG_W(unsigned int addr,int value)
{
    prn_dword(addr);
	prn_dword(value);
}

unsigned short SP_REG_R(unsigned int addr)
{
	unsigned short temp;
	temp = 0;
    prn_dword(addr);
	return temp;
}

#define IMEM_ADDR 0x50000
#define DMEM_ADDR 0x54000
#define mem_size 256
unsigned sum = 0;
unsigned short mem[mem_size];
void tcpsum(const unsigned char *buf, unsigned size, unsigned char flag )
{
	int i,j;
	/* Accumulate checksum */
	for (i = 0; i < size - 1; i += 2)
	{
		unsigned short word16 = *(unsigned short *) &buf[i];
		//prn_dword(word16);
		sum += word16;
		//prn_dword(sum);
		j = i/2;
		mem[j] = word16;
	}	

	if(flag == 0)
		return;
					
	/* Handle odd-sized case */
	if (size & 1)
	{
		unsigned short word16 = (unsigned char) buf[i];
		sum += word16;
	}

	/* Fold to get the ones-complement result */
	while (sum >> 16) sum = (sum & 0xFFFF)+(sum >> 16);
	//prn_dword(sum);

	/* Invert to get the negative in ones-complement arithmetic */
	sum = ~sum;
	
	//prn_dword(sum);
	
}

void LoadBinCode(unsigned char Train2D, unsigned int offset)
{
	struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET+offset);//xboot start addr
	//unsigned short *temp = (SPI_FLASH_BASE + SPI_XBOOT_OFFSET+offset+32);
	unsigned short i, j, addr, num0, num1;
	#define rsize 512
	#define IM1D_HDR_MAGIC   0x64316d69      
	#define DM1D_HDR_MAGIC   0x64316d64
	#define IM2D_HDR_MAGIC   0x64326d69      
	#define DM2D_HDR_MAGIC   0x64326d64

	//prn_dword(xhdr->magic);
	//prn_dword(xhdr->length);
	//prn_dword(xhdr->checksum);
    // checksum verify
    num0 = (xhdr->length/rsize);
	num1 = (xhdr->length%rsize);
	//prn_string("num0");
	//prn_dword(num0);
	//prn_string("num1");
	//prn_dword(num1);
	for(i=0;i<num0;i++)
	{
		for (addr=0;addr<mem_size;addr++) 
			mem[addr]=0;

		unsigned char *temp = (unsigned char*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET+offset+32+(rsize*i));
		tcpsum(temp, rsize, 0);
		for(j=0;j<256;j++)
		{
		    /*****write register *********/			
			/*****write register *********/
			//prn_string("addr");
			//prn_dword(IMEM_ADDR+ (256*i)+j);					
			//prn_string("value");
			//prn_dword(mem[j]);
		}			
	}

	for (addr=0;addr<mem_size;addr++) 
	{
		mem[addr]=0;
	}
	unsigned char *temp =  (unsigned char*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET+offset+32+(rsize*num0));
	tcpsum(temp, num1, 1);
	for(i=0;i<(num1/2);i++)
	{	
		/*****write register *********/
		/*****write register *********/
		//prn_string("addr");
		//prn_dword(IMEM_ADDR+ (256*num0)+i);					
		//prn_string("value");
		//prn_dword(mem[i]); //wirte dword
	}
    
	if((sum&0x0000FFFF) != xhdr->checksum)
	{
	    if (xhdr->magic == IM1D_HDR_MAGIC)
			prn_string("1d IMEM checksum error!!!!\n");
		else if (xhdr->magic == DM1D_HDR_MAGIC)
			prn_string("1d DMEM checksum error!!!!\n");
		else if (xhdr->magic == IM2D_HDR_MAGIC)
			prn_string("2d IMEM checksum error!!!!\n");
		else if (xhdr->magic == DM2D_HDR_MAGIC)
			prn_string("2d DMEM checksum error!!!!\n");

		prn_string("sum");
		prn_dword(sum);					
		prn_string("xhdr->checksum");
		prn_dword((xhdr->checksum)); //wirte dword
	}		
	else
	{
		if (xhdr->magic == IM1D_HDR_MAGIC)
			prn_string("1d IMEM checksum ok!!!!\n");
		else if (xhdr->magic == DM1D_HDR_MAGIC)
			prn_string("1d DMEM checksum ok!!!!\n");
		else if (xhdr->magic == IM2D_HDR_MAGIC)
			prn_string("2d IMEM checksum ok!!!!\n");
		else if (xhdr->magic == DM2D_HDR_MAGIC)
			prn_string("2d DMEM checksum ok!!!!\n");
		//prn_string("sum");
		//prn_dword(sum);					
		//prn_string("xhdr->checksum");
		//prn_dword((xhdr->checksum)); //wirte dword
	}
	sum = 0;
}

void dwc_ddrphy_phyinit_D_loadIMEM (int Train2D) {
    
    char *printf_header;
    if (Train2D)
        printf_header = "// [dwc_ddrphy_phyinit_D_loadIMEM, 2D]";
    else
        printf_header = "// [dwc_ddrphy_phyinit_D_loadIMEM, 1D]";
    
    dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_D_loadIMEM (Train2D=%d)\n", printf_header, Train2D);

    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("// (D) Load the %dD IMEM image\n", Train2D+1);
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("// This function loads the training firmware IMEM image into the SRAM.\n");
    dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");

	struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET);//xboot start addr
	unsigned int offset;
	offset = sizeof(struct xboot_hdr) + xhdr->length; //xboot length
	xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
	if(Train2D == 0)
		LoadBinCode(0,offset);
	offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d  length
	xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
	offset = offset + sizeof(struct xboot_hdr)	+ xhdr->length;//xboot+im1d+dm1d  length
	xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
	if(Train2D == 1)
		LoadBinCode(1,offset);
	offset = offset + sizeof(struct xboot_hdr)	+ xhdr->length;//xboot+im1d+dm1d+im2d  length
	xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
}

void dwc_ddrphy_phyinit_F_loadDMEM (int pstate, int Train2D) {

    
    char *printf_header;
    if (Train2D)
        printf_header = "// [phyinit_F_loadDMEM, 2D]";
    else
        printf_header = "// [phyinit_F_loadDMEM, 1D]";

    dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_F_loadDMEM (pstate=%d, Train2D=%d)\n", printf_header, pstate, Train2D);

    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("// (F) Load the %dD DMEM image and write the %dD Message Block parameters for the training firmware \n", Train2D+1, Train2D+1);
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("\n");

	struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET);//xboot start addr
	unsigned int offset;
	offset = sizeof(struct xboot_hdr) + xhdr->length; //xboot length
	xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
	offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d  length
	xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
	if(Train2D == 0)
			LoadBinCode(0,offset);	
	offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d+dm1d  length
	xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
	offset = offset + sizeof(struct xboot_hdr) + xhdr->length;//xboot+im1d+dm1d+im2d  length
	xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + offset);
	if(Train2D == 1)
		LoadBinCode(1,offset);
}

void dwc_ddrphy_phyinit_main(void)
{
   //#include <dwc_ddrphy_phyinit_out_lpddr4_train1d2d.txt>
   //#include <dwc_ddrphy_phyinit_out_lpddr4_skiptrain.txt>
   #include <dwc_ddrphy_phyinit_out_lpddr4_devinit_skiptrain.txt>
}

int dram_training_flow_for_ddr4(unsigned int dram_id)
{
	unsigned int SDC_BASE_GRP = 0, PHY_BASE_GRP = 0;
	char *printf_header;

	prn_string(">>> enter dram_training_flow_for_ddr4 for DRAM");
	prn_decimal(dram_id);
	prn_string("\n");

	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	dbg_stamp(0xA001);
	get_sdc_phy_addr(dram_id, &SDC_BASE_GRP, &PHY_BASE_GRP);
	
	// -------------------------------------------------------
	// 2. 
	// -------------------------------------------------------
	// DRAM MRS SETTING
	dbg_stamp(0xA002);
#if 0	
	UMCTL2_REG(0x0304) = 0x00000001;									
	UMCTL2_REG(0x0030) = 0x00000001;	//PWRCTL								
	UMCTL2_REG(0x0004); //STAT									
	UMCTL2_REG(0x0000) = 0x80080020;	//MSTR								
	UMCTL2_REG(0x0010) = 0x4000c010;	//MRCTRL0								
	UMCTL2_REG(0x0014) = 0x000209ce;	//MRCTRL1								
	UMCTL2_REG(0x001c) = 0xe7256067;	//MRCTRL2								
	UMCTL2_REG(0x0020) = 0x00001203;	//DERATEEN								
	UMCTL2_REG(0x0024) = 0x031cd8cf;	//DERATEINT								
	UMCTL2_REG(0x002c) = 0x00000000;	//DERATECTL								
	UMCTL2_REG(0x0030) = 0x00000120;	//PWRCTL								
	UMCTL2_REG(0x0034) = 0x00040600;	//PWRTMG								
	UMCTL2_REG(0x0038) = 0x00000000;	//HWLPCTL								
	UMCTL2_REG(0x0050) = 0x00a080f4;	//RFSHCTL0								
	UMCTL2_REG(0x0060) = 0x00000000;	//RFSHCTL3								
	UMCTL2_REG(0x0064) = 0x81860048;	//RFSHTMG								
	UMCTL2_REG(0x0068) = 0x00480000;	//RFSHTMG1								
	UMCTL2_REG(0x00c0) = 0x00000000;	//CRCPARCTL0								
	UMCTL2_REG(0x00c4) = 0x00001000;	//CRCPARCTL1								
	UMCTL2_REG(0x00d0) = 0xc0020003;	//INIT0								
	UMCTL2_REG(0x00d4) = 0x00010006;	//INIT1								
	UMCTL2_REG(0x00d8) = 0x00004505;	//INIT2								
	UMCTL2_REG(0x00dc) = 0x0054002d;	//INIT3								
	UMCTL2_REG(0x00e0) = 0x00f10028;	//INIT4								
	UMCTL2_REG(0x00e4) = 0x00040009;	//INIT5								
	UMCTL2_REG(0x00e8) = 0x0004004d;	//INIT6								
	UMCTL2_REG(0x00ec) = 0x0000004d;	//INIT7								
	UMCTL2_REG(0x00f0) = 0x00000000;	//DIMMCTL								
	UMCTL2_REG(0x0100) = 0x1b203622;	//DRAMTMG0								
	UMCTL2_REG(0x0104) = 0x00060630;	//DRAMTMG1								
	UMCTL2_REG(0x0108) = 0x07101717;	//DRAMTMG2								
	UMCTL2_REG(0x010c) = 0x00b0c006;	//DRAMTMG3								
	UMCTL2_REG(0x0110) = 0x0f04080f;	//DRAMTMG4								
	UMCTL2_REG(0x0114) = 0x09520c0c;	//DRAMTMG5								
	UMCTL2_REG(0x0118) = 0x09050007;	//DRAMTMG6								
	UMCTL2_REG(0x011c) = 0x0000080c;	//DRAMTMG7								
	UMCTL2_REG(0x0120) = 0x01016601;	//DRAMTMG8								
	UMCTL2_REG(0x0124) = 0x40000015;	//DRAMTMG9								
	UMCTL2_REG(0x0128) = 0x0009080b;	//DRAMTMG10								
	UMCTL2_REG(0x012c) = 0x01010005;	//DRAMTMG11								
	UMCTL2_REG(0x0130) = 0x00020000;	//DRAMTMG12								
	UMCTL2_REG(0x0134) = 0x0c100002;	//DRAMTMG13								
	UMCTL2_REG(0x0138) = 0x00000f6b;	//DRAMTMG14								
	UMCTL2_REG(0x013c) = 0x80000000;	//DRAMTMG15								
	UMCTL2_REG(0x0180) = 0x03200018;	//ZQCTL0								
	UMCTL2_REG(0x0184) = 0x02800070;	//ZQCTL1								
	UMCTL2_REG(0x0188) = 0x00000000;	//ZQCTL2								
							
										
	UMCTL2_REG(0x0190) = 0x039b820a;	//DFITMG0								
							
	UMCTL2_REG(0x0194) = 0x00090202;	//DFITMG1								
	UMCTL2_REG(0x0198) = 0x07f1a001;	//DFILPCFG0								
	UMCTL2_REG(0x019c) = 0x00000020;	//DFILPCFG1								
	UMCTL2_REG(0x01a0) = 0x20400018;	//DFIUPD0								
	UMCTL2_REG(0x01a4) = 0x000100fa;	//DFIUPD1								
	UMCTL2_REG(0x01a8) = 0x00000000;	//DFIUPD2								
	UMCTL2_REG(0x01b0) = 0x00000051;	//DFIMISC								
	UMCTL2_REG(0x01b4) = 0x00001b0a;	//DFITMG2								
	UMCTL2_REG(0x01b8) = 0x0000001d;	//DFITMG3								
	UMCTL2_REG(0x01c0) = 0x00000006;	//DBICTL								
	UMCTL2_REG(0x01c4) = 0x00000001;	//DFIPHYMSTR)								
	UMCTL2_REG(0x0204) = 0x00121416;	//ADDRMAP1								
	UMCTL2_REG(0x0208) = 0x00000000;	//ADDRMAP2								
	UMCTL2_REG(0x020c) = 0x02010000;	//ADDRMAP3								
	UMCTL2_REG(0x0210) = 0x00001f1f;	//ADDRMAP4								
	UMCTL2_REG(0x0214) = 0x08040302;	//ADDRMAP5								
								
										
	UMCTL2_REG(0x0218) = 0x0f060209;	//ADDRMAP6								
	UMCTL2_REG(0x021c) = 0x00000f0f;	//ADDRMAP7								
	UMCTL2_REG(0x0220) = 0x00000000;	//ADDRMAP8								
	UMCTL2_REG(0x0224) = 0x09070904;	//ADDRMAP9								
	UMCTL2_REG(0x0228) = 0x0b030901;	//ADDRMAP10								
	UMCTL2_REG(0x022c) = 0x00000009;	//ADDRMAP11								
	UMCTL2_REG(0x0240) = 0x0e1d0608;	//ODTCFG								
	UMCTL2_REG(0x0244) = 0x00000000;	//ODTMAP								
	UMCTL2_REG(0x0250) = 0x48f79a06;	//SCHED								
	UMCTL2_REG(0x0254) = 0x00000030;	//SCHED1								
	UMCTL2_REG(0x025c) = 0x0d0021ee;	//PERFHPR1								
	UMCTL2_REG(0x0264) = 0x6100a3f5;	//PERFLPR1								
	UMCTL2_REG(0x026c) = 0x4200594c;	//PERFWR1								
	UMCTL2_REG(0x0300) = 0x00000000;	//DBG0								
	UMCTL2_REG(0x0304) = 0x00000000;	//DBG1								
	UMCTL2_REG(0x030c) = 0x00000000;	//DBGCMD								
	UMCTL2_REG(0x0320) = 0x00000001;	//SWCTL								
	UMCTL2_REG(0x0328) = 0x00000000;	//SWCTLSTATIC								
	UMCTL2_REG(0x036c) = 0x00010000;	//POISONCFG								
	UMCTL2_REG(0x0400) = 0x00000110;	//PCCFG								
	UMCTL2_REG(0x0404) = 0x000001df;	//PCFGR_0								
	UMCTL2_REG(0x0408) = 0x00006069;	//PCFGW_0								
	UMCTL2_REG(0x0490) = 0x00000001;	//PCTRL_0								
	UMCTL2_REG(0x0494) = 0x00210002;	//PCFGQOS0_0								
	UMCTL2_REG(0x0498) = 0x00e702d6;	//PCFGQOS1_0								
	UMCTL2_REG(0x049c) = 0x01100c01;	//PCFGWQOS0_0								
	UMCTL2_REG(0x04a0) = 0x02af028c;	//PCFGWQOS1_0								
													
	UMCTL2_REG(0x0060);									
	UMCTL2_REG(0x0030);		//PWRCTL							
	UMCTL2_REG(0x0030) = 0x00000120;//PWRCTL									
										
	//SP_REG([     3133125] RESET: <aresetn> for Port 0  DEASSERTED									
	//SP_REG([     3133125] RESET: <core_ddrc_rstn> DEASSERTED									
													
	UMCTL2_REG(0x0304) = 0x00000000; //DBG1									
	UMCTL2_REG(0x0030); //PWRCTL									
	UMCTL2_REG(0x0030) = 0x00000120; //PWRCTL									
	UMCTL2_REG(0x0030); //PWRCTL									
	UMCTL2_REG(0x0030) = 0x00000120; //PWRCTL									
	UMCTL2_REG(0x01c4); //DFIPHYMSTR									
	UMCTL2_REG(0x01c4) = 0x00000000; //DFIPHYMSTR									
	UMCTL2_REG(0x0320) = 0x00000000; //SWCTL									
	UMCTL2_REG(0x01b0) = 0x00000050; //DFIMISC									
	UMCTL2_REG(0x01b0) = 0x00000050; //DFIMISC									
	UMCTL2_REG(0x0304) = 0x00000002; //DBG1									
						
									
	UMCTL2_REG(0x00d0);//INIT0									
	UMCTL2_REG(0x01c0);//DBICTL 'h00000006 									
	UMCTL2_REG(0x0000);//MSTR 'h80080020 									
	UMCTL2_REG(0x0000);//MSTR 'h80080020 									
	UMCTL2_REG(0x00dc);//INIT3 'h0054002d 									
										
	UMCTL2_REG(0x00e0);//INIT4 'h00f10028 									
	UMCTL2_REG(0x00e8);//INIT6 'h0004004d 									
	UMCTL2_REG(0x00e8);//INIT6 'h0004004d 									
	UMCTL2_REG(0x00e0);//INIT4 'h00f10028  									
	UMCTL2_REG(0x00ec);//INIT7 'h0000004d  									
	UMCTL2_REG(0x00ec);//INIT7 'h0000004d  									
	UMCTL2_REG(0x00d0);//INIT0 'hc0020003  									
	UMCTL2_REG(0x01c0);//DBICTL 'h00000006  									
	UMCTL2_REG(0x0000);//MSTR 'h80080020  									
	UMCTL2_REG(0x0000);//MSTR 'h80080020  									
	UMCTL2_REG(0x00dc);//INIT3 'h0054002d  									
	UMCTL2_REG(0x00dc);//INIT3 'h0054002d  									
	UMCTL2_REG(0x00e0);//INIT4 'h00f10028  									
	UMCTL2_REG(0x00e8);//INIT6 'h0004004d  									
	UMCTL2_REG(0x00e8);//INIT6 'h0004004d  									
	UMCTL2_REG(0x00e0);//INIT4 'h00f10028  									
	UMCTL2_REG(0x00ec);//INIT7 'h0000004d  									
	UMCTL2_REG(0x00ec);//INIT7 'h0000004d  									
	UMCTL2_REG(0x00d0);//INIT0 'hc0020003  									
#endif							
									

	// -------------------------------------------------------
	// 3. 
	// -------------------------------------------------------
	printf_header = "// [dwc_ddrphy_phyinit_sequence]";
	dwc_ddrphy_phyinit_print ("%s Start of dwc_ddrphy_phyinit_sequence()\n", printf_header);
	dbg_stamp(0xA003);
	dwc_ddrphy_phyinit_main();

	if (SP_REG(50, 0) != 0x00) {
		prn_string("<<< TEST ");
		prn_decimal(dram_id);
		prn_string("\n");
	}
    prn_string("<<< leave dram_training_flow_for_ddr4 for DRAM");
	prn_decimal(dram_id);
	prn_string("\n");
	return 1;
}



// ***********************************************************************
// * FUNC      : dram_init
// * PARAM     : dram_id
// * PURPOSE   : to do the following sequences
// *           : (1). DDR_APHY initial sequence (CTCAL->SSCPLL->PZQ)
// *           : (2). SDCTRL RGST setting
// *           : (3). DRAM initial setting by SDCTRL
// *           : (4). DDR_DPHY+APHY data training
// ***********************************************************************
int dram_init(unsigned int dram_id)
{
	unsigned int        SDC_BASE_GRP = 0, PHY_BASE_GRP = 0;
	unsigned int        max_init_fail_cnt = 15;
	unsigned int        loop_time;
	unsigned int        ret = 0;
	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	get_sdc_phy_addr(dram_id, &SDC_BASE_GRP, &PHY_BASE_GRP);

	loop_time = 0;
	// ckobd_training_flag = 0;
	ckobd_re_training_number = 0;
	prn_string(" dram_init\n");

	// 20140728 mazar : add max_init_fail_cnt for sometime training fail
	for (loop_time = 0; loop_time < max_init_fail_cnt; loop_time++) {
		ckobd_re_training_number = loop_time;

		// dram_bootint_flow pass return 1, error return 0;
		// 20140727 mazar : do not add any action if we encounter APHY INIT ERR, just dump error flag
DRAM_BOOT_FLOW_AGAIN:
		if (!dram_booting_flow(dram_id)) {
			// error check flow
			// 1. dram_id = 1 and all initial error flag are asserted (note)
			// => we consider this is 216 package, so package_256_flag = 0;
			// note : SSCPLL isn't assert, so check PZQ, CTCAL, DDL error flag
			// and pass all DRAM 1 training flow
			// 2. dram_id = 1 and not all initial error flag are asserted => we consider this is 256 package, so package_256_flag = 1;
			// and print the error message
			// 3. dram_id = 0, print the error message
			prn_string("DPCU_INFO : \t********** DUMP APHY INIT************************\n");
			prn_string("aphy_select_value =");
			prn_dword(aphy_select_value);
			if ((dram_id == 1) && (aphy_select_value == 0x0D)) {
				// case 1, we think this is 216 pin package, and don't need to dump initial error message
				// only check PZQ, CTCAL, DDL error flag
				//package_256_flag = 0;
				// prn_string("is this 216 package !?\n");
			} else {
				// case 2 or case 3, we dump the initial error flag
				// DUMP INIT flag
				prn_string("DPCU_INFO : \t********** DUMP APHY INIT error infor @ loop_time = ");
				prn_decimal(loop_time);
				prn_string(" ***\n");
				//prn_string("\tCTCAL_ERR flag =");
				//prn_decimal((unsigned int)((SP_REG(PHY_BASE_GRP, 2) >> 8) & 0x01));
				//prn_string("\tSSCPLL_ERR flag =");
				//prn_decimal((unsigned int)((SP_REG(PHY_BASE_GRP, 2) >> 9) & 0x01));
				//prn_string("\n");
				//prn_string("\tDDL_ERR flag =");
				//prn_decimal((unsigned int)((SP_REG(PHY_BASE_GRP, 2) >> 10) & 0x01));
				//prn_string("\tPZQ_ERR flag =");
				//prn_decimal((unsigned int)((SP_REG(PHY_BASE_GRP, 2) >> 11) & 0x01));
				//prn_string("\n");
				prn_string("DPCU_INFO : \t********** DUMP APHY INIT error information end **********\n");
				goto DRAM_BOOT_FLOW_AGAIN;
			}
		} else {
			// prn_string("DRAM-"); prn_decimal(dram_id); prn_string("booting PASS @ loop_time =");
			// prn_decimal(loop_time); prn_string("!!\n");
		}	

		prn_string("GRP(PHY_BASE_GRP, 9) =  ");
		prn_dword(SP_REG(PHY_BASE_GRP, 9));
		prn_string("\n ");
		
		ret = dram_training_flow_for_ddr4(dram_id);
		
		if (ret == WAIT_FLAG_FAIL) {
			prn_string("wait flag or register G(37,10) fail!!!!\n");
			// goto DRAM_BOOT_FLOW_AGAIN;
			return FAIL;
		} else if (ret == 0) {
			// (dram_id=0) or (dram_id=1 and package_256_flag==1) => do training flow
			// dump error message

			prn_string("DPCU_INFO : \t********** DUMP init & training error info @ loop_time = ");
			prn_decimal(loop_time);
			prn_string(" ***\n");
			//DPCU_DT_RESULT_DUMP(dram_id);
		}

		prn_string("lpddr4_test_01\n");
		return SUCCESS;
		
		if (loop_time + 1 == max_init_fail_cnt) { // robert fix
			prn_string("DRAM-");
			prn_decimal(dram_id);
			prn_string("initial failed\n\n");
			// while (1); // robert: return fail rather than hang
			return FAIL;
		} // all loop training fail
		 
	} // end of for loop :: loop_time for initial & training time control

	prn_string("DRAM-");
	prn_decimal(dram_id);
	prn_string("initial done !!!!\n\n");

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

int dram_init_main()
{
	unsigned int temp_value = 0;
#ifdef CHIP_WARM_RESET
	unsigned int cnt;
#endif

	// init params
	ckobd_training_flag = 1;
	gAC = DPCU_AC0BD;
	gACK = DPCU_ACK0BD;
	gCK = DPCU_CK0BD;
	prn_string("3333\n");

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
