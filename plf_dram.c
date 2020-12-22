/*
 * C code entry: dram_init_main();
 */

#include <types.h>
#include <common.h>
#include <config.h>
#include <dram_param.h>

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

static unsigned int data_byte_0_RDQSG_left_total_tap = 0;
static unsigned int data_byte_0_RDQSG_right_total_tap = 0;
static unsigned int data_byte_1_RDQSG_left_total_tap = 0;
static unsigned int data_byte_1_RDQSG_right_total_tap = 0;
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
#define SDRAM_WATCHDOG
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



// ***********************************************************************
// * FUNC      : DPCU_DT_RESULT_DUMP
// * PARAM     : dram_id
// * PURPOSE   : Dump DPCU Training information
// ***********************************************************************
void DPCU_DT_RESULT_DUMP(unsigned int dram_id)
{
	unsigned int SDC_BASE_GRP       = 0;
	unsigned int PHY_BASE_GRP       = 0;
	unsigned int temp_a             = 0;
	unsigned int temp_b             = 0;
	unsigned int temp_c             = 0;
	unsigned int only_dump_PSD      = 0;
	unsigned int RDQS_IPRD_TAP_NO         = 0;

	if (dram_id == 0) {
		prn_string("DPCU_DT_INFO : ----- DUMP DRAM-0 delay line status -----\n\n");
	} else {
		prn_string("DPCU_DT_INFO : ----- DUMP DRAM-1 delay line status -----\n\n");
	}

	// -------------------------------------------------------
	// 0. DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	get_sdc_phy_addr(dram_id, &SDC_BASE_GRP, &PHY_BASE_GRP);

	// DUMP SSCPLL Speed
	prn_string("DPCU_DT_INFO : \t********** DRAM SPEED **********\n");
	temp_a = (SP_REG(PHY_BASE_GRP, 12) >> 0) & 0x3F;
	prn_string("  SSCPLL Setting =");
	prn_byte(temp_a);

	// DUMP CK0BD
	prn_string("DPCU_DT_INFO : \t********** DDRPHY Setting **********\n");
	temp_a = (SP_REG(PHY_BASE_GRP, 17) >> 16) & 0x3F;
	temp_b = (SP_REG(PHY_BASE_GRP, 17) >>  8) & 0x3F;
	temp_c = (SP_REG(PHY_BASE_GRP, 17) >>  0) & 0x3F;

	prn_string("  ACK0BD=");
	prn_decimal(temp_a);
	prn_string("  AC0BD=");
	prn_decimal(temp_b);
	prn_string("  CK0BD=");
	prn_decimal(temp_c);
	prn_string("  \n\n");

	// DUMP INIT & Training flag
	temp_a = SP_REG(PHY_BASE_GRP, 2) & 0x03;
	temp_b = temp_a & 0x01;
	temp_c = (temp_a >> 1) & 0x01;
	prn_string("DPCU_DT_INFO : \t********** DUMP APHY INIT flag **********\n");
	prn_string("  Init  done flag =");
	prn_decimal(temp_b);
	prn_string("(0 : means don't init, 1 : means init  done) \n");
	prn_string("  Init error flag =");
	prn_decimal(temp_c);
	prn_string("(0 : means  init pass, 1 : means init error) \n\n");

	// initial error
	if (temp_c == 1) {
		prn_string("DPCU_DT_INFO : \t********** DUMP APHY INIT error information **********\n");
		temp_a = (SP_REG(PHY_BASE_GRP, 2) >>  8) & 0x01;
		temp_b = (SP_REG(PHY_BASE_GRP, 2) >>  9) & 0x01;
		prn_string("\tCTCAL_ERR flag =");
		prn_decimal(temp_a);
		prn_string("\tSSCPLL_ERR flag =");
		prn_decimal(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP, 2) >> 10) & 0x01;
		temp_b = (SP_REG(PHY_BASE_GRP, 2) >> 11) & 0x01;
		prn_string("\tDDL_ERR flag =");
		prn_decimal(temp_a);
		prn_string("\tPZQ_ERR flag =");
		prn_decimal(temp_b);
		prn_string("\n");
	}

	temp_a = SP_REG(PHY_BASE_GRP + 1, 0) & 0x01;
	temp_b = (SP_REG(PHY_BASE_GRP + 1, 0) >> 8) & 0x3F;

	prn_string("DPCU_DT_INFO : \t********** DUMP Training flag **********\n");
	prn_string("  Training  done flag =   ");
	prn_decimal(temp_a);
	prn_string("(0:don't train,     1 : means training  done) \n");
	prn_string("  Training error flag =");
	prn_byte(temp_b);
	prn_string("(0:train  pass, other : means training error) \n\n");

	// training error
	if (temp_b != 0) {
		prn_string("DPCU_DT_INFO : \t********** DUMP APHY DX0 training error information **********\n");
		temp_a = (SP_REG(PHY_BASE_GRP + 2, 2) >> 8) & 0x01;
		temp_b = (SP_REG(PHY_BASE_GRP + 2, 2) >> 9) & 0x01;
		prn_string("\tWL_ERR flag =");
		prn_decimal(temp_a);
		prn_string("\tRG_ERR flag =");
		prn_decimal(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 2) >> 10) & 0x01;
		temp_b = (SP_REG(PHY_BASE_GRP + 2, 2) >> 11) & 0x01;
		prn_string("\tRDE_ERR flag =");
		prn_decimal(temp_a);
		prn_string("\tREYE_ERR flag =");
		prn_decimal(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 2) >> 12) & 0x01;
		temp_b = (SP_REG(PHY_BASE_GRP + 2, 2) >> 13) & 0x01;
		prn_string("\tWDE_ERR flag =");
		prn_decimal(temp_a);
		prn_string("\tWEYE_ERR flag =");
		prn_decimal(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 2) >> 16) & 0x0F;
		temp_b = (SP_REG(PHY_BASE_GRP + 2, 2) >> 20) & 0x0F;
		prn_string("\tWL_ERR infor =");
		prn_byte(temp_a);
		prn_string("\tRG_ERR infor =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 2) >> 24) & 0x0F;
		temp_b = (SP_REG(PHY_BASE_GRP + 2, 2) >> 28) & 0x0F;
		prn_string("\tREYE_ERR infor =");
		prn_byte(temp_a);
		prn_string("\tWEYE_ERR infor =");
		prn_byte(temp_b);
		prn_string("\n");

		prn_string("DPCU_DT_INFO : \t********** DUMP APHY DX1 training error information **********\n");
		temp_a = (SP_REG(PHY_BASE_GRP + 3, 2) >> 8) & 0x01;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 2) >> 9) & 0x01;
		prn_string("\tWL_ERR flag =");
		prn_decimal(temp_a);
		prn_string("\tRG_ERR flag =");
		prn_decimal(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 3, 2) >> 10) & 0x01;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 2) >> 11) & 0x01;
		prn_string("\tRDE_ERR flag =");
		prn_decimal(temp_a);
		prn_string("\tREYE_ERR flag =");
		prn_decimal(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 3, 2) >> 12) & 0x01;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 2) >> 13) & 0x01;
		prn_string("\tWDE_ERR flag =");
		prn_decimal(temp_a);
		prn_string("\tWEYE_ERR flag =");
		prn_decimal(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 3, 2) >> 16) & 0x0F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 2) >> 20) & 0x0F;
		prn_string("\tWL_ERR infor =");
		prn_byte(temp_a);
		prn_string("\tRG_ERR infor =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 3, 2) >> 24) & 0x0F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 2) >> 28) & 0x0F;
		prn_string("\tREYE_ERR infor =");
		prn_byte(temp_a);
		prn_string("\tWEYE_ERR infor =");
		prn_byte(temp_b);
		prn_string("\n");
	}

	// DUMP IPRD register
	prn_string("DPCU_DT_INFO : \t********** DUMP initial DDR period **********\n");
	prn_string("\t[DATx8-0]\t\t[DATx8-1]\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 3) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 3) >> 0) & 0xFF;
	prn_string("\tDX0 :   WL_IPRD =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   WL_IPRD =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 3) >> 8) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 3) >> 8) & 0xFF;
	prn_string("\tDX0 :   RG_IPRD =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   RG_IPRD =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 3) >> 16) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 3) >> 16) & 0xFF;
	prn_string("\tDX0 : RDQS_IPRD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : RDQS_IPRD =");
	prn_byte(temp_b);
	prn_string("\n");

	if (temp_a > temp_b) {
		RDQS_IPRD_TAP_NO = temp_b;
	} else {
		RDQS_IPRD_TAP_NO = temp_a;
	}

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 3) >> 24) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 3) >> 24) & 0xFF;
	prn_string("\tDX0 : WDQS_IPRD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : WDQS_IPRD =");
	prn_byte(temp_b);
	prn_string("\n\n");

	// DUMP PSD register
	prn_string("DPCU_DT_INFO : \t********** DUMP Training PSD status **********\n");
	prn_string("\t[DATx8-0]\t\t[DATx8-1]\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 4) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 4) >> 0) & 0xFF;
	prn_string("\tDX0 :   WL_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   WL_PSD =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 4) >> 8) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 4) >> 8) & 0xFF;
	prn_string("\tDX0 :   WL_SEL =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   WL_SEL =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 6) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 6) >> 0) & 0xFF;
	prn_string("\tDX0 : REYE_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : REYE_PSD =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 7) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 7) >> 0) & 0xFF;
	prn_string("\tDX0 : WEYE_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : WEYE_PSD =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 14) >> 0) & 0x1F;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 14) >> 0) & 0x1F;
	prn_string("\tDX0 :   RG_RSL =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   RG_RSL =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 14) >> 8) & 0x03;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 14) >> 8) & 0x03;
	prn_string("\tDX0 :   RG_PHA =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   RG_PHA =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 14) >> 16) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 14) >> 16) & 0xFF;
	prn_string("\tDX0 :   RG_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   RG_PSD =");
	prn_byte(temp_b);
	prn_string("\n\n");

	prn_string("DPCU_DT_INFO : \t********** DUMP RG L-side & R-side status **********\n");
	prn_string("\t[DATx8-0]\t\t[DATx8-1]\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 0) & 0x1F;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 0) & 0x1F;
	prn_string("\tDX0 : L_SIDE_RSL =");
	prn_byte(temp_a);
	prn_string("\tDX1 : L_SIDE_RSL =");
	prn_byte(temp_b);
	prn_string("\n");

	data_byte_0_RDQSG_left_total_tap = temp_a * RDQS_IPRD_TAP_NO;
	data_byte_1_RDQSG_left_total_tap = temp_b * RDQS_IPRD_TAP_NO;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 5) & 0x03;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 5) & 0x03;
	prn_string("\tDX0 : L_SIDE_PHA =");
	prn_byte(temp_a);
	prn_string("\tDX1 : L_SIDE_PHA =");
	prn_byte(temp_b);
	prn_string("\n");

	data_byte_0_RDQSG_left_total_tap = (temp_a * RDQS_IPRD_TAP_NO / 2) + data_byte_0_RDQSG_left_total_tap;
	data_byte_1_RDQSG_left_total_tap = (temp_b * RDQS_IPRD_TAP_NO / 2) + data_byte_1_RDQSG_left_total_tap;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 8) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 8) & 0xFF;
	prn_string("\tDX0 : L_SIDE_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : L_SIDE_PSD =");
	prn_byte(temp_b);
	prn_string("\n");

	data_byte_0_RDQSG_left_total_tap = temp_a + data_byte_0_RDQSG_left_total_tap;
	data_byte_1_RDQSG_left_total_tap = temp_b + data_byte_1_RDQSG_left_total_tap;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 16) & 0x1F;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 16) & 0x1F;
	prn_string("\tDX0 : R_SIDE_RSL =");
	prn_byte(temp_a);
	prn_string("\tDX1 : R_SIDE_RSL =");
	prn_byte(temp_b);
	prn_string("\n");

	data_byte_0_RDQSG_right_total_tap = temp_a * RDQS_IPRD_TAP_NO;
	data_byte_1_RDQSG_right_total_tap = temp_b * RDQS_IPRD_TAP_NO;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 21) & 0x03;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 21) & 0x03;
	prn_string("\tDX0 : R_SIDE_PHA =");
	prn_byte(temp_a);
	prn_string("\tDX1 : R_SIDE_PHA =");
	prn_byte(temp_b);
	prn_string("\n");

	data_byte_0_RDQSG_right_total_tap = (temp_a * RDQS_IPRD_TAP_NO / 2) + data_byte_0_RDQSG_right_total_tap;
	data_byte_1_RDQSG_right_total_tap = (temp_b * RDQS_IPRD_TAP_NO / 2) + data_byte_1_RDQSG_right_total_tap;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 24) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 24) & 0xFF;
	prn_string("\tDX0 : R_SIDE_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : R_SIDE_PSD =");
	prn_byte(temp_b);
	prn_string("\n\n");

	data_byte_0_RDQSG_right_total_tap = temp_a + data_byte_0_RDQSG_right_total_tap;
	data_byte_1_RDQSG_right_total_tap = temp_b + data_byte_1_RDQSG_right_total_tap;

	prn_string("DPCU_DT_INFO : \t********** DUMP R/W EYE status **********\n");
	prn_string("\t[DATx8-0]\t\t[DATx8-1]\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 16) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 16) >> 0) & 0xFF;
	prn_string("\tDX0 : REYE_MIN =");
	prn_byte(temp_a);
	prn_string("\tDX1 : REYE_MIN =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 16) >> 8) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 16) >> 8) & 0xFF;
	prn_string("\tDX0 : REYE_MAX =");
	prn_byte(temp_a);
	prn_string("\tDX1 : REYE_MAX =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 16) >> 16) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 16) >> 16) & 0xFF;
	prn_string("\tDX0 : WEYE_MIN =");
	prn_byte(temp_a);
	prn_string("\tDX1 : WEYE_MIN =");
	prn_byte(temp_b);
	prn_string("\n");

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 16) >> 24) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 16) >> 24) & 0xFF;
	prn_string("\tDX0 : WEYE_MAX =");
	prn_byte(temp_a);
	prn_string("\tDX1 : WEYE_MAX =");
	prn_byte(temp_b);
	prn_string("\n\n");

	if (!only_dump_PSD) {
		prn_string("DPCU_DT_INFO : \t********** DUMP REYE BDD status **********\n");
		prn_string("\t[DATx8-0]\t\t[DATx8-1]\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 11) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 11) >> 0) & 0x3F;
		prn_string("\tDX0 : RDQS_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQS_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 12) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 12) >> 0) & 0x3F;
		prn_string("\tDX0 : RDQ0_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ0_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 12) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 12) >> 8) & 0x3F;
		prn_string("\tDX0 : RDQ1_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ1_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 12) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 12) >> 16) & 0x3F;
		prn_string("\tDX0 : RDQ2_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ2_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 12) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 12) >> 24) & 0x3F;
		prn_string("\tDX0 : RDQ3_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ3_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 13) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 13) >> 0) & 0x3F;
		prn_string("\tDX0 : RDQ4_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ4_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 13) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 13) >> 8) & 0x3F;
		prn_string("\tDX0 : RDQ5_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ5_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 13) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 13) >> 16) & 0x3F;
		prn_string("\tDX0 : RDQ6_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ6_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 13) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 13) >> 24) & 0x3F;
		prn_string("\tDX0 : RDQ7_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ7_BDD =");
		prn_byte(temp_b);
		prn_string("\n\n");

		prn_string("DPCU_DT_INFO : \t********** DUMP WEYE BDD status **********\n");
		prn_string("\t[DATx8-0]\t\t[DATx8-1]\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 8) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 8) >> 0) & 0x3F;
		prn_string("\tDX0 :     WDM_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :     WDM_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 8) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 8) >> 8) & 0x3F;
		prn_string("\tDX0 :    WDQS_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQS_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 8) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 8) >> 16) & 0x3F;
		prn_string("\tDX0 : WDQS_OE_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : WDQS_OE_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 8) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 8) >> 24) & 0x3F;
		prn_string("\tDX0 :  WDQ_OE_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :  WDQ_OE_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 9) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 9) >> 0) & 0x3F;
		prn_string("\tDX0 :    WDQ0_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ0_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 9) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 9) >> 8) & 0x3F;
		prn_string("\tDX0 :    WDQ1_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ1_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 9) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 9) >> 16) & 0x3F;
		prn_string("\tDX0 :    WDQ2_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ2_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 9) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 9) >> 24) & 0x3F;
		prn_string("\tDX0 :    WDQ3_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ3_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 10) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 10) >> 0) & 0x3F;
		prn_string("\tDX0 :    WDQ4_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ4_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 10) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 10) >> 8) & 0x3F;
		prn_string("\tDX0 :    WDQ5_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ5_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 10) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 10) >> 16) & 0x3F;
		prn_string("\tDX0 :    WDQ6_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ6_BDD =");
		prn_byte(temp_b);
		prn_string("\n");

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 10) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 10) >> 24) & 0x3F;
		prn_string("\tDX0 :    WDQ7_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ7_BDD =");
		prn_byte(temp_b);
		prn_string("\n\n");
	}

	prn_string("\t-----------------------------------------------------\n\n");
	// finish
	prn_string("DPCU_INFO : ----- DPCU dump done -----\n\n");
} // end function => DPCU_DT_RESULT_DUMP


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

	// debug = 1;
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
			ret = memory_rw_check(ram[i], i, debug);
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

// ***********************************************************************
// * FUNC      : dram_training_flow
// * PARAM     : dram_id
// * PURPOSE   : to do the following sequences
// *           : (1). DPCU DRAM inital setting
// *           : (2). SDCTRL RGST setting
// *           : (3). DRAM initial setting by SDCTRL
// *           : (5). DDR_DPHY+APHY data training
// ***********************************************************************
int dram_training_flow(unsigned int dram_id)
{
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0;
	unsigned int        temp_1, temp_2;
	unsigned int        wait_flag      = 0;	 // min

	prn_string(">>> enter dram_training_flow for DRAM");
	prn_decimal(dram_id);
	prn_string("\n");
	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	get_sdc_phy_addr(dram_id, &SDC_BASE_GRP, &PHY_BASE_GRP);

#ifdef SDRAM_FPGA
	// There are no APHY circuit in FPGA platform, so bypass this flow
#else
	// mazar test print PZQ impedence
	// rgst_value = (SP_REG(, );
	// prn_string("PZQ IO impedance="); prn_dword(rgst_value); prn_string("\n");
	// end

	// prn_string("\tGRP(PHY_BASE_GRP, 0) = "); prn_dword(SP_REG(PHY_BASE_GRP, 0)); prn_string("\n");
	// prn_string("\tGRP(PHY_BASE_GRP+1, 10) = "); prn_dword(SP_REG(PHY_BASE_GRP+1, 10)); prn_string("\n");
	if (SP_REG(PHY_BASE_GRP, 0) == 0x00) {
		prn_string("<<< 1 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return WAIT_FLAG_FAIL;
	}
	if (SP_REG(PHY_BASE_GRP + 1, 10) == 0x00) {
		prn_string("<<< 2 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return WAIT_FLAG_FAIL;
	}

	if (ckobd_training_flag == 1) {
		// 20140812: Add CB0DB+4 to solve WL (DQS/CK wire path issue) and RDQSG change phase issue
		// #define DPCU_CMD_BDD_SETTING            (DPCU_ACK0BD<<16) | (DPCU_AC0BD << 8) | (DPCU_CK0BD << 0)
#if 0
		rgst_value = (DPCU_CK0BD + (ckobd_re_training_number * 4));
		prn_string("\tSet CK0BD value to : ");
		prn_decimal(rgst_value);
		prn_string("\n");
		SP_REG(PHY_BASE_GRP + 0, 17) = (((DPCU_ACK0BD + (ckobd_re_training_number * 4)) << 16) | ((DPCU_AC0BD + (ckobd_re_training_number * 4)) << 8) | ((DPCU_CK0BD +
						(ckobd_re_training_number * 4)) << 0));
#else

		// rgst_value = (gCK+(ckobd_re_training_number*4));
		// prn_string("\tSet CK0BD value to : "); prn_decimal(rgst_value); prn_string("\n");
		SP_REG(PHY_BASE_GRP + 0, 17) = (((gACK + (ckobd_re_training_number * 4)) << 16) | ((gAC + (ckobd_re_training_number * 4)) << 8) | ((gCK +
						(ckobd_re_training_number * 4)) << 0));
#endif
	} else if (ckobd_training_flag == 2) {
		temp_1 = 0x30 + (ckobd_re_training_number * 4);
		SP_REG(PHY_BASE_GRP + 0, 17) = \
					       (((gACK + temp_1 + ((SP_REG(PHY_BASE_GRP + 0, 17) >> 16) & 0x3F)) << 16) | \
						((gAC  + temp_1 + ((SP_REG(PHY_BASE_GRP + 0, 17) >>  8) & 0x3F)) <<  8) | \
						((gCK  + temp_1 + ((SP_REG(PHY_BASE_GRP + 0, 17) >>  0) & 0x3F)) <<  0));
	}

	else {
		// mazar :  Add DPCU APHY CK,AC,ACK BDD Delay for socket and direct IC can use the same SDCTRL RGST setting 2014.0719
		// rgst_value = (DPCU_CK0BD);
		// prn_string("\n\t Set new CK0BD value to Ellison : "); prn_decimal(rgst_value); prn_string(" ------------------\n");
		SP_REG(PHY_BASE_GRP + 0, 17) = DPCU_CMD_BDD_SETTING;
	}

	dbg_stamp(0xA001);

	// release SerDes' reset
	SP_REG(PHY_BASE_GRP + 0, 8) = (SP_REG(PHY_BASE_GRP + 0, 8) & 0xFFFFFFF8) | 0x00000007;

	// clean CMD_ISSUE MRS field
	SP_REG(PHY_BASE_GRP + 1, 25) = 0x00;
	SP_REG(PHY_BASE_GRP + 1, 26) = 0x00;

	// sync. MRS setting to CMD_ISSUE
	// dbg_stamp(CI_MRS_0_VAL_SET);
	// dbg_stamp(CI_MRS_1_VAL_SET);
	// dbg_stamp(CI_MRS_2_VAL_SET);
	// dbg_stamp(CI_MRS_3_VAL_SET);

	// SP_REG(PHY_BASE_GRP+1, 25) = (CI_MRS_1_VAL_SET<<16) | (CI_MRS_0_VAL_SET << 0);
	SP_REG(PHY_BASE_GRP + 1, 25) = CI_MRS_1_VAL_SET;
	SP_REG(PHY_BASE_GRP + 1, 25) = (SP_REG(PHY_BASE_GRP + 1, 25) << 16) | (CI_MRS_0_VAL_SET << 0);
	SP_REG(PHY_BASE_GRP + 1, 26) = (CI_MRS_3_VAL_SET << 16) | (CI_MRS_2_VAL_SET << 0);

#ifdef CSIM_ASIC
	dbg_stamp(0xA002);
	// csim simulation only, for YES_QUICK_SIM and DPCU_FAST (speedup csim flow)
	// RGST_CI_RST_B_HIGH & RGST_CKE_HIGH
	// SP_REG(PHY_BASE_GRP+1, 15)  =   0x00F000FF;
	// RGST_CI_RST_B_LOW
	// SP_REG(PHY_BASE_GRP+1, 16)  =   0x00000FF0;
#else
	// IC - Real initial flow (ASIC & FPGA)
#endif

	dbg_stamp(0xA003);

	// trigger CMD_ISSUE DRAM_INIT sequence
	if (SP_REG(PHY_BASE_GRP + 1, 10) == 0x00) {
		prn_string("<<< 3 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return WAIT_FLAG_FAIL;
	}
	// prn_string("\tbefore GRP((PHY_BASE_GRP+1),10) = "); prn_dword(SP_REG(PHY_BASE_GRP+1, 10)); prn_string("\n");
	SP_REG(PHY_BASE_GRP + 1, 10) = (SP_REG(PHY_BASE_GRP + 1, 10) & 0xFFFFFFEF) | 0x00000010;

	// wait CMD_ISSUE flag
#endif // endif - SDRAM_FPGA

	dbg_stamp(0xA004);

	// -------------------------------------------------------
	// 2. SDCTRL RGST setting => a002
	// -------------------------------------------------------
	// DRAM MRS SETTING
	// DDR2: PREA -> MRS2 -> MRS3 -> MRS1 ->
	//       MRS0(DLL_RESET0) -> MRS0(DLL_RESET1) -> MRS1(OCD DFLT) -> MRS1(OCD EXIT)
	// DDR3: MRS2 -> MRS3 -> MRS1 -> MRS0

	UMCTL2_REG(0x0304) = UMCTL2_304(UMCTL2_304_1);
	UMCTL2_REG(0x0030) = UMCTL2_30(UMCTL2_30_1);
	UMCTL2_REG(0x0000) = UMCTL2_0;
	UMCTL2_REG(0x0010) = UMCTL2_10;
	UMCTL2_REG(0x0014) = UMCTL2_14;
	UMCTL2_REG(0x0030) = UMCTL2_30(UMCTL2_30_2);
	UMCTL2_REG(0x0034) = UMCTL2_34;
	UMCTL2_REG(0x0038) = UMCTL2_38;
	UMCTL2_REG(0x0050) = UMCTL2_50;
	UMCTL2_REG(0x0060) = UMCTL2_60;
#ifdef CONFIG_DRAM_SIZE_USE_OTP
	if (DRAM_SIZE_FLAG == DRAM_SIZE_512Mb) {
		UMCTL2_REG(0x0064) = UMCTL2_64_512Mb;
	} else if (DRAM_SIZE_FLAG == DRAM_SIZE_1Gb) {
		UMCTL2_REG(0x0064) = UMCTL2_64_1Gb;
	} else if (DRAM_SIZE_FLAG == DRAM_SIZE_4Gb) {
		UMCTL2_REG(0x0064) = UMCTL2_64_4Gb;
	} else {
		UMCTL2_REG(0x0064) = UMCTL2_64;
	}
#else
	UMCTL2_REG(0x0064) = UMCTL2_64;
#endif
	UMCTL2_REG(0x00C0) = UMCTL2_C0;
	UMCTL2_REG(0x00D0) = UMCTL2_D0;
	UMCTL2_REG(0x00D4) = UMCTL2_D4;
	UMCTL2_REG(0x00DC) = UMCTL2_DC;
	UMCTL2_REG(0x00E0) = UMCTL2_E0;
	UMCTL2_REG(0x00E4) = UMCTL2_E4;
	UMCTL2_REG(0x00F0) = UMCTL2_F0;
	UMCTL2_REG(0x0100) = UMCTL2_100;
	UMCTL2_REG(0x0104) = UMCTL2_104;
	UMCTL2_REG(0x0108) = UMCTL2_108;
	UMCTL2_REG(0x010C) = UMCTL2_10C;
	UMCTL2_REG(0x0110) = UMCTL2_110;
	UMCTL2_REG(0x0114) = UMCTL2_114;
	UMCTL2_REG(0x0120) = UMCTL2_120;
	UMCTL2_REG(0x013C) = UMCTL2_13C;
	UMCTL2_REG(0x0180) = UMCTL2_180;
	UMCTL2_REG(0x0184) = UMCTL2_184;
#if (defined(DRAMSCAN) || defined(SISCOPE))
	UMCTL2_REG(0x0190) = scan_val_190;
#else
	UMCTL2_REG(0x0190) = UMCTL2_190;
#endif
	UMCTL2_REG(0x0194) = UMCTL2_194;
	UMCTL2_REG(0x0198) = UMCTL2_198;
	UMCTL2_REG(0x01A0) = UMCTL2_1A0;
	UMCTL2_REG(0x01A4) = UMCTL2_1A4;
	UMCTL2_REG(0x01A8) = UMCTL2_1A8;
	UMCTL2_REG(0x01B0) = UMCTL2_1B0(UMCTL2_1B0_1);
	UMCTL2_REG(0x01B4) = UMCTL2_1B4;
	UMCTL2_REG(0x01C4) = UMCTL2_1C4;
	UMCTL2_REG(0x0204) = UMCTL2_204;
	UMCTL2_REG(0x0208) = UMCTL2_208;
	UMCTL2_REG(0x020C) = UMCTL2_20C;
	UMCTL2_REG(0x0210) = UMCTL2_210;
	UMCTL2_REG(0x0214) = UMCTL2_214;
#ifdef CONFIG_DRAM_SIZE_USE_OTP
	if (DRAM_SIZE_FLAG == DRAM_SIZE_512Mb) {
		UMCTL2_REG(0x0218) = UMCTL2_218_512Mb;
	} else if (DRAM_SIZE_FLAG == DRAM_SIZE_1Gb) {
		UMCTL2_REG(0x0218) = UMCTL2_218_1Gb;
	} else if (DRAM_SIZE_FLAG == DRAM_SIZE_4Gb) {
		UMCTL2_REG(0x0218) = UMCTL2_218_4Gb;
	} else {
		UMCTL2_REG(0x0218) = UMCTL2_218;
	}
#else
	UMCTL2_REG(0x0218) = UMCTL2_218;
#endif
	UMCTL2_REG(0x0224) = UMCTL2_224;
	UMCTL2_REG(0x0228) = UMCTL2_228;
	UMCTL2_REG(0x022C) = UMCTL2_22C;
	UMCTL2_REG(0x0240) = UMCTL2_240;
	UMCTL2_REG(0x0244) = UMCTL2_244;
	UMCTL2_REG(0x0250) = UMCTL2_250;
	UMCTL2_REG(0x0254) = UMCTL2_254;
	UMCTL2_REG(0x025C) = UMCTL2_25C;
	UMCTL2_REG(0x0264) = UMCTL2_264;
	UMCTL2_REG(0x026C) = UMCTL2_26C;
	UMCTL2_REG(0x0300) = UMCTL2_300;
	UMCTL2_REG(0x0304) = UMCTL2_304(UMCTL2_304_2);
	UMCTL2_REG(0x030C) = UMCTL2_30C;
	UMCTL2_REG(0x0320) = UMCTL2_320(UMCTL2_320_1);
	UMCTL2_REG(0x036C) = UMCTL2_36C;
	UMCTL2_REG(0x0490) = UMCTL2_490;
	UMCTL2_REG(0x0400) = UMCTL2_400;
	UMCTL2_REG(0x0404) = UMCTL2_404(UMCTL2_404_1);
	UMCTL2_REG(0x0404) = UMCTL2_404(UMCTL2_404_2);
	UMCTL2_REG(0x0404) = UMCTL2_404(UMCTL2_404_2);
	UMCTL2_REG(0x0404) = UMCTL2_404(UMCTL2_404_3);
	UMCTL2_REG(0x0408) = UMCTL2_408(UMCTL2_408_1);
	UMCTL2_REG(0x0408) = UMCTL2_408(UMCTL2_408_2);
	UMCTL2_REG(0x0408) = UMCTL2_408(UMCTL2_408_2);
	UMCTL2_REG(0x0408) = UMCTL2_408(UMCTL2_408_3);
	UMCTL2_REG(0x0404) = UMCTL2_404(UMCTL2_404_2);
	UMCTL2_REG(0x0408) = UMCTL2_408(UMCTL2_408_3);
	UMCTL2_REG(0x0404) = UMCTL2_404(UMCTL2_404_1);
	UMCTL2_REG(0x0408) = UMCTL2_408(UMCTL2_408_4);
	wait_loop(5);
	UMCTL2_REG(0x0304) = UMCTL2_304(UMCTL2_304_2);
	UMCTL2_REG(0x0030) = UMCTL2_30(UMCTL2_30_2);
	UMCTL2_REG(0x0030) = UMCTL2_30(UMCTL2_30_2);
	UMCTL2_REG(0x0320) = UMCTL2_320(UMCTL2_320_2);
	UMCTL2_REG(0x01B0) = UMCTL2_1B0(UMCTL2_1B0_2);
	wait_loop(5);
	wait_loop(5);
	UMCTL2_REG(0x01B0) = UMCTL2_1B0(UMCTL2_1B0_3);
	UMCTL2_REG(0x0320) = UMCTL2_320(UMCTL2_320_1);
	do {
		wait_flag = UMCTL2_REG(0x0324) & 0x00000001;
	} while ((wait_flag == 0));


#ifdef DRAM_ZQ_CFG
	// ZQCL setting
#ifdef PLATFORM_PENTAGRAM
	// TBD
#elif defined(PLATFORM_GEMINI)
	// Not support
#endif

#endif

	dbg_stamp(0xA002);

	// -------------------------------------------------------
	// 1. DPCU data training => a003
	// -------------------------------------------------------

	// Setting RGST_DT_WREYE_MIN_RANGE
	// WR deskew issue
#ifdef SDRAM_FPGA
	// bypass data training at FPGA setting
#else
#ifdef MPEG_DRAM_8BIT
	SP_REG(PHY_BASE_GRP + 3, 0) = 0x00000000; // disable 1 serdes
#endif
	// Set DPCU Deskew manual pattern and access address range
	// 1. Pattern type select; G37 addr 1 bit[29:28]
	// 2. Pattern bank select; G37 addr 1 bit[25:24]
	// 3. Pattern address select; G37 addr 2(bank, rol,col)
	// 4. Pattern select; G37 addr 3(burst3_0), G37 addr 4(burst7_4)
	// Pattern 1
	// 1.&2. Pattern type and bank select
	SP_REG(PHY_BASE_GRP + 1, 1) = DT_PAT_TYPE_SEL(n_DESKEW_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_0);;
	// 3. Address select
	SP_REG(PHY_BASE_GRP + 1, 2) = DT_DRAM0_ADDRESS;
	// 4. Patterns select
	SP_REG(PHY_BASE_GRP + 1, 3) = DT_PAT0_DESKEW_3_0;
	SP_REG(PHY_BASE_GRP + 1, 4) = DT_PAT0_DESKEW_7_4;
	// Pattern 2
	// 1.&2. Pattern type and bank select
	SP_REG(PHY_BASE_GRP + 1, 1) = DT_PAT_TYPE_SEL(n_DESKEW_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_1);
	// 3. Address select
	SP_REG(PHY_BASE_GRP + 1, 2) = DT_DRAM0_ADDRESS;
	// 4. Patterns select
	SP_REG(PHY_BASE_GRP + 1, 3) = DT_PAT1_DESKEW_3_0;
	SP_REG(PHY_BASE_GRP + 1, 4) = DT_PAT1_DESKEW_7_4;
	// Pattern 3
	// 1.&2. Pattern type and bank select
	SP_REG(PHY_BASE_GRP + 1, 1) = DT_PAT_TYPE_SEL(n_DESKEW_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_2);
	// 3. Address select
	SP_REG(PHY_BASE_GRP + 1, 2) = DT_DRAM0_ADDRESS;
	// 4. Patterns select
	SP_REG(PHY_BASE_GRP + 1, 3) = DT_PAT2_DESKEW_3_0;
	SP_REG(PHY_BASE_GRP + 1, 4) = DT_PAT2_DESKEW_7_4;
	// Pattern 4
	// 1.&2. Pattern type and bank select
	SP_REG(PHY_BASE_GRP + 1, 1) = DT_PAT_TYPE_SEL(n_DESKEW_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_3);
	// 3. Address select
	SP_REG(PHY_BASE_GRP + 1, 2) = DT_DRAM0_ADDRESS;
	// 4. Patterns select
	SP_REG(PHY_BASE_GRP + 1, 3) = DT_PAT3_DESKEW_3_0;
	SP_REG(PHY_BASE_GRP + 1, 4) = DT_PAT3_DESKEW_7_4;

	// EYE manual pattern
	// 1. Pattern type select; G37 addr 1 bit[29:28]
	// 2. Pattern bank select; G37 addr 1 bit[25:24]
	// 3. Pattern address select; G37 addr 2(bank, rol,col)
	// 4. Pattern select; G37 addr 3(burst3_0), G37 addr 4(burst7_4)
	// Pattern 1
	// 1.&2. Pattern type and bank select
	SP_REG(PHY_BASE_GRP + 1, 1) = DT_PAT_TYPE_SEL(n_EYE_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_0);
	// 3. Address select
	SP_REG(PHY_BASE_GRP + 1, 2) = DT_DRAM0_ADDRESS;
	// 4. Patterns select
	SP_REG(PHY_BASE_GRP + 1, 3) = DT_PAT0_EYE_3_0;
	SP_REG(PHY_BASE_GRP + 1, 4) = DT_PAT0_EYE_7_4;
	// Pattern 2
	// 1.&2. Pattern type and bank select
	SP_REG(PHY_BASE_GRP + 1, 1) = DT_PAT_TYPE_SEL(n_EYE_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_1);
	// 3. Address select
	SP_REG(PHY_BASE_GRP + 1, 2) = DT_DRAM0_ADDRESS;
	// 4. Patterns select
	SP_REG(PHY_BASE_GRP + 1, 3) = DT_PAT1_EYE_3_0;
	SP_REG(PHY_BASE_GRP + 1, 4) = DT_PAT1_EYE_7_4;
	// Pattern 3
	// 1.&2. Pattern type and bank select
	SP_REG(PHY_BASE_GRP + 1, 1) = DT_PAT_TYPE_SEL(n_EYE_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_2);
	// 3. Address select
	SP_REG(PHY_BASE_GRP + 1, 2) = DT_DRAM0_ADDRESS;
	// 4. Patterns select
	SP_REG(PHY_BASE_GRP + 1, 3) = DT_PAT2_EYE_3_0;
	SP_REG(PHY_BASE_GRP + 1, 4) = DT_PAT2_EYE_7_4;
	// Pattern 4
	// 1.&2. Pattern type and bank select
	SP_REG(PHY_BASE_GRP + 1, 1) = DT_PAT_TYPE_SEL(n_EYE_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_3);
	// 3. Address select
	SP_REG(PHY_BASE_GRP + 1, 2) = DT_DRAM0_ADDRESS;
	// 4. Patterns select
	SP_REG(PHY_BASE_GRP + 1, 3) = DT_PAT3_EYE_3_0;
	SP_REG(PHY_BASE_GRP + 1, 4) = DT_PAT3_EYE_7_4;

	// step-1: SWITCH PATH to DPCU
	SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_DPCU);
#ifdef DPCU_TRAINING_ODT_ON  // open IC_ODT and DRAM_ODT
	// step-0: SWITCH PATH to DPCU
	// step a switch to CMD ISSUE SW mux
	rgst_value                     =   SP_REG(PHY_BASE_GRP + 1, 10);
	wait_loop(10);

	SP_REG(PHY_BASE_GRP + 1, 10) = rgst_value | (1 << 1);
	// prn_string("(SP_REG(PHY_BASE_GRP+1, 10); prn_dword((SP_REG(PHY_BASE_GRP+1, 10); prn_string("!!\n");
	// prn_string("(SP_REG(PHY_BASE_GRP+1, 11); prn_dword((SP_REG(PHY_BASE_GRP+1, 11); prn_string("!!\n");

	// step b issue CMD ISSUE IC ODT on command
	// dram_id, CMD, RANK, BANK, ADDR, DM, DATA1_HIGH, DATA1_LOW, DATA0_HIGH, DATA0_LOW, TRIGGER
	DPCU_CMD_ISSUE_SW_CMD(dram_id, 0x08, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);  // COMMIT and TIGGER // ODT ON SPECIAL COMMAND
	// step b issue CMD ISSUE ODT on command
	// dram_id, CMD, RANK, BANK, ADDR, DM, DATA1_HIGH, DATA1_LOW, DATA0_HIGH, DATA0_LOW, TRIGGER
	DPCU_CMD_ISSUE_SW_CMD(dram_id, 0x08, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);  // COMMIT and TIGGER // ODT ON SPECIAL COMMAND
	wait_flag = 0;
	do {
		wait_loop(1000);
		wait_flag   = (SP_REG(PHY_BASE_GRP + 1, 10) >> 17) & 0x01;
	} while ((wait_flag == 0));
	// step c Switch back to HW
	rgst_value                     =   SP_REG(PHY_BASE_GRP + 1, 10);
	SP_REG(PHY_BASE_GRP + 1, 10) = rgst_value & (0xFFFFFFFD);
#endif
#ifdef  DPCU_TRAINING_MANUAL_PAT
	prn_string("\tDPCU Training by Manual pattern!!\n");
	rgst_value                =    SP_REG(PHY_BASE_GRP + 1, 1) & 0x00FFFFFF; // extract [23:0]
	// B0
	SP_REG(PHY_BASE_GRP + 1, 1) = rgst_value | (0 << 24) | (3 << 28); // BANK0, WR/RD EYE setting
	SP_REG(PHY_BASE_GRP + 1, 3) = 0x02488024;
	SP_REG(PHY_BASE_GRP + 1, 4) = 0x42088420;
	// B1
	SP_REG(PHY_BASE_GRP + 1, 1) = rgst_value | (1 << 24) | (3 << 28); // BANK1, WR/RD EYE setting
	SP_REG(PHY_BASE_GRP + 1, 3) = 0x28040482;
	SP_REG(PHY_BASE_GRP + 1, 4) = 0x28404082;
	// B2
	SP_REG(PHY_BASE_GRP + 1, 1) = rgst_value | (2 << 24) | (3 << 28); // BANK2, WR/RD EYE setting
	SP_REG(PHY_BASE_GRP + 1, 3) = 0x84024208;
	SP_REG(PHY_BASE_GRP + 1, 4) = 0x80242048;
	// B3
	SP_REG(PHY_BASE_GRP + 1, 1) = rgst_value | (3 << 24) | (3 << 28); // BANK2, WR/RD EYE setting
	SP_REG(PHY_BASE_GRP + 1, 3) = 0x40284802;
	SP_REG(PHY_BASE_GRP + 1, 4) = 0x20848204;
#endif

	/* by default,
	 * DPCU_TRAINING_READ_MPR is not entered
	 * DPCU_TRAINING_RG_LINEAR is entered, but we do not want to enter it while resume
	 */
#ifdef  DPCU_TRAINING_READ_MPR
	prn_string("\tDPCU Training by MPR mode!!\n");
	rgst_value                =    SP_REG(PHY_BASE_GRP + 1, 5);
	SP_REG(PHY_BASE_GRP + 1, 5) = rgst_value | DT_RD_MPR(n_DT_RD_MPR_EN);
#endif
	// step-2: trigger 1st data training start
#ifdef  DPCU_TRAINING_RG_LINEAR
	prn_string("\tDPCU RG Training by Linear Search\n");
	rgst_value                  =   SP_REG(PHY_BASE_GRP + 1, 5);
	SP_REG(PHY_BASE_GRP + 1, 5) = rgst_value | DT_RG_LINEAR(n_DT_RG_LINEAR_EN);
#endif

#if (defined(PLATFORM_PENTAGRAM) || defined(PLATFORM_I143))
	SP_REG(PHY_BASE_GRP + 1, 5) = (SP_REG(PHY_BASE_GRP + 1, 5) & 0xFFFFFFF6) | 0x00000009;
	SP_REG(PHY_BASE_GRP + 1, 6) = (SP_REG(PHY_BASE_GRP + 1, 6) & 0xFFFFFF00) | 0x0000000B;
#endif

	// refresh period
	rgst_value = SP_REG(PHY_BASE_GRP + 1, 1);
	rgst_value &= (~(0x7FFF) << 8);
	rgst_value |= 0x0A20 << 8;
	SP_REG(PHY_BASE_GRP + 1, 1) = rgst_value;
	rgst_value = SP_REG(PHY_BASE_GRP + 1, 1);
	prn_string("\trefresh period = ");
	prn_dword(rgst_value);

	SP_REG(PHY_BASE_GRP + 1, 10) |= 1 << 20;
	SP_REG(PHY_BASE_GRP + 1, 1) = DPCU_DT_GO;

	// wait_dpcu_1st_training
	wait_flag = 0;
	do {
		UMCTL2_REG(0x0190) = UMCTL2_REG(0x0190) & 0xFF80FFFF;
		rgst_value = ( SP_REG(PHY_BASE_GRP + 1, 6) >> 27 );
		UMCTL2_REG(0x0190) = UMCTL2_REG(0x0190) | (( n_tCL + rgst_value ) << 16 );

		wait_loop(1000);
		wait_flag   =   SP_REG(PHY_BASE_GRP + 1, 0) & 0x01;
		// prn_string("111 training SP_REG(PHY_BASE_GRP+1, 0)="); prn_dword(SP_REG(PHY_BASE_GRP+1, 0)); prn_string("\n");
	} while ((wait_flag == 0));

	// check_1st_dt_result
	rgst_value = (SP_REG(PHY_BASE_GRP + 1, 0) >> 8) & 0x3F;

	if (rgst_value != 0x00) {
		// dpcu training failed
		SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
		dbg_stamp(0xDEADA003);
		prn_string("<<< 4 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return 0;
	} // end- if check dt result

	// step-3: dpcu_2nd_training - trigger RDQSG training
	SP_REG(PHY_BASE_GRP + 1, 1) = DPCU_DT_CFG0 | DT_RG(n_RG_EN) | DPCU_TRAIN_START(n_DT_START);
#ifdef CONFIG_DRAM_SIZE_USE_OTP
	if (DRAM_SIZE_FLAG == DRAM_SIZE_512Mb) {
		SP_REG(PHY_BASE_GRP + 1, 1) = (SP_REG(PHY_BASE_GRP + 1, 1) & 0xFF8000FF) | DT_AREF_PRD_512Mb;
	} else if (DRAM_SIZE_FLAG == DRAM_SIZE_1Gb) {
		SP_REG(PHY_BASE_GRP + 1, 1) = (SP_REG(PHY_BASE_GRP + 1, 1) & 0xFF8000FF) | DT_AREF_PRD_1Gb;
	} else if (DRAM_SIZE_FLAG == DRAM_SIZE_4Gb) {
		SP_REG(PHY_BASE_GRP + 1, 1) = (SP_REG(PHY_BASE_GRP + 1, 1) & 0xFF8000FF) | DT_AREF_PRD_4Gb;
	}
#endif
	wait_loop(10000);   // wait for clear DPCU DT done

	// rgst_value = SP_REG(PHY_BASE_GRP+1, 0);
	// prn_string("DPCU mid 2sec Training Status G37.0: 0x"); prn_dword(rgst_value); prn_string("!!\n");

	// wait_dpcu_2nd_training
	wait_flag = 0;
	do {
		UMCTL2_REG(0x0190) = UMCTL2_REG(0x0190) & 0xFF80FFFF;
		rgst_value = ( SP_REG(PHY_BASE_GRP + 1, 6) >> 27 );
		UMCTL2_REG(0x0190) = UMCTL2_REG(0x0190) | (( n_tCL + rgst_value ) << 16 );

		wait_loop(1000);
		wait_flag   = (SP_REG(PHY_BASE_GRP + 1, 0) & 0x01);
		// prn_string("222 training SP_REG(PHY_BASE_GRP+1, 0)="); prn_dword(SP_REG(PHY_BASE_GRP+1, 0)); prn_string("\n");
	} while ((wait_flag == 0));

	// check_2nd_dt_result
	rgst_value = (SP_REG(PHY_BASE_GRP + 1, 0) >> 9) & 0x01;

	if (rgst_value != 0x00) {
		// dpcu training failed
		SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
		dbg_stamp(0xDEADA003);
		prn_string("<<< 5 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return 0;
	} // end- if check dt result

	// move switch SDCTRL path before dump
	SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
	DPCU_DT_RESULT_DUMP(dram_id); // SET_RDQS_IPRD_TAP_NO(dram_id);

	// Ellison : 20140815 : Check RDQSG Eye boundary result of Each byte correct or not !? ( Left-side < Right-side)
	if ((data_byte_0_RDQSG_left_total_tap >= data_byte_0_RDQSG_right_total_tap) ||
	    (data_byte_1_RDQSG_left_total_tap >= data_byte_1_RDQSG_right_total_tap)) {
		SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
		prn_string("\tRDQSG Training result : Left side boundary > Right side boundary !!!\n");
		prn_string("<<< 6 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return 0;
	}

	// +- offset(1/8)*2 RG_PSD
	temp_2 = (SP_REG(PHY_BASE_GRP + 2, 3) >> 8) & 0xFF; // DX0_RG_IPRD
	rgst_value = (SP_REG(PHY_BASE_GRP + 2, 14) >> 16) & 0xFF; // DX0_RG_PSD
	temp_1 = temp_2 / 4; // 1/4 offset
	// prn_string("\t DX0_RG_IPRD = "); prn_dword(temp_2); prn_string("\n");
	// prn_string("\t DX0_RG_PSD = "); prn_dword(rgst_value); prn_string("\n");
	// prn_string("\t offset(1/4 RG_IPRD)  = "); prn_dword(temp_1); prn_string("\n");
	if ((rgst_value + temp_1) >= (temp_2 << 1) || rgst_value < temp_1) {
		// prn_string( "RG_PSD is too big or small\n");
		prn_string("<<< 7 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return 0;
	} // end- if RG_PSD is too big or small

	temp_2 = (SP_REG(PHY_BASE_GRP + 3, 3) >> 8) & 0xFF; // DX1_RG_IPRD
	rgst_value = (SP_REG(PHY_BASE_GRP + 3, 14) >> 16) & 0xFF; // DX1_RG_PSD
	temp_1 = temp_2 / 4; // 1/4 offset
	// prn_string("\t DX1_RG_IPRD = "); prn_dword(temp_2); prn_string("\n");
	// prn_string("\t DX1_RG_PSD = "); prn_dword(rgst_value); prn_string("\n");
	// prn_string("\t offset(1/4 RG_IPRD)  = "); prn_dword(temp_1); prn_string("\n");
	if ((rgst_value + temp_1) >= (temp_2 << 1) || rgst_value < temp_1) {
		// dpcu training WL_PSD is too big
		// prn_string( "RG_PSD is too big or small\n");
		prn_string("<<< 7 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return 0;
	} // end- if RG_PSD is too big or small

	// DDR training results confirm flow
	temp_1 = (SP_REG(PHY_BASE_GRP + 2, 14) >> 0) & 0x1F; // DX0_RG_RSL
	temp_2 = (SP_REG(PHY_BASE_GRP + 3, 14) >> 0) & 0x1F; // DX1_RG_RSL
	if (temp_1 != temp_2) {
		prn_string("\t DX0_RG_RSL = "); prn_dword(temp_1);
		prn_string("\t DX1_RG_RSL = "); prn_dword(temp_2);
		prn_string("\n\n");
		prn_string("<<< 7 leave dram_training_flow for DRAM");
		prn_decimal(dram_id);
		prn_string("\n");
		return 0;
	} else {
		temp_1 = (SP_REG(PHY_BASE_GRP + 2, 14) >> 8) & 0x3; // DX0_RG_PHA
		temp_2 = (SP_REG(PHY_BASE_GRP + 3, 14) >> 8) & 0x3; // DX1_RG_PHA
		if (temp_1 != temp_2) {
			prn_string("\t DX0_RG_PHA = "); prn_dword(temp_1);
			prn_string("\t DX1_RG_PHA = "); prn_dword(temp_2);
			prn_string("\n\n");
			prn_string("<<< 7 leave dram_training_flow for DRAM");
			prn_decimal(dram_id);
			prn_string("\n");
			return 0;
		}
	}
#endif // SDRAM_FPGA

	prn_string("<<< leave 8 dram_training_flow for DRAM");
	prn_decimal(dram_id);
	prn_string("\n");
	return 1;
} // end of dram_training_flow

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
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0;
	unsigned int        temp_3         = 0;
	unsigned int        package_256_flag;  // this flag only using in dram_id == 1
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

	// 20140728 mazar : add max_init_fail_cnt for sometime training fail
	for (loop_time = 0; loop_time < max_init_fail_cnt; loop_time++) {
		ckobd_re_training_number = loop_time;
		// ckobd_training_flag = 1;
		package_256_flag = 1; // this flag only using in dram_id == 1

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
			temp_3 = (SP_REG(PHY_BASE_GRP, 2) >> 8) & 0x0F;
			if ((dram_id == 1) && (aphy_select_value == 0x0D)) {
				// case 1, we think this is 216 pin package, and don't need to dump initial error message
				// only check PZQ, CTCAL, DDL error flag
				package_256_flag = 0;
				// prn_string("is this 216 package !?\n");
			} else {
				// case 2 or case 3, we dump the initial error flag
				// DUMP INIT flag
				prn_string("DPCU_INFO : \t********** DUMP APHY INIT error infor @ loop_time = ");
				prn_decimal(loop_time);
				prn_string(" ***\n");
				prn_string("\tCTCAL_ERR flag =");
				prn_decimal((unsigned int)((SP_REG(PHY_BASE_GRP, 2) >> 8) & 0x01));
				prn_string("\tSSCPLL_ERR flag =");
				prn_decimal((unsigned int)((SP_REG(PHY_BASE_GRP, 2) >> 9) & 0x01));
				prn_string("\n");
				prn_string("\tDDL_ERR flag =");
				prn_decimal((unsigned int)((SP_REG(PHY_BASE_GRP, 2) >> 10) & 0x01));
				prn_string("\tPZQ_ERR flag =");
				prn_decimal((unsigned int)((SP_REG(PHY_BASE_GRP, 2) >> 11) & 0x01));
				prn_string("\n");
				prn_string("DPCU_INFO : \t********** DUMP APHY INIT error information end **********\n");
				goto DRAM_BOOT_FLOW_AGAIN;
			}
		} else {
			// prn_string("DRAM-"); prn_decimal(dram_id); prn_string("booting PASS @ loop_time =");
			// prn_decimal(loop_time); prn_string("!!\n");
		}
		prn_string("(dram_id)=");
		prn_decimal(dram_id);
		prn_string("!!\n");
		prn_string("(package_256_flag)=");
		prn_decimal(package_256_flag);
		prn_string("!!\n");

#ifdef DPCU_PZQ_IMPEDANCE_FROM_REG
		// setting PZQ CFG1
		prn_string("DPCU PZQ impedance come from RGST!!!!\n");
		SP_REG(PHY_BASE_GRP + 0, 19) = DPCU_PZQ_CFG1 | DPCU_PZQ_OVRD_ST(n_DPCU_PZQ_OVRD_EN);
		// prn_dword(SP_REG(PHY_BASE_GRP, 19);
#endif

		// dram_training_flow pass return 1, error return 0;
		// 20140727 mazar : training flow chart
		// check DRAM ID, 0 : do training,
		//                1 : check package_256_flag, 0 : pass training flow
		//                                             1 : do training flow
		// 20140727 mazar :  do not add any action if we encounter APHY Training ERR, just dump error flag
		temp_3 = (SP_REG(PHY_BASE_GRP, 2) >> 8) & 0x01;
		if (((dram_id == 1) && (package_256_flag == 0)) || (temp_3 == 1)) {
			// case 1 : (dram_id==1) && (package_256_flag ==0 ) : we think this is 216 package, so don't do data training
			// case 2 : (temp_3 == 1)                           : because SSCPLL isn't assert error flag, we consider CTCAL error flag
			// as SSCPLL error flag, so don't do data training too
			return PACKAGE_216; // break for_loop if encounter case1 or case2
		}

		prn_string("GRP(PHY_BASE_GRP, 9) =  ");
		prn_dword(SP_REG(PHY_BASE_GRP, 9));
		prn_string("\n ");
		ret = dram_training_flow(dram_id);


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
			DPCU_DT_RESULT_DUMP(dram_id);
		} else {
			int i = 0;
			int pass_count = 0;
#if defined(SDRAM0_SIZE_2Gb) || defined(SDRAM0_SIZE_4Gb) || defined(SDRAM0_SIZE_8Gb)
#ifdef PLATFORM_PENTAGRAM
			unsigned int TEST_ADDRESS[3] = {0x00000000, 0x08000000, 0x0C800000};
#else
			unsigned int TEST_ADDRESS[3] = {0x20000000, 0x28000000, 0x2C800000};
#endif
#elif defined(SDRAM0_SIZE_1Gb)
			unsigned int TEST_ADDRESS[3] = {0x00000000, 0x08000000, 0x0C800000};
#elif defined(SDRAM0_SIZE_512Mb)
			unsigned int TEST_ADDRESS[3] = {0x00000000, 16 << 20, 32 << 20};
#else
#error Please assign TEST_ADDRESS[]
#endif
			const unsigned int TEST_COUNT = sizeof(TEST_ADDRESS) >> 2;
			for (i = 0; i < TEST_COUNT; i++) {
				if (SDCTRL_TRIMMER_TEST(dram_id, TEST_ADDRESS[i], 0x0100) != 0)
					pass_count++;
			}

			if (pass_count == TEST_COUNT) {
				prn_string("  ==>  DDR-PHY Training Pass & DRAM Trimmer PASS ....\n\n");
				break;
			} else {
				prn_string("  ==>  DDR-PHY Training Pass but DRAM Trimmer FAIL ....\n\n");
			}
		}
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
	mp = 1;
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
