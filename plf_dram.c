#include <types.h>
#include <common.h>
#include <config.h>
#include <dram_param.h>

/*
 * Flow
 * ------------------------
 * start.S
 * dram_init_main()                    - c code entry
 *     dram_init(0)                    - init dram 0
 *       1. dram_booting_flow()        - ddr phy init
 *             do_system_reset_flow()
 *       2. dram_training_flow()       - ddr phy training
 *             DPCU_CMD_ISSUE_SW_CMD()
 *       3. SDCTRL_TRIMMER_TEST_()     - run trimmer test to verify dram init ok
 *
 * DPCU_DT_RESULT_DUMP()              - print/dump dpcu info
 */

// #define SUPPORT_DRAM1   // dram 1

// ***********************************************************************
// Setting DRAM_INIT_DEBUG to 1 causes the system to print
// debug messages or not. If set to 0 all DRAM_INIT messages will be suppressed instead.
// Dumping all debug messages to UART will cause booting speed to slowdown
// ***********************************************************************
// #define DRAMINIT_BIN_REL_DATE "draminit.bin release date 2016/12/01:2300\n"
#define DRAMINIT_BIN_REL_DATE "\n\nDRAM STRESS TEST TOOL, release date 2017/12/18:1700\n"

// #define DRAM_INIT_DEBUG 0 // defined in Makefile: please use "make debug"

#ifdef          CSIM_ASIC
#define ASIC_CSIM
#elif defined   CSIM_FPGA
#define SDRAM_FPGA
#else
#error Please define => simulation type
#endif

struct sp_registers {
	unsigned int sp_register[1024][32];
};
static volatile struct sp_registers *sp_reg_ptr = (volatile struct sp_registers *)(RF_GRP(0, 0));
#define SP_REG(GROUP, OFFSET)		(sp_reg_ptr->sp_register[GROUP][OFFSET])

// DRAM 0/1 size is 2Byte width
#define DRAM0_BASE_ADDR     0x00000000
#define DRAM1_BASE_ADDR     0x10000000

#define DRAM_0_SDC_REG_BASE     33
#define DRAM_0_PHY_REG_BASE     50
#define DRAM_1_SDC_REG_BASE     46
#define DRAM_1_PHY_REG_BASE     49

#define TEST_LEN                (256 << 20) // 0x10000000 // 256MB
#define SCAN_TRIM_LEN           5


// call external function
// extern void RID_PASS(void);
// extern void RID_FAIL(void);

// --- internal function declaration ---
int dram_booting_flow(unsigned int DRAM_ID)        ;
int dram_training_flow(unsigned int DRAM_ID)        ;
void select_SDC_PHY_GRP(unsigned int DRAM_ID, unsigned int *sdc, unsigned int *phy);

#ifdef SW_REFINE_DT_RESULT
void DRAM_WRITE_CLEAN(unsigned int, unsigned int)    ;
int DRAM_RW_CHECK(unsigned int value, unsigned int answer, int debug);
int DRAM_SEQUENTIAL_RW_TEST(unsigned int DRAM_TEST_SIZE, int debug);
int DRAM_RANDOM_RW_TEST(unsigned int DRAM_TEST_SIZE, int debug);
int DRAM_INVERT_RW_TEST(unsigned int DRAM_TEST_SIZE, int debug);
int do_stress_test(unsigned int test_len, int isDebug, int loop, int exit);
int dram_refine_flow(unsigned int DRAM_ID);
void trim_WDM(unsigned int PHY_REG_BASE);
#endif

#if (defined(DRAMSCAN) || defined(SISCOPE))
void check_ddr_eye_window();
void check_stress_test();
void check_run_siscope();
void dump_now_ddr_clk_info();
int getSquare(int base, int square);
int silent_dram_init();
unsigned int get_unit_pico(unsigned int PHY_REG_BASE);
void DRAM_SCAN(unsigned int)                  ;
int dram_test_flow(unsigned int)                  ;
#endif

unsigned int        rgst_value     = 0;
unsigned int        aphy_select_value     = 0;
unsigned int        ckobd_training_flag      = 0;
unsigned int        ckobd_re_training_number = 0;

unsigned int        data_byte_0_RDQSG_left_total_tap     = 0;
unsigned int        data_byte_0_RDQSG_right_total_tap    = 0;
unsigned int        data_byte_1_RDQSG_left_total_tap     = 0;
unsigned int        data_byte_1_RDQSG_right_total_tap    = 0;
unsigned int 		gAC 			=	 0;
unsigned int 		gACK			=	 0;
unsigned int 		gCK				=	 0;
unsigned int 		gEXTRA_CL_CNT	=	 0;
unsigned int 		gSTR_DQS_IN		=	 0;
unsigned int 		gWL_CNT			=	 0;
unsigned int 		gMPLL_DIV		=	 0;
unsigned char       gResetMPLL      =    0;
unsigned char       gSiScopeDebug   =    0;
u32 mp;
u32 mpb;

void select_SDC_PHY_GRP(unsigned int DRAM_ID, unsigned int *sdc, unsigned int *phy)
{
	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	switch (DRAM_ID) {
	case 0: {
		*sdc    =   DRAM_0_SDC_REG_BASE     ;
		*phy    =   DRAM_0_PHY_REG_BASE     ;
		break;
	}
	case 1: {
		*sdc    =   DRAM_1_SDC_REG_BASE     ;
		*phy    =   DRAM_1_PHY_REG_BASE     ;
		break;
	}
	} // end switch
}


// ***********************************************************************
// * FUNC      : wait_loop
// * PARAM     : wait_counter
// * PURPOSE   : to wait the number of counter times
// ***********************************************************************
void wait_loop(unsigned int wait_counter)
{
	unsigned int    i   =   0   ;

	for (i = 0 ; i < wait_counter ; i++) {;}

} // end - wait_loop


// ***********************************************************************
// * FUNC      : DPCU_DT_RESULT_DUMP
// * PARAM     : DRAM_ID
// * PURPOSE   : Dump DPCU Training information
// ***********************************************************************
void DPCU_DT_RESULT_DUMP(unsigned int DRAM_ID)
{
	unsigned int SDC_BASE_GRP       = 0 ;
	unsigned int PHY_BASE_GRP       = 0 ;
	unsigned int temp_a             = 0 ;
	unsigned int temp_b             = 0 ;
	unsigned int temp_c             = 0 ;
	unsigned int only_dump_PSD      = 0 ;
	unsigned int RDQS_IPRD_TAP_NO         = 0 ;

	if (DRAM_ID == 0) {
		prn_string("DPCU_DT_INFO : ----- DUMP DRAM-0 delay line status -----\n\n") ;
	} else {
		prn_string("DPCU_DT_INFO : ----- DUMP DRAM-1 delay line status -----\n\n") ;
	}

	// -------------------------------------------------------
	// 0. DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	select_SDC_PHY_GRP(DRAM_ID, &SDC_BASE_GRP, &PHY_BASE_GRP);

	// DUMP SSCPLL Speed
	prn_string("DPCU_DT_INFO : \t********** DRAM SPEED **********\n") ;
	temp_a = (SP_REG(PHY_BASE_GRP, 12) >> 0) & 0x3F;
	prn_string("  SSCPLL Setting =");
	prn_byte(temp_a);

	// DUMP SDCTRL parameter
	prn_string("DPCU_DT_INFO : \t********** SDCTRL Setting **********\n") ;
	temp_a = (SP_REG(SDC_BASE_GRP, 11) >> 25) & 0x3F;
	temp_b = (SP_REG(SDC_BASE_GRP, 11) >> 20) & 0x1F;
	temp_c = (SP_REG(SDC_BASE_GRP, 11) >>  8) & 0x0F;

	prn_string("  STR_DQS_IN=");
	prn_decimal(temp_b);
	prn_string("  EXT_CL_CNT=");
	prn_decimal(temp_a);
	prn_string("  INT_WL_CNT=");
	prn_decimal(temp_c);
	prn_string("  \n\n");

	// DUMP CK0BD
	prn_string("DPCU_DT_INFO : \t********** DDRPHY Setting **********\n") ;
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
	temp_c = (temp_a >> 1) & 0x01 ;
	prn_string("DPCU_DT_INFO : \t********** DUMP APHY INIT flag **********\n") ;
	prn_string("  Init  done flag =");
	prn_decimal(temp_b);
	prn_string("(0 : means don't init, 1 : means init  done) \n");
	prn_string("  Init error flag =");
	prn_decimal(temp_c);
	prn_string("(0 : means  init pass, 1 : means init error) \n\n");

	// initial error
	if (temp_c == 1) {
		prn_string("DPCU_DT_INFO : \t********** DUMP APHY INIT error information **********\n") ;
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

	prn_string("DPCU_DT_INFO : \t********** DUMP Training flag **********\n") ;
	prn_string("  Training  done flag =   ");
	prn_decimal(temp_a);
	prn_string("(0:don't train,     1 : means training  done) \n");
	prn_string("  Training error flag =");
	prn_byte(temp_b);
	prn_string("(0:train  pass, other : means training error) \n\n");

	// training error
	if (temp_b != 0) {
		prn_string("DPCU_DT_INFO : \t********** DUMP APHY DX0 training error information **********\n") ;
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

		prn_string("DPCU_DT_INFO : \t********** DUMP APHY DX1 training error information **********\n") ;
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
	prn_string("DPCU_DT_INFO : \t********** DUMP initial DDR period **********\n") ;
	prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 3) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 3) >> 0) & 0xFF;
	prn_string("\tDX0 :   WL_IPRD =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   WL_IPRD =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 3) >> 8) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 3) >> 8) & 0xFF;
	prn_string("\tDX0 :   RG_IPRD =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   RG_IPRD =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 3) >> 16) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 3) >> 16) & 0xFF;
	prn_string("\tDX0 : RDQS_IPRD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : RDQS_IPRD =");
	prn_byte(temp_b);
	prn_string("\n") ;

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
	prn_string("\n\n") ;

	// DUMP PSD register
	prn_string("DPCU_DT_INFO : \t********** DUMP Training PSD status **********\n") ;
	prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 4) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 4) >> 0) & 0xFF;
	prn_string("\tDX0 :   WL_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   WL_PSD =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 4) >> 8) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 4) >> 8) & 0xFF;
	prn_string("\tDX0 :   WL_SEL =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   WL_SEL =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 6) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 6) >> 0) & 0xFF;
	prn_string("\tDX0 : REYE_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : REYE_PSD =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 7) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 7) >> 0) & 0xFF;
	prn_string("\tDX0 : WEYE_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : WEYE_PSD =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 14) >> 0) & 0x1F;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 14) >> 0) & 0x1F;
	prn_string("\tDX0 :   RG_RSL =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   RG_RSL =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 14) >> 8) & 0x03;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 14) >> 8) & 0x03;
	prn_string("\tDX0 :   RG_PHA =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   RG_PHA =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 14) >> 16) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 14) >> 16) & 0xFF;
	prn_string("\tDX0 :   RG_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 :   RG_PSD =");
	prn_byte(temp_b);
	prn_string("\n\n") ;

	prn_string("DPCU_DT_INFO : \t********** DUMP RG L-side & R-side status **********\n") ;
	prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 0) & 0x1F;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 0) & 0x1F;
	prn_string("\tDX0 : L_SIDE_RSL =");
	prn_byte(temp_a);
	prn_string("\tDX1 : L_SIDE_RSL =");
	prn_byte(temp_b);
	prn_string("\n") ;

	data_byte_0_RDQSG_left_total_tap = temp_a * RDQS_IPRD_TAP_NO;
	data_byte_1_RDQSG_left_total_tap = temp_b * RDQS_IPRD_TAP_NO;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 5) & 0x03;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 5) & 0x03;
	prn_string("\tDX0 : L_SIDE_PHA =");
	prn_byte(temp_a);
	prn_string("\tDX1 : L_SIDE_PHA =");
	prn_byte(temp_b);
	prn_string("\n") ;

	data_byte_0_RDQSG_left_total_tap = (temp_a * RDQS_IPRD_TAP_NO / 2) + data_byte_0_RDQSG_left_total_tap;
	data_byte_1_RDQSG_left_total_tap = (temp_b * RDQS_IPRD_TAP_NO / 2) + data_byte_1_RDQSG_left_total_tap;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 8) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 8) & 0xFF;
	prn_string("\tDX0 : L_SIDE_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : L_SIDE_PSD =");
	prn_byte(temp_b);
	prn_string("\n") ;

	data_byte_0_RDQSG_left_total_tap = temp_a + data_byte_0_RDQSG_left_total_tap;
	data_byte_1_RDQSG_left_total_tap = temp_b + data_byte_1_RDQSG_left_total_tap;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 16) & 0x1F;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 16) & 0x1F;
	prn_string("\tDX0 : R_SIDE_RSL =");
	prn_byte(temp_a);
	prn_string("\tDX1 : R_SIDE_RSL =");
	prn_byte(temp_b);
	prn_string("\n") ;

	data_byte_0_RDQSG_right_total_tap = temp_a * RDQS_IPRD_TAP_NO;
	data_byte_1_RDQSG_right_total_tap = temp_b * RDQS_IPRD_TAP_NO;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 21) & 0x03;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 21) & 0x03;
	prn_string("\tDX0 : R_SIDE_PHA =");
	prn_byte(temp_a);
	prn_string("\tDX1 : R_SIDE_PHA =");
	prn_byte(temp_b);
	prn_string("\n") ;

	data_byte_0_RDQSG_right_total_tap = (temp_a * RDQS_IPRD_TAP_NO / 2) + data_byte_0_RDQSG_right_total_tap;
	data_byte_1_RDQSG_right_total_tap = (temp_b * RDQS_IPRD_TAP_NO / 2) + data_byte_1_RDQSG_right_total_tap;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 15) >> 24) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 15) >> 24) & 0xFF;
	prn_string("\tDX0 : R_SIDE_PSD =");
	prn_byte(temp_a);
	prn_string("\tDX1 : R_SIDE_PSD =");
	prn_byte(temp_b);
	prn_string("\n\n") ;

	data_byte_0_RDQSG_right_total_tap = temp_a + data_byte_0_RDQSG_right_total_tap;
	data_byte_1_RDQSG_right_total_tap = temp_b + data_byte_1_RDQSG_right_total_tap;

	prn_string("DPCU_DT_INFO : \t********** DUMP R/W EYE status **********\n") ;
	prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 16) >> 0) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 16) >> 0) & 0xFF;
	prn_string("\tDX0 : REYE_MIN =");
	prn_byte(temp_a);
	prn_string("\tDX1 : REYE_MIN =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 16) >> 8) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 16) >> 8) & 0xFF;
	prn_string("\tDX0 : REYE_MAX =");
	prn_byte(temp_a);
	prn_string("\tDX1 : REYE_MAX =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 16) >> 16) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 16) >> 16) & 0xFF;
	prn_string("\tDX0 : WEYE_MIN =");
	prn_byte(temp_a);
	prn_string("\tDX1 : WEYE_MIN =");
	prn_byte(temp_b);
	prn_string("\n") ;

	temp_a = (SP_REG(PHY_BASE_GRP + 2, 16) >> 24) & 0xFF;
	temp_b = (SP_REG(PHY_BASE_GRP + 3, 16) >> 24) & 0xFF;
	prn_string("\tDX0 : WEYE_MAX =");
	prn_byte(temp_a);
	prn_string("\tDX1 : WEYE_MAX =");
	prn_byte(temp_b);
	prn_string("\n\n") ;

	if (!only_dump_PSD) {
		prn_string("DPCU_DT_INFO : \t********** DUMP REYE BDD status **********\n") ;
		prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 11) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 11) >> 0) & 0x3F;
		prn_string("\tDX0 : RDQS_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQS_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 12) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 12) >> 0) & 0x3F;
		prn_string("\tDX0 : RDQ0_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ0_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 12) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 12) >> 8) & 0x3F;
		prn_string("\tDX0 : RDQ1_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ1_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 12) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 12) >> 16) & 0x3F;
		prn_string("\tDX0 : RDQ2_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ2_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 12) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 12) >> 24) & 0x3F;
		prn_string("\tDX0 : RDQ3_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ3_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 13) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 13) >> 0) & 0x3F;
		prn_string("\tDX0 : RDQ4_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ4_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 13) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 13) >> 8) & 0x3F;
		prn_string("\tDX0 : RDQ5_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ5_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 13) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 13) >> 16) & 0x3F;
		prn_string("\tDX0 : RDQ6_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ6_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 13) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 13) >> 24) & 0x3F;
		prn_string("\tDX0 : RDQ7_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : RDQ7_BDD =");
		prn_byte(temp_b);
		prn_string("\n\n") ;

		prn_string("DPCU_DT_INFO : \t********** DUMP WEYE BDD status **********\n") ;
		prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 8) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 8) >> 0) & 0x3F;
		prn_string("\tDX0 :     WDM_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :     WDM_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 8) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 8) >> 8) & 0x3F;
		prn_string("\tDX0 :    WDQS_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQS_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 8) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 8) >> 16) & 0x3F;
		prn_string("\tDX0 : WDQS_OE_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 : WDQS_OE_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 8) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 8) >> 24) & 0x3F;
		prn_string("\tDX0 :  WDQ_OE_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :  WDQ_OE_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 9) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 9) >> 0) & 0x3F;
		prn_string("\tDX0 :    WDQ0_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ0_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 9) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 9) >> 8) & 0x3F;
		prn_string("\tDX0 :    WDQ1_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ1_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 9) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 9) >> 16) & 0x3F;
		prn_string("\tDX0 :    WDQ2_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ2_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 9) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 9) >> 24) & 0x3F;
		prn_string("\tDX0 :    WDQ3_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ3_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 10) >> 0) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 10) >> 0) & 0x3F;
		prn_string("\tDX0 :    WDQ4_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ4_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 10) >> 8) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 10) >> 8) & 0x3F;
		prn_string("\tDX0 :    WDQ5_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ5_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 10) >> 16) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 10) >> 16) & 0x3F;
		prn_string("\tDX0 :    WDQ6_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ6_BDD =");
		prn_byte(temp_b);
		prn_string("\n") ;

		temp_a = (SP_REG(PHY_BASE_GRP + 2, 10) >> 24) & 0x3F;
		temp_b = (SP_REG(PHY_BASE_GRP + 3, 10) >> 24) & 0x3F;
		prn_string("\tDX0 :    WDQ7_BDD =");
		prn_byte(temp_a);
		prn_string("\tDX1 :    WDQ7_BDD =");
		prn_byte(temp_b);
		prn_string("\n\n") ;
	}

	prn_string("\t-----------------------------------------------------\n\n") ;
	// finish
	prn_string("DPCU_INFO : ----- DPCU dump done -----\n\n") ;
} // end function => DPCU_DT_RESULT_DUMP


// ***********************************************************************
// * FUNC      : ASSERT_SDC_PHY_RESET
// * PARAM     : DRAM_ID
// * PURPOSE   : assert reset
// ***********************************************************************
void assert_sdc_phy_reset(unsigned int DRAM_ID)
{
#ifdef PLATFORM_GEMINI
	if (DRAM_ID == 0) {
		SP_REG(0, 17) |= (
						 (1 << 14) | // SDCTRL0_RESET
						 (1 << 16)   // DDR_PHY0_RESET
					 );
	}
#endif
} // end function => assert_sdc_phy_reset

// ***********************************************************************
// * FUNC      : release_sdc_phy_reset
// * PARAM     : void
// * PURPOSE   : release reset
// ***********************************************************************
void release_sdc_phy_reset(void)
{
#ifdef PLATFORM_GEMINI
	SP_REG(0, 17) &= ~(
					 (1 << 14) | // SDCTRL0_RESET
					 (1 << 16)   // DDR_PHY0_RESET
				 );
#endif
} // end of release_sdc_phy_reset

// ***********************************************************************
// * FUNC      : do_system_reset_flow
// * PARAM     : DRAM_ID
// * PURPOSE   : do SDC & PHY reset flow
// ***********************************************************************
void do_system_reset_flow(unsigned int DRAM_ID)
{
	assert_sdc_phy_reset(DRAM_ID) ;
	wait_loop(1000) ;
	release_sdc_phy_reset() ;
} // end of do_system_reset_flow

// ***********************************************************************
// * FUNC      : SDCTRL_TRIMMER_TEST
// * PARAM     : DRAM_ID
// * PURPOSE   : trigger SDC.trimmer 4 modes to do random DRAM access test
// ***********************************************************************
int SDCTRL_TRIMMER_TEST_(unsigned int DRAM_ID, unsigned int start_addr, unsigned int TEST_DATA_LENGTH)
{
	// CBDMA
	// #define CBDMA0
	// #define DEBUG
	// #define DEBUG_ERROR
#ifdef DEBUG
	prn_string("\n[DRAM_SCAN] Trimmer-test by CBDMA !!!\n");
#endif
	volatile unsigned int *dram = 0;
	unsigned int dram_value = 0;

#ifdef CBDMA0
	volatile unsigned int *sram = (volatile unsigned int *)0x9E800040; // CBDMA0
	unsigned int DMA_GROUP = 26;
	unsigned int CBDMA_SRAM_SIZE = TEST_DATA_LENGTH;
#else
	volatile unsigned int *sram = (volatile unsigned int *)0x9E820000; // CBDMA1
	unsigned int DMA_GROUP = 27;
	unsigned int CBDMA_SRAM_SIZE = TEST_DATA_LENGTH;
#endif
	unsigned int CURRENT_PROCESS_ADDR = 0;
	unsigned int MAX_LOOP_TIMES = 100;
	unsigned int watting_status_counter = 0;
	unsigned int temp = 0;
	unsigned int i = 0, j = 0, loop_time = 0;
	unsigned int WAIT_TIMEOUT = 50000;
	unsigned int DATA_BYTE = 4;
	int isPassed = 1;
	unsigned int DATA_PATTERN[7] = {0xAAAAAAAA, 0x55555555, 0xAAAA5555, 0x5555AAAA, 0xAA57AA57, 0xFFDDFFDD, 0x55D755D7};
	unsigned int DATA_PATTERN_INDEX = 0;
	const unsigned int DATA_PATTERNS = sizeof(DATA_PATTERN) >> 2;


	// step1. SET DRAM ADDRESS
	if (0 == DRAM_ID) {
		dram = (volatile unsigned int *)(DRAM0_BASE_ADDR);
	} else {
		dram = (volatile unsigned int *)(DRAM1_BASE_ADDR);
	}


	for (loop_time = 0; loop_time < MAX_LOOP_TIMES; loop_time++) {
		// SET DRAM START ADDDRESS
		CURRENT_PROCESS_ADDR = (start_addr >> 2) + (loop_time * TEST_DATA_LENGTH);

		// step2. SET SRAM VALUE & CLEAR DRAM DATA
		// DEBUG
#ifdef DEBUG
		prn_string("SET SRAM VALUE & CLEAR DRAM DATA:\n");
#endif
		for (i = 0; i < TEST_DATA_LENGTH; i++) {
			sram[i] = DATA_PATTERN[i % DATA_PATTERNS];
			dram[CURRENT_PROCESS_ADDR + i] = 0;
#ifdef DEBUG
			prn_string("sram[");
			prn_dword(i);
			prn_string("]");
			prn_string("=");
			prn_dword(sram[i]);
			prn_string("\n");
#endif
		}

		// DEBUG
#ifdef DEBUG
		prn_string("before write DRAM Data by CBDMA:\n");
		for (i = 0; i < TEST_DATA_LENGTH; i++) {
			prn_string("dram[");
			prn_dword(CURRENT_PROCESS_ADDR + i);
			prn_string("]");
			prn_string("=");
			prn_dword(dram[CURRENT_PROCESS_ADDR + i]);
			prn_string("\n");
		}
#endif


		// step3. WRITE SRAM DATA TO DRAM
		SP_REG(DMA_GROUP, 2) = TEST_DATA_LENGTH * DATA_BYTE; // data length
#ifdef CBDMA0
		SP_REG(DMA_GROUP, 3) = 0x00000040; // set data start addr from SRAM
#else
		SP_REG(DMA_GROUP, 3) = 0x00000000; // set data start addr from SRAM
#endif
		SP_REG(DMA_GROUP, 4) = &dram[CURRENT_PROCESS_ADDR]; // set data end addr to dram
		SP_REG(DMA_GROUP, 8) = 0x000000FF; // set sdram size
		SP_REG(DMA_GROUP, 1) = 0x00000101; // write data from internal sram to main memory

		while (watting_status_counter < WAIT_TIMEOUT) {
			temp = SP_REG(DMA_GROUP, 1) >> 8;
			if (0 == temp) {
				break;
			}
			watting_status_counter++;
		}

		if (watting_status_counter >= WAIT_TIMEOUT) {
#ifdef DEBUG_ERROR
			prn_string("CBDMA write to dram failed !!!\n");
#endif
			isPassed = 0;
			break;
		}


		// DEBUG
#ifdef DEBUG
		prn_string("after write DRAM data by CBDMA:\n");
		for (i = 0; i < TEST_DATA_LENGTH; i++) {
			prn_string("dram[");
			prn_dword(CURRENT_PROCESS_ADDR + i);
			prn_string("]");
			prn_string("=");
			prn_dword(dram[CURRENT_PROCESS_ADDR + i]);
			prn_string("\n");
		}
#endif

		// step4. CLEAR SRAM DATA
		for (i = 0; i < TEST_DATA_LENGTH; i++) {
			sram[i] = 0;
		}
		// DEBUG
#ifdef DEBUG
		prn_string("CLEAR SRAM DATA:\n");
		for (i = 0; i < TEST_DATA_LENGTH; i++) {
			prn_string("sram[");
			prn_dword(i);
			prn_string("]");
			prn_string("=");
			prn_dword(sram[i]);
			prn_string("\n");
		}
#endif


		// step5. READ DATA FROM DRAM TO SRAM
		watting_status_counter = 0;
		SP_REG(DMA_GROUP, 2) = TEST_DATA_LENGTH * DATA_BYTE; // data length
		SP_REG(DMA_GROUP, 3) = &dram[CURRENT_PROCESS_ADDR]; // set data start addr from DRAM
#ifdef CBDMA0
		SP_REG(DMA_GROUP, 4) = 0x00000040; // set data end addr to SRAM
#else
		SP_REG(DMA_GROUP, 4) = 0x00000000; // set data end addr to SRAM
#endif
		SP_REG(DMA_GROUP, 8) = 0x000000FF; // set sdram size
		SP_REG(DMA_GROUP, 1) = 0x00000102; // read data from main memory to internal sram
		while (watting_status_counter < WAIT_TIMEOUT) {
			temp = SP_REG(DMA_GROUP, 1) >> 8;
			if (0 == temp) {
				break;
			}
			watting_status_counter++;
		}
		if (watting_status_counter >= WAIT_TIMEOUT) {
#ifdef DEBUG_ERROR
			prn_string("CBDMA read from dram failed !!!\n");
#endif
			isPassed = 0;
			break;
		}

		// STEP6. VERIFY SRAM DATA
#ifdef DEBUG
		prn_string("VERIFY SRAM DATA:\n");
#endif
		for (i = 0 ; i < TEST_DATA_LENGTH ; i++) {
#ifdef DEBUG
			prn_string("sram[");
			prn_dword(i);
			prn_string("]:");
			prn_dword(sram[i]);
			prn_string("\n");
#endif
			if (DATA_PATTERN[i % DATA_PATTERNS] != sram[i]) {
#ifdef DEBUG_ERROR
				prn_string("VERIFY SRAM DATA FAILED!!\n");
				prn_string("CURRENT_PROCESS_ADDR=");
				prn_dword(CURRENT_PROCESS_ADDR);
				prn_string("\nsram[");
				prn_dword(i);
				prn_string("]:");
				prn_dword(sram[i]);
				prn_string(", correct will be ");
				prn_dword(DATA_PATTERN[i % DATA_PATTERNS]);
				prn_string("\n");
#endif
				isPassed = 0;
				break;
			}
		}

		// step7. CLEAR SRAM & DRAM DATA
		for (i = 0; i < TEST_DATA_LENGTH; i++) {
			sram[i] = 0;
			dram[CURRENT_PROCESS_ADDR + i] = 0;
		}

		// step8. CHECK RESULT
		if (isPassed) {
#ifdef DEBUG
			prn_string("TRIM BY CBDMA PASSED\n\n");
#endif
		} else {
#ifdef DEBUG
			prn_string("TRIM BY CBDMA FAILED\n\n");
#endif
			break;
		}

	} // end for


	// step9. CLEAR SRAM & DRAM DATA
	for (i = 0; i < TEST_DATA_LENGTH; i++) {
		sram[i] = 0;
		dram[CURRENT_PROCESS_ADDR + i] = 0;
	}
	return isPassed;
} // end function => SDCTRL_TRIMMER_TEST


// ***********************************************************************
// * FUNC      : DPCU_CMD_ISSUE_SW_CMD
// * PARAM     : CMD, RANK,BANK,ADDR, DATA_MASK, DATA, TRIGGER
// * PURPOSE   : using CMD ISSUE issue
// ***********************************************************************
void DPCU_CMD_ISSUE_SW_CMD(unsigned int DRAM_ID, unsigned int CMD, unsigned int RANK, unsigned int BANK, unsigned int ADDR,
			   unsigned int SW_WRDATA_MASK, unsigned int SW_WRDATA1_HIGH, unsigned int SW_WRDATA1_LOW,
			   unsigned int SW_WRDATA0_HIGH, unsigned int SW_WRDATA0_LOW, unsigned int CMD_TRIGGER)
{
	unsigned int        temp            ;
	unsigned int        loop_x          ;
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0      ;
	select_SDC_PHY_GRP(DRAM_ID, &SDC_BASE_GRP, &PHY_BASE_GRP);


	temp = SP_REG(PHY_BASE_GRP + 1, 27) & 0xF0000000;
	// fill cmd , RANK, BANK, ADDR infor
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
// * PARAM     : DRAM_ID
// * PURPOSE   : to do the following sequences
// *           : (1). DDR_APHY initial sequence (CTCAL->SSCPLL->PZQ)
// ***********************************************************************
int dram_booting_flow(unsigned int DRAM_ID)
{
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0  ;
	unsigned int        wait_flag      = 0;	 // min
	unsigned int        aphy_select1_value = 0;
	unsigned int        aphy_select2_value = 0;
	prn_string(">>> enter dram_booting_flow for DRAM");
	prn_decimal(DRAM_ID);
	prn_string("\n");
	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	select_SDC_PHY_GRP(DRAM_ID, &SDC_BASE_GRP, &PHY_BASE_GRP);

	// -------------------------------------------------------
	// 0. Set DRAM-0 size in CBUS( CBUS to MBUS Bridge address decode/mapping ) , soft-configure-reg 13
	// DRAM-0 size : Group-1, reg-13 : bit[22:21] = 0 => 128Mb , 1=>256MB, 2=>512MB, 3=>1GB
	// DRAM-1 size : Group-1, reg-13 : bit[24:23] = 0 => 128Mb , 1=>256MB, 2=>512MB, 3=>1GB
	// -------------------------------------------------------
	SP_REG(1, 13) = SP_REG(1, 13) & (~(15 << 21));
	SP_REG(1, 13) = SP_REG(1, 13) | ((MO_SDRAM_A_SIZE << 21)) | ((MO_SDRAM_B_SIZE << 23));

	// -------------------------------------------------------
	// 1. DPCU_APHY_INIT setting => a001
	// -------------------------------------------------------
#ifdef SDRAM_FPGA
	// There are no APHY circuit in FPGA platform, so bypass this flow
#else
	if (0 == gResetMPLL)
		do_system_reset_flow(DRAM_ID);
	dbg_stamp(0xA000);


	SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_DPCU);
	// set MPLL_DIV to operation freq.
	SP_REG(PHY_BASE_GRP + 0, 12) = MPLL_CFG1_DEF | MPLL_DIV(gMPLL_DIV) ;
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
	SP_REG(PHY_BASE_GRP + 0, 1) = DPCU_AI_CFG0_SELECT1;
	// enable APHY_INIT start
	SP_REG(PHY_BASE_GRP + 0, 1) = DPCU_AI_CFG0_SELECT1 | AI_INIT_START(n_AI_INIT_START_EN);

	// wait aphy init done
	wait_flag   =   0   ;
	do {
		wait_flag   =   SP_REG(PHY_BASE_GRP + 0, 2) & 0x00000001;
	} while ((wait_flag == 0));

	rgst_value = (SP_REG(PHY_BASE_GRP + 0, 2) >> 8) & 0x0F;

	aphy_select1_value = rgst_value;
	// Disable DDR IO PAD Retention flag
	rgst_value = SP_REG(8, 0);
	// setting AI CFG
	SP_REG(PHY_BASE_GRP + 0, 1) = DPCU_AI_CFG0_SELECT2;
	// enable APHY_INIT start
	SP_REG(PHY_BASE_GRP + 0, 1) = DPCU_AI_CFG0_SELECT2 | AI_INIT_START(n_AI_INIT_START_EN);
	// wait aphy init done
	wait_flag   =   0   ;
	do {
		wait_flag   =   SP_REG(PHY_BASE_GRP + 0, 2) & 0x00000001;
	} while ((wait_flag == 0));
	rgst_value = (SP_REG(PHY_BASE_GRP + 0, 2) >> 8) & 0x0F;

	aphy_select2_value = rgst_value;
	aphy_select_value = (aphy_select1_value | aphy_select2_value);

	if (rgst_value != 0) {
		prn_string("<<< leave dram_booting_flow for DRAM");
		prn_decimal(DRAM_ID);
		prn_string("\n");
		return 0 ;
	}
#endif
	prn_string("<<< leave dram_booting_flow for DRAM");
	prn_decimal(DRAM_ID);
	prn_string("\n");
	return 1;
} // end of dram_booting_flow

// ***********************************************************************
// * FUNC      : dram_training_flow
// * PARAM     : DRAM_ID
// * PURPOSE   : to do the following sequences
// *           : (1). DPCU DRAM inital setting
// *           : (2). SDCTRL RGST setting
// *           : (3). DRAM initial setting by SDCTRL
// *           : (5). DDR_DPHY+APHY data training
// ***********************************************************************
int dram_training_flow(unsigned int DRAM_ID)
{
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0,
			    waiting_status_counter = 0;
	unsigned int        temp_1, temp_2;
	int i = 0;
	unsigned int        wait_flag      = 0;	 // min

	prn_string(">>> enter dram_training_flow for DRAM");
	prn_decimal(DRAM_ID);
	prn_string("\n");
	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	select_SDC_PHY_GRP(DRAM_ID, &SDC_BASE_GRP, &PHY_BASE_GRP);

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
		prn_decimal(DRAM_ID);
		prn_string("\n");
		return WAIT_FLAG_FAIL;
	}
	if (SP_REG(PHY_BASE_GRP + 1, 10) == 0x00) {
		;
		prn_string("<<< 2 leave dram_training_flow for DRAM");
		prn_decimal(DRAM_ID);
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

	dbg_stamp(0xA001) ;

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
	dbg_stamp(0xA002) ;
	// csim simulation only, for YES_QUICK_SIM and DPCU_FAST (speedup csim flow)
	// RGST_CI_RST_B_HIGH & RGST_CKE_HIGH
	// SP_REG(PHY_BASE_GRP+1, 15)  =   0x00F000FF      ;
	// RGST_CI_RST_B_LOW
	// SP_REG(PHY_BASE_GRP+1, 16)  =   0x00000FF0      ;
#else
	// IC - Real initial flow (ASIC & FPGA)
#endif

	dbg_stamp(0xA003) ;

	// trigger CMD_ISSUE DRAM_INIT sequence
	if (SP_REG(PHY_BASE_GRP + 1, 10) == 0x00) {
		prn_string("<<< 3 leave dram_training_flow for DRAM");
		prn_decimal(DRAM_ID);
		prn_string("\n");
		return WAIT_FLAG_FAIL;
	}
	// prn_string("\tbefore GRP((PHY_BASE_GRP+1),10) = "); prn_dword(SP_REG(PHY_BASE_GRP+1, 10)); prn_string("\n");
	SP_REG(PHY_BASE_GRP + 1, 10) = (SP_REG(PHY_BASE_GRP + 1, 10) & 0xFFFFFFEF) | 0x00000010;

	waiting_status_counter = 0;

	// wait CMD_ISSUE flag
#endif // endif - SDRAM_FPGA

	dbg_stamp(0xA004) ;

	// -------------------------------------------------------
	// 2. SDCTRL RGST setting => a002
	// -------------------------------------------------------
#ifdef DISABLE_L3_CACHE
	SP_REG(24, 0) = SP_REG(24, 0) & (~(3 << 12));
#endif
#ifdef DISABLE_L3_ACCESS_DRAM_TO_MBUS
	SP_REG(SDC_BASE_GRP + 7, 5) = (SP_REG(SDC_BASE_GRP + 7, 5) & ~(1 << 1));
#endif
	// #ifdef L3C_SIZE_64KB
	//   SP_REG(SDC_BASE_GRP+7, 1) =   0x0010                      ;
	// #endif
	SP_REG(SDC_BASE_GRP - 1, 1) = MCPP_BKLEN_CFG_VAL;
	SP_REG(SDC_BASE_GRP - 1, 7) = MCPP_LIFE_VAL;
	SP_REG(SDC_BASE_GRP + 0, 3) = DATA_NO_LIM_CFG_VAL; // ASIC ONLY
	SP_REG(SDC_BASE_GRP + 0, 8) = AREF_REG_DIS_VAL;
	SP_REG(SDC_BASE_GRP + 0, 9) = AREF_INTVAL(nAREF_INTVAL); // ASIC DIFF

	if (DRAM_ID == 1) {
		SP_REG(SDC_BASE_GRP + 0, 5) = SDRAM1_SIZE_TYPE_VAL; // ASIC DIFF
	} else {
		SP_REG(SDC_BASE_GRP + 0, 5) = SDRAM0_SIZE_TYPE_VAL; // ASIC DIFF
	}
	SP_REG(SDC_BASE_GRP + 0, 6) = SD_SYS_MISC;
	SP_REG(SDC_BASE_GRP + 0, 7) = 0;
	if (DRAM_ID == 1) {
		SP_REG(SDC_BASE_GRP + 0, 11) = SCAN_SD1_ACC_LATENCY;
	} else {
		SP_REG(SDC_BASE_GRP + 0, 11) = ((gEXTRA_CL_CNT << 25) | (gSTR_DQS_IN << 20) | (gWL_CNT << 8));
	}
	SP_REG(SDC_BASE_GRP + 0, 12) = SD_PAR_INTERVAL_VAL;
	SP_REG(SDC_BASE_GRP + 0, 13) = SD_PAR_TIMING0_VAL;
	SP_REG(SDC_BASE_GRP + 0, 14) = SD_PAR_TIMING1_VAL;
	SP_REG(SDC_BASE_GRP + 0, 15) = SD_PAR_TIMING2_VAL;
	SP_REG(SDC_BASE_GRP + 0, 16) = SD_PAR_TIMING3_VAL;
	SP_REG(SDC_BASE_GRP + 0, 19) = ODT_SIGNAL_TIMING_VAL;
	SP_REG(SDC_BASE_GRP + 0, 27) = ZQCL_CFG_VAL;
	SP_REG(SDC_BASE_GRP + 0, 28) = ZQCS_CFG_VAL;
	SP_REG(SDC_BASE_GRP + 0, 29) = SCPP_CFG_VAL;
	// -------------------------------------------------------
	// 3. SDCTRL-0 initial task sequence
	// -------------------------------------------------------
	// switch DFI path to SDCTRL
	SP_REG(SDC_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
	// assert SDCTRL's DDR3_RST & DDR3_CKE register
	SP_REG(SDC_BASE_GRP + 0, 31) = DDR3_RST_CKE_DISABLE;
	wait_loop(10)   ;   // wait for DRAM RST

	// release SDCTRL's DDR3_RST & DDR3_CKE register
	SP_REG(SDC_BASE_GRP + 0, 31) = DDR3_RST_EN;
	wait_loop(5)   ;    // wait for RST high

	SP_REG(SDC_BASE_GRP + 0, 31) = DDR3_RST_CKE_EN;
	wait_loop(5)   ;    // wait for CKE high

	// DRAM MRS SETTING
	// DDR2: PREA -> MRS2 -> MRS3 -> MRS1 ->
	//       MRS0(DLL_RESET0) -> MRS0(DLL_RESET1) -> MRS1(OCD DFLT) -> MRS1(OCD EXIT)
	// DDR3: MRS2 -> MRS3 -> MRS1 -> MRS0

#ifdef MPEG_DRAM_TYPE_DDR2
	// PREA
	SP_REG(SDC_BASE_GRP + 0, 8) = PREA_CMD_TRIG_VAL;
#endif

	// MRS_MODE2
	SP_REG(SDC_BASE_GRP + 0, 17) = MRS_MODE2_VAL_SET;
	SP_REG(SDC_BASE_GRP + 0, 8) = MRS_CMD_TRIG_VAL;
	// MRS_MODE3
	SP_REG(SDC_BASE_GRP + 0, 17) = MRS_MODE3_VAL_SET;
	SP_REG(SDC_BASE_GRP + 0, 8) = MRS_CMD_TRIG_VAL;
	// MRS_MODE1
	// OCD Exit & DLL_ENABLE
	SP_REG(SDC_BASE_GRP + 0, 17) = MRS_MODE1_VAL_SET;
	SP_REG(SDC_BASE_GRP + 0, 8) = MRS_CMD_TRIG_VAL;

	// MRS_MODE0
	// SDRAM DLL Reset
	SP_REG(SDC_BASE_GRP + 0, 17) = MRS_MODE0_VAL_SET;
	SP_REG(SDC_BASE_GRP + 0, 8) = MRS_CMD_TRIG_VAL;

#ifdef MPEG_DRAM_TYPE_DDR2
	SP_REG(SDC_BASE_GRP + 0, 8) = PREA_CMD_TRIG_VAL;
	SP_REG(SDC_BASE_GRP + 0, 8) = AREF_CMD_TRIG_VAL;
	SP_REG(SDC_BASE_GRP + 0, 8) = AREF_CMD_TRIG_VAL;
	// Write_recovery_6T , SDRAM DLL Without Reset
	SP_REG(SDC_BASE_GRP + 0, 17) = MRS_MODE0_VAL_SET_1;
	SP_REG(SDC_BASE_GRP + 0, 8) = MRS_CMD_TRIG_VAL;
	// OCD Default
	SP_REG(SDC_BASE_GRP + 0, 17) = MRS_MODE1_VAL_SET_1;
	SP_REG(SDC_BASE_GRP + 0, 8) = MRS_CMD_TRIG_VAL;
	// OCD Exit
	SP_REG(SDC_BASE_GRP + 0, 17) = MRS_MODE1_VAL_SET;
	SP_REG(SDC_BASE_GRP + 0, 8) = MRS_CMD_TRIG_VAL;
#endif
#ifdef DRAM_ZQ_CFG
	// ZQCL setting
	SP_REG(SDC_BASE_GRP + 0, 8) = ZQCL_CMD_TRIG_VAL;
#endif

	dbg_stamp(0xA002) ;

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
	// DRAM_ID, CMD , RANK, BANK, ADDR, DM, DATA1_HIGH, DATA1_LOW, DATA0_HIGH, DATA0_LOW, TRIGGER
	DPCU_CMD_ISSUE_SW_CMD(DRAM_ID, 0x08, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);  // COMMIT and TIGGER // ODT ON SPECIAL COMMAND
	// step b issue CMD ISSUE ODT on command
	// DRAM_ID, CMD , RANK, BANK, ADDR, DM, DATA1_HIGH, DATA1_LOW, DATA0_HIGH, DATA0_LOW, TRIGGER
	DPCU_CMD_ISSUE_SW_CMD(DRAM_ID, 0x08, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);  // COMMIT and TIGGER // ODT ON SPECIAL COMMAND
	wait_flag = 0   ;
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

	// refresh period
	rgst_value = SP_REG(PHY_BASE_GRP + 1, 1);
	rgst_value &= (~(0x7FFF) << 8);
	rgst_value |= 0x0A20 << 8;
	SP_REG(PHY_BASE_GRP + 1, 1) = rgst_value;
	rgst_value = SP_REG(PHY_BASE_GRP + 1, 1);
	prn_string("\trefresh period = ");
	prn_dword(rgst_value);
	SP_REG(PHY_BASE_GRP + 1, 1) = DPCU_DT_GO;

	// wait_dpcu_1st_training
	wait_flag = 0   ;
	do {
		wait_loop(1000)   ;
		wait_flag   =   SP_REG(PHY_BASE_GRP + 1, 0) & 0x01;
		// prn_string("111 training SP_REG(PHY_BASE_GRP+1, 0)="); prn_dword(SP_REG(PHY_BASE_GRP+1, 0)); prn_string("\n");
	} while ((wait_flag == 0));

	// check_1st_dt_result
	rgst_value = (SP_REG(PHY_BASE_GRP + 1, 0) >> 8) & 0x3F;

	if (rgst_value != 0x00) {
		// dpcu training failed
		SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
		dbg_stamp(0xDEADA003) ;
		prn_string("<<< 4 leave dram_training_flow for DRAM");
		prn_decimal(DRAM_ID);
		prn_string("\n");
		return 0 ;
	} // end- if check dt result

	// step-3: dpcu_2nd_training - trigger RDQSG training
	SP_REG(PHY_BASE_GRP + 1, 1) = DPCU_DT_CFG0 | DT_RG(n_RG_EN) | DPCU_TRAIN_START(n_DT_START);
	wait_loop(10000)   ;   // wait for clear DPCU DT done

	// rgst_value = SP_REG(PHY_BASE_GRP+1, 0);
	// prn_string("DPCU mid 2sec Training Status G37.0: 0x"); prn_dword(rgst_value); prn_string("!!\n");

	// wait_dpcu_2nd_training
	wait_flag = 0   ;
	do {
		wait_flag   = (SP_REG(PHY_BASE_GRP + 1, 0) & 0x01);
		// prn_string("222 training SP_REG(PHY_BASE_GRP+1, 0)="); prn_dword(SP_REG(PHY_BASE_GRP+1, 0)); prn_string("\n");
	} while ((wait_flag == 0));

	// check_2nd_dt_result
	rgst_value = (SP_REG(PHY_BASE_GRP + 1, 0) >> 9) & 0x01;

	if (rgst_value != 0x00) {
		// dpcu training failed
		SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
		dbg_stamp(0xDEADA003)   ;
		prn_string("<<< 5 leave dram_training_flow for DRAM");
		prn_decimal(DRAM_ID);
		prn_string("\n");
		return 0 ;
	} // end- if check dt result

	// move switch SDCTRL path before dump
	SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
	DPCU_DT_RESULT_DUMP(DRAM_ID); // SET_RDQS_IPRD_TAP_NO(DRAM_ID);

	// Ellison : 20140815 : Check RDQSG Eye boundary result of Each byte correct or not !? ( Left-side < Right-side)
	if ((data_byte_0_RDQSG_left_total_tap >= data_byte_0_RDQSG_right_total_tap) ||
	    (data_byte_1_RDQSG_left_total_tap >= data_byte_1_RDQSG_right_total_tap)) {
		SP_REG(PHY_BASE_GRP + 0, 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
		prn_string("\tRDQSG Training result : Left side boundary > Right side boundary !!!\n");
		prn_string("<<< 6 leave dram_training_flow for DRAM");
		prn_decimal(DRAM_ID);
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
		prn_decimal(DRAM_ID);
		prn_string("\n");
		return 0 ;
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
		prn_decimal(DRAM_ID);
		prn_string("\n");
		return 0 ;
	} // end- if RG_PSD is too big or small

	// step-4: switch path to SDCTRL
	/* move this setting before DPCU_DT_RESULT_DUMP, it will switch DFI path to SDCTRL.
	 * That is, auto refresh will be releaseed, and dump cost time. We need auto refresh earlier.
	SP_REG(PHY_BASE_GRP+0, 0) =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL); */
#endif // SDRAM_FPGA

	dbg_stamp(0xA003); // phy-training done !!!
	// issue PREA & enable AREF after training
	SP_REG(SDC_BASE_GRP + 0, 8) = PREA_CMD_TRIG_VAL;
	SP_REG(SDC_BASE_GRP + 0, 8) = AREF_REG_EN_VAL;
	// SDRAM power initial task completed:
	SP_REG(SDC_BASE_GRP + 0, 18) = 0x0002;
	rgst_value = SP_REG(SDC_BASE_GRP + 0, 18);

	prn_string("<<< leave 8 dram_training_flow for DRAM");
	prn_decimal(DRAM_ID);
	prn_string("\n");
	return 1;
} // end of dram_training_flow

// ***********************************************************************
// * FUNC      : dram_init
// * PARAM     : DRAM_ID
// * PURPOSE   : to do the following sequences
// *           : (1). DDR_APHY initial sequence (CTCAL->SSCPLL->PZQ)
// *           : (2). SDCTRL RGST setting
// *           : (3). DRAM initial setting by SDCTRL
// *           : (4). DDR_DPHY+APHY data training
// ***********************************************************************
int dram_init(unsigned int DRAM_ID)
{
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0  ;
	unsigned int        temp_1         = 0 ;
	unsigned int        temp_2         = 0 ;
	unsigned int        temp_3         = 0 ;
	unsigned int        package_256_flag;  // this flag only using in DRAM_ID == 1
	unsigned int        max_init_fail_cnt = 15;
	unsigned int        loop_time       ;
	unsigned int        ret = 0;
	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	select_SDC_PHY_GRP(DRAM_ID, &SDC_BASE_GRP, &PHY_BASE_GRP);

	loop_time = 0;
	// ckobd_training_flag = 0;
	ckobd_re_training_number = 0;


	// 20140728 mazar : add max_init_fail_cnt for sometime training fail
	for (loop_time = 0; loop_time < max_init_fail_cnt; loop_time++) {
		ckobd_re_training_number = loop_time;
		// ckobd_training_flag = 1;
		package_256_flag = 1; // this flag only using in DRAM_ID == 1

		// dram_bootint_flow pass return 1, error return 0;
		// 20140727 mazar : do not add any action if we encounter APHY INIT ERR, just dump error flag
DRAM_BOOT_FLOW_AGAIN:
		if (!dram_booting_flow(DRAM_ID)) {
			// error check flow
			// 1. DRAM_ID = 1 and all initial error flag are asserted (note)
			// => we consider this is 216 package, so package_256_flag = 0;
			// note : SSCPLL isn't assert, so check PZQ, CTCAL, DDL error flag
			// and pass all DRAM 1 training flow
			// 2. DRAM_ID = 1 and not all initial error flag are asserted => we consider this is 256 package, so package_256_flag = 1;
			// and print the error message
			// 3. DRAM_ID = 0, print the error message
			prn_string("DPCU_INFO : \t********** DUMP APHY INIT************************\n");
			prn_string("aphy_select_value =");
			prn_dword(aphy_select_value);
			temp_3 = (SP_REG(PHY_BASE_GRP, 2) >> 8) & 0x0F;
			if ((DRAM_ID == 1) && (aphy_select_value == 0x0D)) {
				// case 1, we think this is 216 pin package, and don't need to dump initial error message
				// only check PZQ, CTCAL, DDL error flag
				package_256_flag = 0 ;
				// prn_string("is this 216 package !?\n");
			} else {
				// case 2 or case 3, we dump the initial error flag
				// DUMP INIT flag
				prn_string("DPCU_INFO : \t********** DUMP APHY INIT error infor @ loop_time = ");
				prn_decimal(loop_time);
				prn_string(" ***\n");
				temp_1 = (SP_REG(PHY_BASE_GRP, 2) >> 8) & 0x01;
				temp_2 = (SP_REG(PHY_BASE_GRP, 2) >> 9) & 0x01;
				prn_string("\tCTCAL_ERR flag =");
				prn_decimal(temp_1);
				prn_string("\tSSCPLL_ERR flag =");
				prn_decimal(temp_2);
				prn_string("\n");
				temp_1 = (SP_REG(PHY_BASE_GRP, 2) >> 10) & 0x01;
				temp_2 = (SP_REG(PHY_BASE_GRP, 2) >> 11) & 0x01;
				prn_string("\tDDL_ERR flag =");
				prn_decimal(temp_1);
				prn_string("\tPZQ_ERR flag =");
				prn_decimal(temp_2);
				prn_string("\n");
				prn_string("DPCU_INFO : \t********** DUMP APHY INIT error information end **********\n") ;
				goto DRAM_BOOT_FLOW_AGAIN;
			}
		} else {
			// prn_string("DRAM-"); prn_decimal(DRAM_ID); prn_string("booting PASS @ loop_time =");
			// prn_decimal(loop_time); prn_string("!!\n");
		}
		prn_string("(DRAM_ID)=");
		prn_decimal(DRAM_ID);
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
		// check DRAM ID , 0 : do training,
		//                 1 : check package_256_flag, 0 : pass training flow
		//                                             1 : do training flow
		// 20140727 mazar :  do not add any action if we encounter APHY Training ERR, just dump error flag
		temp_3 = (SP_REG(PHY_BASE_GRP, 2) >> 8) & 0x01;
		if (((DRAM_ID == 1) && (package_256_flag == 0)) || (temp_3 == 1)) {
			// case 1 : (DRAM_ID==1) && (package_256_flag ==0 ) : we think this is 216 package, so don't do data training
			// case 2 : (temp_3 == 1)                           : because SSCPLL isn't assert error flag, we consider CTCAL error flag
			// as SSCPLL error flag, so don't do data training too
			return PACKAGE_216; // break for_loop if encounter case1 or case2
		}

		prn_string("GRP(PHY_BASE_GRP, 9) =  ");
		prn_dword(SP_REG(PHY_BASE_GRP, 9));
		prn_string("\n ");
		ret = dram_training_flow(DRAM_ID);

		if (ret == WAIT_FLAG_FAIL) {
			prn_string("wait flag or register G(37,10) fail!!!!\n");
			// goto DRAM_BOOT_FLOW_AGAIN;
			return FAIL;
		} else if (ret == 0) {
			// (DRAM_ID=0) or (DRAM_ID=1 and package_256_flag==1) => do training flow
			// dump error message

			prn_string("DPCU_INFO : \t********** DUMP init & training error info @ loop_time = ");
			prn_decimal(loop_time);
			prn_string(" ***\n") ;
			DPCU_DT_RESULT_DUMP(DRAM_ID);
		} else {
			// training pass
			// double check RSL result for SDCTRL setting
			rgst_value = (SP_REG(PHY_BASE_GRP + 2, 14) >> 0) & 0x1F;
			// prn_string("\tDX0 :   RG_RSL ="); prn_byte(temp_1); prn_string("\tDX1 :   RG_RSL ="); prn_byte(temp_2); prn_string("\n") ;
			if (rgst_value == 1) {
				// modify SDCTRL STR_DQS_IN if RSL ==1 (STR_DQS_IN -1)
				temp_1 = (SP_REG(SDC_BASE_GRP, 11) >> 25) & 0x3F;
				temp_2 = (SP_REG(SDC_BASE_GRP, 11) >> 20) & 0x1F;
				temp_3 = (SP_REG(SDC_BASE_GRP, 11) >>  8) & 0x0F;
				temp_2 = temp_2 - 1 ;
				SP_REG(SDC_BASE_GRP, 11) = (temp_1 << 25) | (temp_2 << 20) | (temp_3 << 8);
			} else if (rgst_value == 3) {
				// modify SDCTRL STR_DQS_IN if RSL ==1 (STR_DQS_IN +1)
				temp_1 = (SP_REG(SDC_BASE_GRP, 11) >> 25) & 0x3F;
				temp_2 = (SP_REG(SDC_BASE_GRP, 11) >> 20) & 0x1F;
				temp_3 = (SP_REG(SDC_BASE_GRP, 11) >>  8) & 0x0F;
				temp_2 = temp_2 + 1 ;
				SP_REG(SDC_BASE_GRP, 11) = (temp_1 << 25) | (temp_2 << 20) | (temp_3 << 8);
			}
			int i = 0;
			int pass_count = 0;
			unsigned int TEST_ADDRESS[3] = {0x00000000, 0x08000000, 0x0C800000};
			const unsigned int TEST_COUNT = sizeof(TEST_ADDRESS) >> 2;
			for (i = 0; i < TEST_COUNT; i++) {
				if (SDCTRL_TRIMMER_TEST_(DRAM_ID, TEST_ADDRESS[i], 0x0100) != 0)
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
			prn_decimal(DRAM_ID);
			prn_string("initial failed\n\n");
			// RID_FAIL();
			// while(1); // robert: return fail rather than hang
			return FAIL;
		} // all loop training fail
	} // end of for loop :: loop_time for initial & training time control

	prn_string("DRAM-");
	prn_decimal(DRAM_ID);
	prn_string("initial done !!!!\n\n") ;
	// dram_refine_flow( DRAM_ID );
	return SUCCESS;
} // end dram_init


#ifdef SW_REFINE_DT_RESULT
// ***********************************************************************
// * FUNC      : DRAM_WRITE_CLEAN
// * PARAM     : DRAM_ID
// * PURPOSE   : clean dram content of test region
// ***********************************************************************
void DRAM_WRITE_CLEAN(unsigned int DRAM_TEST_SIZE, unsigned int DRAM_ID)
{
	unsigned int cpu_wr_index = 0 ;

	volatile unsigned int *ram_0 = (volatile unsigned int *)DRAM0_BASE_ADDR;
	volatile unsigned int *ram_1 = (volatile unsigned int *)DRAM1_BASE_ADDR;

	for (cpu_wr_index = 0 ; cpu_wr_index < DRAM_TEST_SIZE ; cpu_wr_index++) {
		if (DRAM_ID == 0) {
			ram_0[cpu_wr_index] = 0 ;
		} else {
			ram_1[cpu_wr_index] = 0 ;
		}
	} // end for


} // end function => DRAM_WRITE_CLEAN

int DRAM_RW_CHECK(unsigned int value, unsigned int answer, int debug)
{
	int ret = 0;

	if (value != answer) {
		if (debug) {
			prn_string("\t\tvalue=");
			prn_dword0(value);
			prn_string(" correct will be ");
			prn_dword0(answer);
			prn_string("\n");
		}
		ret = -1;
	}
	return ret;

} // end - DRAM_ACC_TEST

int DRAM_SEQUENTIAL_RW_TEST(unsigned int DRAM_TEST_SIZE, int debug)
{
	int             ret = 0 ;
	unsigned int    i = 0   ;
	unsigned int    dram_test_len = DRAM_TEST_SIZE >> 2; // 1 addr = 4 bytes
	volatile unsigned int *ram = (volatile unsigned int *)DRAM0_BASE_ADDR;
	if (debug)
		prn_string("\tDO DRAM SEQUENTIAL RW TEST START\n");

	// write clean
	DRAM_WRITE_CLEAN(dram_test_len, 0);

	// write value
	for (i = 0 ; i < dram_test_len ; i++) {
		ram[i] = i;
	}

	// read fata from DRAM
	for (i = 0 ; i < dram_test_len ; i++) {
		ret = DRAM_RW_CHECK(ram[i], i, debug);
		if (0 > ret) {
			if (debug)
				prn_string("\tDRAM SEQUENTIAL RW TEST FAIL !!!\n");
			ret = -1;
			break;
		}
	} // end for

	if (0 == ret) {
		if (debug)
			prn_string("\tDO DRAM SEQUENTIAL RW TEST PASS\n\n");
	}

	return ret ;

} // end - DRAM_SEQUENTIAL_RW_TEST

int DRAM_RANDOM_RW_TEST(unsigned int DRAM_TEST_SIZE, int debug)
{
	int             ret = 0 ;
	unsigned int    i = 0   ;
	unsigned int    dram_test_len = DRAM_TEST_SIZE >> 2; // 1 addr = 4 bytes
	volatile unsigned int *ram = (volatile unsigned int *)DRAM0_BASE_ADDR;
	const unsigned int DATA_PATTERN[7] = {0xAAAAAAAA, 0x55555555, 0xAAAA5555, 0x5555AAAA, 0xAA57AA57, 0xFFDDFFDD, 0x55D755D7};
	unsigned int DATA_PATTERN_INDEX = 0;
	const unsigned int DATA_PATTERNS = sizeof(DATA_PATTERN) >> 2;
	if (debug)
		prn_string("\tDO DRAM RANDOM RW TEST START\n");

	// write clean
	DRAM_WRITE_CLEAN(dram_test_len, 0);

	// write value
	for (i = 0 ; i < dram_test_len ; i++) {
		ram[i] = DATA_PATTERN[i % DATA_PATTERNS];
	}

	// read fata from DRAM
	for (i = 0 ; i < dram_test_len ; i++) {
		ret = DRAM_RW_CHECK(ram[i], DATA_PATTERN[i % DATA_PATTERNS], debug);
		if (0 > ret) {
			if (debug)
				prn_string("\tDRAM RANDOM RW TEST FAIL !!!\n");
			ret = -1;
			break;
		}
	} // end for

	if (0 == ret) {
		if (debug)
			prn_string("\tDO DRAM RANDOM RW TEST PASS\n\n");
	}

	return ret ;
} // end - DRAM_RANDOM_RW_TEST

int DRAM_INVERT_RW_TEST(unsigned int DRAM_TEST_SIZE, int debug)
{
	int             ret = 0 ;
	unsigned int    i = 0   ;
	unsigned int    dram_test_len = DRAM_TEST_SIZE >> 2; // 1 addr = 4 bytes
	volatile unsigned int *ram = (volatile unsigned int *)DRAM0_BASE_ADDR;
	if (debug)
		prn_string("\tDO DRAM_INVERT_RW_TEST START\n");


	// write clean
	DRAM_WRITE_CLEAN(dram_test_len, 0);

	// write then read test
	for (i = 0 ; i < dram_test_len ; i++) {
		// write 0 test
		ram[i] = 0x00000000;
		ret = DRAM_RW_CHECK(ram[i], 0x00000000, debug);
		if (0 > ret) {
			if (debug)
				prn_string("\tDRAM INVERT RW 0x00000000 TEST FAIL !!!\n");
			ret = -1;
			break;
		}

		// write 1 test
		ram[i] = 0xFFFFFFFF;
		ret = DRAM_RW_CHECK(ram[i], 0xFFFFFFFF, debug);
		if (0 > ret) {
			if (debug)
				prn_string("\tDRAM INVERT RW 0xFFFFFFFF TEST FAIL !!!\n");
			ret = -1;
			break;
		}
	}

	if (0 == ret) {
		if (debug)
			prn_string("\tDO DRAM INVERT RW TEST PASS\n\n");
	}

	return ret ;
} // end - DRAM_INVERT_RW_TEST

int do_stress_test(unsigned int test_len, int isDebug, int loop, int exit)
{
	int ret = 0;
	do {
		// 1) sequence test ddr R/W
		ret = DRAM_SEQUENTIAL_RW_TEST(test_len, isDebug);
		// while(0 > ret){;}
		if (0 > ret && exit)
			return ret;

		// 2) random test ddr R/W
		ret = DRAM_RANDOM_RW_TEST(test_len, isDebug);
		// while(0 > ret){;}
		if (0 > ret && exit)
			return ret;

		// 3) invert test ddr R/W
		ret = DRAM_INVERT_RW_TEST(test_len, isDebug);
		// while(0 > ret){;}
		if (0 > ret && exit)
			return ret;
	} while (loop);

	return 0;
}

/**
**FOR 8388 , DDR3-1600 ONLY
**/
// ***********************************************************************
// * FUNC      : trim_WDM
// * PARAM     : void
// * PURPOSE   : refine DPCU traing WDM result for DRAM0/DRAM1
// ***********************************************************************
// #define DBG_DM
void trim_WDM(unsigned int PHY_REG_BASE)
{
	unsigned int trim_count = 0;
	unsigned int ori_wdm, new_wdm = 0;
	unsigned int wdq_iprd = 0;
	unsigned int i = 0;
	unsigned int offset_psd = 0;
	unsigned int psd = 0;
	unsigned int right_psd = 0;
	const unsigned int MAX_PSD = 100;
	rgst_value = SP_REG(PHY_REG_BASE, 8) & ~(0x3F);

#ifdef DBG_DM
	prn_string("\t DX");
	prn_decimal(PHY_REG_BASE - 52);
	prn_string(":\n");
#endif

	// Get IPRD
	wdq_iprd = ((SP_REG(PHY_REG_BASE, 3) >> 24) & 0xFF);  // WDQ_IPRD
#ifdef DBG_DM
	prn_string("\t DX");
	prn_decimal(PHY_REG_BASE - 52);
	prn_string(" IPRD = ");
	prn_byte(wdq_iprd);
	prn_string("\n");
#endif

	// Cal each PSD pico second
	psd = IPRD_VALUE / wdq_iprd;

#ifdef DBG_DM
	prn_string("\t DX");
	prn_decimal(PHY_REG_BASE - 52);
	prn_string(" 1 PSD PICO = ");
	prn_decimal(psd);
	prn_string("\n");
#endif

	// FIND FAILED EDGE
	ori_wdm = SP_REG(PHY_REG_BASE, 8) & 0x3F;
	do {
		new_wdm = ori_wdm + (++trim_count);
		SP_REG(PHY_REG_BASE, 8) = rgst_value | new_wdm;
		// prn_string("\t WDM TRIM COUNT ="); prn_decimal(trim_count); prn_string("\n");
	} while (0 == do_stress_test(0x1000, 0, 0, 1));
	// }while( SDCTRL_TRIMMER_TEST_(0, 0, 1) );

	// CAL OFFSET PSD
	new_wdm = ori_wdm;
	right_psd = SAFE_MARGIN_VALUE - (psd * trim_count);
	if (right_psd < 0) {
		SP_REG(PHY_REG_BASE, 8) = rgst_value | ori_wdm;
	} else {
#ifdef DBG_DM
		prn_string("\t 265<-ORI = ");
		prn_decimal(right_psd);
		prn_string(" PICO\n");
#endif
		for (offset_psd = 0; offset_psd < MAX_PSD; offset_psd++) {
			if ((psd * offset_psd) > right_psd) {
				offset_psd--;
#ifdef DBG_DM
				prn_string("\t DX");
				prn_decimal(PHY_REG_BASE - 52);
				prn_string(" OFFSET_PSD = ");
				prn_byte(offset_psd);
				prn_string("\n");
#endif
				if (ori_wdm >= offset_psd)
					new_wdm = ori_wdm - offset_psd;
				else {
#ifdef DBG_DM
					prn_string("\t DX");
					prn_decimal(PHY_REG_BASE - 52);
					prn_string(" WARNING!!! ori_wdm < offset_psd, force set wdm = 0\n");
					prn_string(" OFFSET_PSD = ");
					prn_byte(ori_wdm);
					prn_string("\n");
#endif
					new_wdm = 0;
				}
				break;
			}
		}
#ifdef DBG_DM
		prn_string("\t DX");
		prn_decimal(PHY_REG_BASE - 52);
		prn_string(" ORI_BDD_WDM =");
		prn_byte(ori_wdm);
		prn_string("\n");
		prn_string("\t DX");
		prn_decimal(PHY_REG_BASE - 52);
		prn_string(" NEW_BDD_WDM =");
		prn_byte(new_wdm);
		prn_string("\n");
#endif
		SP_REG(PHY_REG_BASE, 8) = rgst_value | new_wdm;
	}
}

// ***********************************************************************
// * FUNC      : dram_refine_flow
// * PARAM     : int
// * PURPOSE   : refine DPCU traing result for DRAM0/DRAM1
// ***********************************************************************
int dram_refine_flow(unsigned int DRAM_ID)
{
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0  ;
	select_SDC_PHY_GRP(DRAM_ID, &SDC_BASE_GRP, &PHY_BASE_GRP);

	// refine WDM
	trim_WDM(PHY_BASE_GRP + 2);
	trim_WDM(PHY_BASE_GRP + 3);

	return PASS;
}
#endif

#if (defined(DRAMSCAN) || defined(SISCOPE))

#define FAE_DEBUG
// #define DEBUG_BDD
unsigned int get_unit_pico(unsigned int PHY_REG_BASE)
{
	unsigned int wdq_iprd = 0;
	unsigned int pico = 0;
	const unsigned int MAX_PSD = 100;

	// Get IPRD
	wdq_iprd = ((SP_REG(PHY_REG_BASE, 3) >> 24) & 0xFF);
#ifdef DEBUG_BDD
	prn_string("\tDX");
	prn_decimal(PHY_REG_BASE - 52);
	prn_string(" IPRD = ");
	prn_byte(wdq_iprd);
	prn_string("\n");
#endif

	// Cal each PSD pico second
	for (pico = 1; pico < MAX_PSD; pico++) {
		if ((pico * wdq_iprd) > IPRD_VALUE) {
			pico--;
			break;
		}
	}

	return pico;
}

int dram_test_bdd(unsigned int group, unsigned int regs, unsigned int shift_bit, int trim)
{
	unsigned int trim_count = 0;
	unsigned int ori_bdd_value = 0;
	unsigned int new_bdd_value = 0;
	int i;

	// BACKUP ORIGINAL REGS VALUE WITHOUT TARGET BDD
	rgst_value = SP_REG(group, regs) & (~(0x3F << shift_bit));

	// GET TARGET VALUE
	ori_bdd_value = (SP_REG(group, regs) >> shift_bit) & 0x3F;

#ifdef DEBUG_BDD
	prn_string("\n");
	prn_string("\t GROUP =");
	prn_dword(group);
	prn_string("\t REGS =");
	prn_dword(regs);
	prn_string(", shift_bit =");
	prn_dword(shift_bit);
	prn_string("\n");
	prn_string("\t ORI_BDD =");
	prn_dword(SP_REG(group, regs);
		  prn_string("\t ORI_BDD(NO_TARGET_BDD) ="); prn_dword(rgst_value);
		  prn_string("\t TARGET_BDD ="); prn_dword(ori_bdd_value);
#endif

		  // FIND FAILED EDGE
	do {
		trim_count++;
		new_bdd_value = ori_bdd_value + (trim_count * trim);
		if (new_bdd_value > 0x3F) {
#ifdef FAE_DEBUG
			prn_string("pass");
			prn_string(",");
			prn_string("pass");
			prn_string(",");
#endif
			break;
		}
		SP_REG(group, regs) = rgst_value | (new_bdd_value << shift_bit);
		// prn_string("\t TRIM COUNT ="); prn_decimal(trim_count); prn_string("\n");
	} while (0 == do_stress_test(0x1000, 0, 0, 1));
	trim_count--;

#ifdef DEBUG_BDD
	prn_string(" TOTAL TRIM ="); prn_decimal(trim_count); prn_string("\n");
#endif

	SP_REG(group, regs) = rgst_value | (ori_bdd_value << shift_bit);


				      return trim_count;
}


int dram_test_addr(int trim)
{
	unsigned int trim_count = 0;
	unsigned int ck_value = 0;
	unsigned int ac_value = 0;
	unsigned int ack_value = 0;
	unsigned int ori_ddl_value = 0;
	const unsigned int group = 50;
	const unsigned int regs = 17;
	int i;

	// BACKUP ORIGINAL REGS VALUE
	ori_ddl_value = SP_REG(group, regs);

	// GET TARGET VALUE
	ck_value = (SP_REG(group, regs) >>  0) & 0x3F;
	ac_value = (SP_REG(group, regs) >>  8) & 0x3F;
	ack_value = (SP_REG(group, regs) >> 16) & 0x3F;
#ifdef DEBUG_BDD
	prn_string("\nCK =");
	prn_dword(ck_value);
	prn_string("AC =");
	prn_dword(ac_value);
	prn_string("ACK =");
	prn_dword(ack_value);
#endif

	// FIND FAILED EDGE
	do {
		trim_count++;
		ac_value += trim;
		ack_value += trim;
		if (ac_value > 0x3F || ack_value > 0x3F) {
#ifdef FAE_DEBUG
			prn_string("pass");
			prn_string(",");
			prn_string("pass");
			prn_string(",");
#endif
			break;
		}
		SP_REG(group, regs) = (ck_value << 0) | (ac_value << 8) | (ack_value << 16);
#ifdef DEBUG_BDD
		prn_string(" DDL VALUE =");
		prn_dword(SP_REG(group, regs);
#endif
	} while (0 == do_stress_test(0x1000, 0, 0, 1));
	trim_count--;

#ifdef DEBUG_BDD
	prn_string(" TOTAL TRIM =");
	prn_decimal(trim_count);
	prn_string("\n");
#endif

	SP_REG(group, regs) = ori_ddl_value;

	return trim_count;
}

void dram_shift_bdd(unsigned int group, unsigned int reg, unsigned int shift, unsigned int psd)
{
	unsigned int tmp = 0;
#ifdef DEBUG_BDD
	prn_string("\tBEFORE:");
	prn_dword(SP_REG(group, reg);
#endif

		  tmp = (SP_REG(group, reg) >> shift) & 0x3F;
#ifdef DEBUG_BDD
		  prn_string("\tORI:"); prn_byte(tmp); prn_string("\n");
#endif
		  tmp += psd;
		  tmp &= 0x3F;
#ifdef DEBUG_BDD
		  prn_string("\tNEW:"); prn_byte(tmp); prn_string("\n");
#endif
		  SP_REG(group, reg) &= ~(0x3F << shift);
		  SP_REG(group, reg) |= (tmp << shift);

#ifdef DEBUG_BDD
		  prn_string("\tAFTER:"); prn_dword(SP_REG(group, reg); prn_string("\n");
#endif
}

	  void dump_ddr_eye_result(int window, char *msg)
{
	prn_string(msg);
	prn_decimal(window);
	prn_string(" pico second\n");
}

int dram_test_flow(unsigned int DRAM_ID)
{
	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0  ;
	select_SDC_PHY_GRP(DRAM_ID, &SDC_BASE_GRP, &PHY_BASE_GRP);

	unsigned int i = 0, j = 0, g_max = 0, r_max = 0, tmp = 0, trim_count = 0;
	unsigned int GROUPS[2] = {PHY_BASE_GRP + 2, PHY_BASE_GRP + 3};
	unsigned int DX_PICO_PSD[2] = {0};
	unsigned int DX_SHIFT_PSD[2] = {0};
	unsigned int SHIFT_ADDR_PSD[2] = {0};
	unsigned int pico = 0, bdd = 0;
	unsigned int BDD_LEFT_RESULT[2][19] = {0};
	unsigned int BDD_RIGHT_RESULT[2][19] = {0};
	const int FOUND_LEFT_EDGE = 1;
	const int FOUND_RIGHT_EDGE = -1;
	static const unsigned int REGS[][2] = {
		// reg, shift_bit, offset_psd
		{11,  0}, // RDQS_BDD,0
		{12,  0}, // RDQ0_BDD,1
		{12,  8}, // RDQ1_BDD,2
		{12, 16}, // RDQ2_BDD,3
		{12, 24}, // RDQ3_BDD,4
		{13,  0}, // RDQ4_BDD,5
		{13,  8}, // RDQ5_BDD,6
		{13, 16}, // RDQ6_BDD,7
		{13, 24}, // RDQ7_BDD,8
		{ 8,  0}, // WDM_BDD,9
		{ 8,  8}, // WDQS_BDD,10
		// { 8, 16},// WDQS_OE_BDD
		{ 9,  0}, // WDQ0_BDD
		{ 9,  8}, // WDQ1_BDD
		{ 9, 16}, // WDQ2_BDD
		{ 9, 24}, // WDQ3_BDD
		{10,  0}, // WDQ4_BDD
		{10,  8}, // WDQ5_BDD
		{10, 16}, // WDQ6_BDD
		{10, 24}, // WDQ7_BDD
	};

	const static char *bdd_name[19] = {
		"RDQS_BDD",
		"RDQ0_BDD",
		"RDQ1_BDD",
		"RDQ2_BDD",
		"RDQ3_BDD",
		"RDQ4_BDD",
		"RDQ5_BDD",
		"RDQ6_BDD",
		"RDQ7_BDD",
		"WDM_BDD ",
		"WDQS_BDD",
		"WDQ0_BDD",
		"WDQ1_BDD",
		"WDQ2_BDD",
		"WDQ3_BDD",
		"WDQ4_BDD",
		"WDQ5_BDD",
		"WDQ6_BDD",
		"WDQ7_BDD"
	};


	g_max = sizeof(GROUPS) >> 3;
	r_max = sizeof(REGS) >> 3;

	// CHECK APHY INIT RESULT
	i = (SP_REG(PHY_BASE_GRP, 2) >> 1) & 0x01;
	if (i == 1) {
		prn_string("<|> \tAPHY INIT ERROR...IC or DDR need to rework!\n");
		DPCU_DT_RESULT_DUMP(0);
		return FAIL;
	}

	// CHECK DT INIT RESULT
	// prn_string("\n");
	i = (SP_REG(PHY_BASE_GRP + 1, 0) >> 8) & 0x3F;
	if (i != 0) {
		prn_string("<|> \tDT   INIT ERROR...IC or DDR need to rework!\n");
		DPCU_DT_RESULT_DUMP(0);
		return FAIL;
	}


	// CAL 1 PSD = ? PICO
	for (i = 0; i <= g_max; i++) {
		pico = get_unit_pico(GROUPS[i]);
		DX_PICO_PSD[i] = pico;
#ifdef FAE_DEBUG
		prn_string("<|> \tDX");
		prn_decimal(i);
		prn_string(" 1 PSD = ");
		prn_byte(DX_PICO_PSD[i]);
		prn_string(" PICO\n");
#endif
	}


	// test +
#ifdef FAE_DEBUG
	prn_string("\n\nFIND DQS/DQ SETUP TIME\n");
#endif

	for (i = 0; i <= g_max; i++) {
		for (j = 0; j < r_max; j++) {
#ifdef FAE_DEBUG
			prn_string("<|> \t");
			prn_string(" DX");
			prn_decimal(i);
			prn_string(bdd_name[j]);
			prn_string(" SETUP TIME,");
#endif
			trim_count = dram_test_bdd(GROUPS[i], REGS[j][0], REGS[j][1], FOUND_LEFT_EDGE);
			BDD_LEFT_RESULT[i][j] = trim_count;
#ifdef FAE_DEBUG
			prn_decimal(trim_count * DX_PICO_PSD[i]);
			prn_string(" pico;\n");
#endif
		}
	}

	// shift +
	unsigned int psd = 0;
	const unsigned int MAX_PSD = 100;

#ifdef FAE_DEBUG
	// prn_string("\n\n SHIFT:"); prn_decimal(SAFE_MARGIN_VALUE); prn_string("\n");
#endif
	for (i = 0; i <= g_max; i++) {
		for (psd = 1; psd < MAX_PSD; psd++) {
			if ((psd * DX_PICO_PSD[i]) >= SAFE_MARGIN_VALUE) {
				psd--;
				DX_SHIFT_PSD[i] = psd;
				break;
			}
		}

#ifdef FAE_DEBUG
		prn_string("<|> \t DX");
		prn_decimal(i);
		prn_string(" SHIFT +");
		prn_decimal(psd * pico);
		prn_string(" PICO;\n");
#endif
		for (j = 0; j < r_max; j++) {
#ifdef DEBUG_BDD
			prn_string("\tBEFORE:");
			prn_dword(SP_REG(GROUPS[i], REGS[j][0]));
#endif

			tmp = (SP_REG(GROUPS[i], REGS[j][0]) >> REGS[j][1]) & 0x3F;
#ifdef DEBUG_BDD
			prn_string("\tORI:");
			prn_byte(tmp);
			prn_string("\n");
#endif
			tmp += DX_SHIFT_PSD[i];
#ifdef DEBUG_BDD
			prn_string("\tNEW:");
			prn_byte(tmp);
			prn_string("\n");
#endif
			SP_REG(GROUPS[i], REGS[j][0]) &= ~(0x3F << REGS[j][1]);
			SP_REG(GROUPS[i], REGS[j][0]) |= (tmp << REGS[j][1]);

#ifdef DEBUG_BDD
			prn_string("\tAFTER:");
			prn_dword(SP_REG(GROUPS[i], REGS[j][0]); prn_string("\n");
#endif
		}
	}


	// test -
#ifdef FAE_DEBUG
	prn_string("\n\nFIND DQS/DQ HOLD TIME:\n");
#endif
	for (i = 0; i <= g_max; i++) {
		for (j = 0; j < r_max; j++) {
#ifdef FAE_DEBUG
			prn_string("<|> \t");
			prn_string(" DX");
			prn_decimal(i);
			prn_string(bdd_name[j]);
			prn_string(" HOLD TIME,");
#endif
			trim_count = dram_test_bdd(GROUPS[i], REGS[j][0], REGS[j][1], FOUND_RIGHT_EDGE);
			BDD_RIGHT_RESULT[i][j] = trim_count;
#ifdef FAE_DEBUG
			prn_decimal(trim_count * DX_PICO_PSD[i]);
			prn_string(" pico;\n");
#endif
		}
	}


	// shift -
	for (i = 0; i <= g_max; i++) {
#ifdef FAE_DEBUG
		prn_string("<|> \t DX");
		prn_decimal(i);
		prn_string(" SHIFT -");
		prn_decimal(DX_SHIFT_PSD[i]*pico);
		prn_string(" PICO;\n");
#endif
		for (j = 0; j < r_max; j++) {
#ifdef DEBUG_BDD
			prn_string("\tBEFORE:");
			prn_dword(SP_REG(GROUPS[i], REGS[j][0]);
#endif

				  tmp = (SP_REG(GROUPS[i], REGS[j][0]) >> REGS[j][1]) & 0x3F;
#ifdef DEBUG_BDD
				  prn_string("\tORI:"); prn_byte(tmp); prn_string("\n");
#endif
				  tmp -= DX_SHIFT_PSD[i];
#ifdef DEBUG_BDD
				  prn_string("\tNEW:"); prn_byte(tmp); prn_string("\n");
#endif
				  SP_REG(GROUPS[i], REGS[j][0]) &= ~(0x3F << REGS[j][1]);
				  SP_REG(GROUPS[i], REGS[j][0]) |= (tmp << REGS[j][1]);

#ifdef DEBUG_BDD
				  prn_string("\tAFTER:"); prn_dword(SP_REG(GROUPS[i], REGS[j][0]); prn_string("\n");
#endif
		}
	}

	SHIFT_ADDR_PSD[0] = dram_test_addr(1);
#ifdef FAE_DEBUG
	prn_string("FIND AC/ACK SETUP TIME:\n");
	prn_string("<|> \t ");
	prn_decimal(SHIFT_ADDR_PSD[0]*pico);
	prn_string(" PICO;\n");
#endif

	// test report
#ifdef FAE_DEBUG
	prn_string("\n");
	prn_string("BDD WINDOW REPORT:\n");
#endif
	const unsigned int bdd_left_margin 		=	85;
	const unsigned int bdd_right_margin		=	95;
	const unsigned int addr_left_margin		=	245;
	const unsigned int addr_right_margin	=	170;
	int dump_debug = 0;
	int isAllPassed = 1;

	// check addr
	psd = SHIFT_ADDR_PSD[0] * pico;
	prn_string("\nCHECK ADDRESS WINDOW:\n"); // prn_decimal(psd); prn_string(" pico\n");
	dump_ddr_eye_result(psd, "\tADDRESS COMMAND: ");
	if (addr_left_margin > psd) {
		isAllPassed = 0;
	}
	// check R/W window
	const unsigned char DX_NAME[][5] = {"LOW", "HIGH"};
	for (i = 0; i <= g_max; i++) {
		prn_string("CHECK ");
		prn_string(DX_NAME[i]);
		prn_string(" BYTE:\n");
		// check DX0 read setup time
		psd = BDD_RIGHT_RESULT[i][0] * DX_PICO_PSD[i];
		// prn_string("RS_WIN="); prn_decimal(psd); prn_string("\n");
		dump_ddr_eye_result(psd, "\tREAD SETUP TIME: ");
		if (bdd_left_margin > psd) {
			isAllPassed = 0;
		}

		// check DX1 read hole time
		psd = BDD_LEFT_RESULT[i][0] * DX_PICO_PSD[i];
		// prn_string("RH_WIN="); prn_decimal(psd); prn_string("\n");
		dump_ddr_eye_result(psd, "\tREAD HOLD TIME: ");
		if (bdd_left_margin > psd) {
			isAllPassed = 0;
		}

		// check DX0 write setup time
		psd = BDD_RIGHT_RESULT[i][10] * DX_PICO_PSD[i];
		// prn_string("WS_WIN="); prn_decimal(psd); prn_string("\n");
		dump_ddr_eye_result(psd, "\tWRITE SETUP TIME: ");
		if (bdd_left_margin > psd) {
			isAllPassed = 0;
		}

		// check DX1 write hold time
		psd = BDD_LEFT_RESULT[i][10] * DX_PICO_PSD[i];
		dump_ddr_eye_result(psd, "\tWRITE HOLD TIME: ");
		if (bdd_left_margin > psd) {
			isAllPassed = 0;
		}
	}

	if (1 == isAllPassed)
		prn_string("[STATUS] GREAT!! DRAM WINDOW ALL PASSED!!\n");
	else
		prn_string("[STATUS] WARNING!! DRAM MARGIN NOT ENOUGH!!\n");

	return isAllPassed;
}


// #define DBG_SHOW_DRAMINIT_MSG
// ***********************************************************************
// * FUNC      : DRAM_SCAN
// * PARAM     : DRAM_ID
// * PURPOSE   : do the DRAM_SCAN to findout the best parameters of
//               SDCTRL and DDR_PHY
// * RGST field: SDCTRL System Timing
//     [30:25] : Read Data Path delay 1 cycles
//     [24:20] : DQS IN delay cycle number =
//     [19:16] : SUB_INTERNAK_RL = 0
//     [11: 8] : WL_CNT
// ***********************************************************************
void DRAM_SCAN(unsigned int DRAM_ID)
{
	unsigned int        idx                 ;
	int	    ret                 ;
	unsigned int        rec_idx             ;
	unsigned int        rgst_value          ;

	unsigned int        SDC_BASE_GRP = 0,
			    PHY_BASE_GRP = 0      ;

	unsigned int        sdc_str_dqs_in      ;   // sdctrl - internal STR_DQS_IN
	unsigned int        sdc_ext_cl_cnt      ;   // sdctrl - internal EXTRA_CL_CNT
	unsigned int        sdc_int_wl_cnt      ;   // sdctrl - internal WL_CNT

	unsigned int        cpu_test_result     ;   // cpu simple write/read result
	unsigned int        trim_test_result    ;   // trimmer random write/read result

	unsigned int        scan_pass_param[30] ;   // record max 20 parameters
	unsigned int        scan_pass_acack[30] ;   // record max 20 parameters
	// -------------------------------------------------------
	// 0. SDCTRL / DDR_PHY RGST GRP selection
	// -------------------------------------------------------
	select_SDC_PHY_GRP(DRAM_ID, &SDC_BASE_GRP, &PHY_BASE_GRP);

	for (rec_idx = 0 ; rec_idx < 30 ; rec_idx++) {
		// reset record array
		scan_pass_param[rec_idx] = 0 ;
		scan_pass_acack[rec_idx] = 0 ;
	}

	// Ellison test
	// RID_PASS();
	int mpb = 0;
	rec_idx = 0 ;
	unsigned int start_sdc_str_dqs_in = 0;
#ifdef MPEG_DRAM_DDR_1333
	start_sdc_str_dqs_in = 14;
#elif defined MPEG_DRAM_DDR_1600
	start_sdc_str_dqs_in = 15;
#elif defined MPEG_DRAM_DDR_1866
	start_sdc_str_dqs_in = 17;
#else
#error "Now DRAM SCAN only support 1333/1600/1866, you should add other data rate parameters"
#endif

	// -------------------------------------------------------
	// 1. for loop to findout str_dqs_in & ext_cl_cnt
	// -------------------------------------------------------
	// for( sdc_str_dqs_in=14 ; sdc_str_dqs_in<=18 ; sdc_str_dqs_in++ ){
	for (sdc_str_dqs_in = start_sdc_str_dqs_in ; sdc_str_dqs_in <= start_sdc_str_dqs_in ; sdc_str_dqs_in++) {
		// for( sdc_str_dqs_in=17 ; sdc_str_dqs_in<=17 ; sdc_str_dqs_in++ ){

		for (sdc_ext_cl_cnt = 20 ; sdc_ext_cl_cnt <= 30 ; sdc_ext_cl_cnt++) {
			// for( sdc_ext_cl_cnt=24 ; sdc_ext_cl_cnt<=25 ; sdc_ext_cl_cnt++ ){

			for (sdc_int_wl_cnt = 8 ; sdc_int_wl_cnt <= 10 ; sdc_int_wl_cnt++) {
				// for( sdc_int_wl_cnt=10 ; sdc_int_wl_cnt<=10 ; sdc_int_wl_cnt++ ){

#ifndef DBG_SHOW_DRAMINIT_MSG
				// mp = 1; // hide msg
#endif
				gEXTRA_CL_CNT = sdc_ext_cl_cnt;
				gSTR_DQS_IN = sdc_str_dqs_in;
				gWL_CNT = sdc_int_wl_cnt;
				ckobd_training_flag = 1;
				ret = silent_dram_init();
				mpb = mp;
				mp = 0;
				prn_string("\nSCAN=>");
				prn_string("  STR_DQS_IN=");
				prn_decimal(sdc_str_dqs_in);
				prn_string("  EXT_CL_CNT=");
				prn_decimal(sdc_ext_cl_cnt);
				prn_string("  INT_WL_CNT=");
				prn_decimal(sdc_int_wl_cnt);
				prn_string("\n");
				mp = mpb;
				if (ret == SUCCESS) {
#ifdef DBG_SHOW_DRAMINIT_MSG
					prn_string("\n/======================DRAM PARAMS START======================\\\n");
					prn_string("\nSCAN=>");
					prn_string("  STR_DQS_IN=");
					prn_decimal(sdc_str_dqs_in);
					prn_string("  EXT_CL_CNT=");
					prn_decimal(sdc_ext_cl_cnt);
					prn_string("  INT_WL_CNT=");
					prn_decimal(sdc_int_wl_cnt);
					prn_string("\n");
					prn_string("\n\\======================DRAM PARAMS END======================/\n");
#endif
				} else {
					continue;
				}

				// clean dram content of test region and test result
				cpu_test_result     = 0 ;
				trim_test_result    = 0 ;
				DRAM_WRITE_CLEAN(0x1000, DRAM_ID) ;

				// test-1 : simple CPU W/R test
				cpu_test_result = do_stress_test(0x1000, 0, 0, 1) ;

				if (0 == cpu_test_result) {
					// test-2 : random trimmer test (after cpu W/R test)
					for (idx = 0 ; idx <= SCAN_TRIM_LEN ; idx++) {
						trim_test_result = SDCTRL_TRIMMER_TEST_(DRAM_ID, DRAM0_BASE_ADDR, 0x0100) ;
						// check trimmer test result
						if (trim_test_result) {
							if (idx == SCAN_TRIM_LEN) {
								mpb = mp;
								mp = 0;
								prn_string("\tSCAN=> Test Pass\n");
								scan_pass_param[rec_idx] = SP_REG(SDC_BASE_GRP, 11);
								scan_pass_acack[rec_idx] = SP_REG(PHY_BASE_GRP + 0, 17);
								dram_test_flow(0);
								rec_idx += 1 ;
								mp = mpb;
							}
						} else {
							prn_string("\tSCAN=> Test Fail\n");
							break;
						} // end if - trim_test_result
					} // end for - trimmer test loop

				} else {
					prn_string("\tSCAN=> Test Fail\n");
				} // end if - cpu_test_restult
			} // end for - sdc_int_wl_cnt
		} // end for - sdc_ext_cl_cnt
	} // end for - str_dqs_in

	// -------------------------------------------------------
	// 2. dump final pass parameters
	// -------------------------------------------------------
	mpb = mp;
	mp = 0;
	prn_string("\n\n==================================================================================\n");
	prn_string("DUMP DRAM-");
	prn_decimal(DRAM_ID);
	prn_string("parameters:\n");
	for (idx = 0 ; idx < rec_idx ; idx++) {
		if (scan_pass_param[idx] != 0) {

			// print out parameters
			prn_string("SCAN=> [");
			prn_decimal(idx);
			prn_string("]");
			prn_string(" ; STR_DQS_IN = ");
			prn_decimal((scan_pass_param[idx] >> 20) & 0x1F);
			prn_string(" ; EXT_CL_CNT = ");
			prn_decimal((scan_pass_param[idx] >> 25) & 0x3F);
			prn_string(" ; INT_WL_CNT = ");
			prn_decimal((scan_pass_param[idx] >> 8) & 0x0F);
			prn_string("; AC=");
			prn_decimal((scan_pass_acack[idx] >> 8) & 0x3F);
			prn_string("; ACK=");
			prn_decimal((scan_pass_acack[idx] >> 16) & 0x3F);
			prn_string("; CK=");
			prn_decimal((scan_pass_acack[idx] >> 0) & 0x3F);
			prn_string(";\n");
		}
	} // end for - idx
	prn_string("==================================================================================\n");
	mp = mpb;
} // end of DRAM_SCAN

void run_SiScope(void)
{
	prn_string("\n\n==================================run_SiScope START================================================\n");
	gAC = DPCU_AC0BD;
	gACK = DPCU_ACK0BD;
	gCK = DPCU_CK0BD;
	// for(gAC=0; gAC<=0; gAC++)
	// for(gACK=0; gACK<=0; gACK++)
	// for(gCK=0; gCK<=15; gCK++)
	{
#if 0
		mp = 0;
		prn_string("\n\n##################################################################################\n");
		prn_string("AC=");
		prn_decimal(gAC);
		prn_string("; ACK=");
		prn_decimal(gACK);
		prn_string("; CK=");
		prn_decimal(gCK);
		prn_string(";\n");
#endif
		DRAM_SCAN(0);
	}
	prn_string("\n\n==================================run_SiScope END================================================\n");
	gEXTRA_CL_CNT = SD0_EXTRA_CL_CNT;
	gSTR_DQS_IN = SD0_STR_DQS_IN;
	gWL_CNT = WL_CNT;
	silent_dram_init();
}

int silent_dram_init()
{
	mpb = mp;
	mp = 1;
	int ret = dram_init(0) ;
#ifndef DRAMSCAN
#ifdef SW_REFINE_DT_RESULT
	dram_refine_flow(0);
#endif
#endif
	mp = mpb;
	return ret;
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

int get_ddr_data_rate()
{
	return ((SP_REG(50, 12) & 0x7F) * 27);
}

void dump_now_ddr_clk_info()
{
	const int dataRate = get_ddr_data_rate();
	prn_string("<|> Now DDR CLOCK is ");
	prn_decimal(dataRate >> 1);
	prn_string(" MHz(");
	prn_decimal(dataRate);
	prn_string("MT/s)\n");
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

void check_stress_test_with_increase_freq()
{
	char input = '\0';
	int i = 0, rate = 0;
	int ret = 0;
	int loop = 0;
	int exit = 0;

	do {
		prn_string("\n*Would you like to test incrementing or decrementing ddr freq?(y/n)\n");
		prn_string("\t[y]: do test with incrementing freq\n");
		prn_string("\t[n]: do test without incrementing freq\n");
		input = sp_getChar();
		if ('y' == input || 'Y' == input) {
			prn_string("*please input percent of incrementing or decrementing ddr freq:\n");
			unsigned char percent[5] = {0};
			int len = sp_getString(percent);
			rate = get_int(len, 0, percent);
			prn_string("you input ");
			prn_decimal(rate);
			prn_string(" %\n");
			const int dataRate = get_ddr_data_rate();
			int increaseFreq = dataRate * rate / 100;
			int offsetMPLL = (increaseFreq / 27) + 1;
			int minMPLL = n_MPLL_DIV - offsetMPLL;
			int maxMPLL = n_MPLL_DIV + offsetMPLL;
			for (i = minMPLL; i <= maxMPLL; i++) {
				gMPLL_DIV = i;
				silent_dram_init();
				dump_now_ddr_clk_info();
				ret = do_stress_test(TEST_LEN, 1, 0, 1);
				while (0 > ret) {;}
			}

#if 0   /* keep testing max. frequency forever */
			gMPLL_DIV = maxMPLL;
			silent_dram_init();
			dump_now_ddr_clk_info();
			while (1) {
				ret = do_stress_test(TEST_LEN, 1, 0, 1);
				while (0 > ret) {;}
			}
#endif
#if 0   /* keep testing min. frequency forever */
			gMPLL_DIV = minMPLL;
			silent_dram_init();
			dump_now_ddr_clk_info();
			while (1) {
				ret = do_stress_test(TEST_LEN, 1, 0, 1);
				while (0 > ret) {;}
			}
#endif
#if 1   /* keep testing max. and min. frequencies forever */
			while (1) {
				gMPLL_DIV = minMPLL;
				silent_dram_init();
				dump_now_ddr_clk_info();
				ret = do_stress_test(TEST_LEN, 1, 0, 1);
				while (0 > ret) {;}
				gMPLL_DIV = maxMPLL;
				silent_dram_init();
				dump_now_ddr_clk_info();
				ret = do_stress_test(TEST_LEN, 1, 0, 1);
				while (0 > ret) {;}
			}
#endif

			// restore
			gMPLL_DIV = n_MPLL_DIV;
			silent_dram_init();
		} else if ('n' == input || 'N' == input) {
			prn_string("\n*Loop Test?(y/n)\n");
			input = sp_getChar();
			if ('y' == input || 'Y' == input)
				loop = 1;

			prn_string("\n*Exit, when test failed?(y/n)\n");
			input = sp_getChar();
			if ('y' == input || 'Y' == input)
				exit = 1;

			dump_now_ddr_clk_info();
			ret = do_stress_test(TEST_LEN, 1, loop, exit);
		}
	} while (0);
}

void check_stress_test()
{
	char input = '\0';
	do {

		prn_string("\n\n*Would you like to run stress test?(y/n)\n");
		input = sp_getChar();
		if ('y' == input || 'Y' == input) {
			check_stress_test_with_increase_freq();
		} else if ('n' == input || 'N' == input) {
			break;
		}
	} while (1);
}

void check_ddr_eye_window()
{
	char input = '\0';
	do {
		prn_string("\n\n*Would you like to run DDR EYE WINDOW ANALYSIS?(y/n)\n");
		input = sp_getChar();
		if ('y' == input || 'Y' == input) {
			prn_string("\n\n===============DDR EYE ANALYSIS START=================\n");
			dram_test_flow(0);
			prn_string("\n\n===============DDR EYE ANALYSIS DO====================\n");
		} else if ('n' == input || 'N' == input) {
			break;
		}
	} while (0);
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

// ***********************************************************************
// * FUNC      : dram_init_main
// * PARAM     : void
// * PURPOSE   : to call dram_init function for DRAM0/DRAM1
// ***********************************************************************
int dram_init_main()
{
	int ret, i;	 // for DRAM-1
	unsigned int temp_value = 0;

	// init params
	ckobd_training_flag = 1;
	gAC = DPCU_AC0BD;
	gACK = DPCU_ACK0BD;
	gCK = DPCU_CK0BD;
	gEXTRA_CL_CNT = SD0_EXTRA_CL_CNT;
	gSTR_DQS_IN = SD0_STR_DQS_IN;
	gWL_CNT = WL_CNT;
	gMPLL_DIV = n_MPLL_DIV;

	// do init
#if !(defined(DRAMSCAN) || defined(SISCOPE))
#ifdef DRAM_INIT_DEBUG
	mp = 0;
#endif
	dram_init(0);
#else
	prn_string("Built at " __DATE__ " " __TIME__);

#ifdef PLATFORM_GEMINI
	SP_REG(8, 0) |= 0x0001;			// Keep IOP in reset
	SP_REG(0, 17) |= (1 << 3) | (1 << 13);	// Keep DSP and ARM926 in reset
#endif

	silent_dram_init();
	// dump DDR INDO
	prn_string(DRAMINIT_BIN_REL_DATE);
	dump_now_ddr_clk_info();
	prn_string("<|> Now DDR SIZE is 2Gb\n");

	// start test
	do {
		check_run_siscope();
		check_stress_test();
		check_ddr_eye_window();
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


#ifdef SUPPORT_DRAM1
	with_dram_b_package_flag = 1;
	// DRAM-1 initial process
	for (i = 0; i < 10; i++) {
		ret = dram_init(1) ;
		if (ret == PACKAGE_216) {
			// case 1, we think this is 216 pin package, and don't need to dump initial error message
			// only check PZQ, CTCAL, DDL error flag
			with_dram_b_package_flag = 0 ;
			return PACKAGE_216_PIN;
		} else if (ret == FAIL) {
			prn_string("do something...  for GRP(49, 0) or GRP(50, 10)\n");
			prn_string("for loop @ i =  ");
			prn_dword(i);
			prn_string("\n ");
			SP_REG(0, 16) = SP_REG(0, 16) | (1 << 15) | (1 << 17);
			wait_loop(10000);
			prn_string("GRP((16, 0) =  ");
			prn_dword(SP_REG(16, 0));
			prn_string("\n ");

			SP_REG(0, 16) = SP_REG(0, 16) & (~(1 << 15) | (1 << 17));
			wait_loop(10000);
			prn_string("GRP((16, 0) =  ");
			prn_dword(SP_REG(16, 0); prn_string("\n ");
		} else if (ret == SUCCESS) {
			DPCU_DT_RESULT_DUMP(1);
			break;
		} else {
			prn_string("dram_init(1) no this case ..... what happen??\n");
		}
	}
#endif

#ifdef SDRAM_MPLL_ENABLE

#ifdef DDR_PLL_00
	unsigned int DPCU_RI_MPLL_SSCEN = 1;
	unsigned int DPCU_RI_MPLL_SP = 0;
	unsigned int DPCU_RI_MPLL_DIV_S  = 14;
#elif defined DDR_PLL_01
	unsigned int DPCU_RI_MPLL_SSCEN = 1;
	unsigned int DPCU_RI_MPLL_SP = 1;
	unsigned int DPCU_RI_MPLL_DIV_S  = 8;
#elif defined DDR_PLL_10
	unsigned int DPCU_RI_MPLL_SSCEN = 1;
	unsigned int DPCU_RI_MPLL_SP = 2;
	unsigned int DPCU_RI_MPLL_DIV_S  = 4;
#elif defined DDR_PLL_11
	unsigned int DPCU_RI_MPLL_SSCEN = 1;
	unsigned int DPCU_RI_MPLL_SP = 3;
	unsigned int DPCU_RI_MPLL_DIV_S  = 4;
#else
	unsigned int DPCU_RI_MPLL_SSCEN = 0;
	unsigned int DPCU_RI_MPLL_SP = 0;
	unsigned int DPCU_RI_MPLL_DIV_S  = 0;
#endif

	SP_REG(DRAM_0_PHY_REG_BASE, 12) = SP_REG(DRAM_0_PHY_REG_BASE, 12) & ~(0x60000F00);
	SP_REG(DRAM_0_PHY_REG_BASE, 12) = SP_REG(DRAM_0_PHY_REG_BASE, 12) | (DPCU_RI_MPLL_DIV_S << 8) | (DPCU_RI_MPLL_SP << 29);
	SP_REG(DRAM_0_PHY_REG_BASE, 11) = SP_REG(DRAM_0_PHY_REG_BASE, 11) | (DPCU_RI_MPLL_SSCEN << 2);
	prn_string("DDR PLL SETTING:");
	prn_dword(DPCU_RI_MPLL_SP);
	prn_string("\n ");
#endif

	// pll_init_setting();
	return 1;
}
