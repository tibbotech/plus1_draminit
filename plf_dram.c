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

//#define SUPPORT_DRAM1   // dram 1
//#define SUPPORT_STR     // suspend to ram
//#define PLATFORM_3502   // 3502-compatible version (defined in include/config.h)

//! ***********************************************************************
//! Setting DRAM_INIT_DEBUG to 1 causes the system to print
//! debug messages or not. If set to 0 all DRAM_INIT messages will be suppressed instead. 
//! Dumping all debug messages to UART will cause booting speed to slowdown
//! ***********************************************************************
#define DRAMINIT_BIN_REL_DATE "draminit.bin release date 2016/12/01:2300\n"

//#define DRAM_INIT_DEBUG 0 // defined in Makefile: please use "make debug"

#ifdef          CSIM_ASIC
    #define ASIC_CSIM
#elif defined   CSIM_FPGA
    #define SDRAM_FPGA
#else
    #error Please define => simulation type
#endif

#define GRP(group, addr)    *((volatile unsigned int *)(RGST_BASE + (group)*128 + ((addr)*4)))

//! DRAM 0/1 size is 2Byte width
#define DRAM0_BASE_ADDR     0x00000000
#define DRAM1_BASE_ADDR     0x10000000

#define DRAM_0_SDC_REG_BASE     33
#define DRAM_0_PHY_REG_BASE     50
#define DRAM_1_SDC_REG_BASE     46
#define DRAM_1_PHY_REG_BASE     49

//! Resume test 
//#define RESUME_TEST_SIZE	0x3e00000
#define RESUME_TEST_SIZE	0x1100
//#define SZ_128M		0x08000000

//! Note: DRAM size access range is defined at Makefile.in
//! for 3502 FPGA setting - DRAM1/0 is 256MB/256MB (2Gb/2Gb)
//! for 3502 ASIC setting - please reference production spec.
//#define TEST_LEN                500
//#define TEST_DRAM_0_BASE        0x00000000
//#define TEST_DRAM_0_SREF        0x08000000
//#define TEST_DRAM_1_BASE        0x20000000
//#define TEST_DRAM_1_SREF        0x25000000

//! call external function
extern void RID_PASS(void);
extern void RID_FAIL(void);
int dram_booting_flow( unsigned int DRAM_ID );
int dram_training_flow( unsigned int DRAM_ID );
int dram_refine_flow( unsigned int DRAM_ID );

unsigned int        rgst_value     =0;
unsigned int        wait_flag      =0;	//min
unsigned int        error_flag      =0;	//min
unsigned int        resume_function     =0;
unsigned int        aphy_select_value     =0;
unsigned int        with_dram_b_package_flag=0; //! 1: 3502B, 8700, 6700
unsigned int        aphy_select1_value =0;
unsigned int        aphy_select2_value =0;
unsigned int        ckobd_training_flag      =0;
unsigned int        ckobd_re_training_number =0;
unsigned int        PACKAGE_216_CASE = 0;

unsigned int        data_byte_0_RDQSG_left_total_tap     =0;
unsigned int        data_byte_0_RDQSG_right_total_tap    =0;
unsigned int        data_byte_1_RDQSG_left_total_tap     =0;
unsigned int        data_byte_1_RDQSG_right_total_tap    =0;

u32 mp;

//! ***********************************************************************
//! * FUNC     : read_mp_bit
//! * PARAM    : N/A
//! * PURPOSE  : This function reads mp_bit, mp_bit will return with a value 
//! *            indicating if the OTP bit is set or not.
//! *            This mp_bit was used to indicate whether or not DRAM_INIT 
//! *            messages will be sent to UART.
//! *            Because enabling it will cause bootup speed to slowdown, it
//! *            is now disabled by default. (setting DRAM_INIT_DEBUG to 0)
//! ***********************************************************************
u32 read_mp_bit(void)
{
#if DRAM_INIT_DEBUG
	//return (HWREG_R(hb_otp_data0) & (1 << 15)); /* Actually read mp_bit to see if debug messages
	//                                               should be sent to UART */
	return 0;
#else
	return 1; /* Setting this value to 1 suppresses DRAM_INIT debug messages */
#endif
}

//! ***********************************************************************
//! * FUNC      : wait_loop
//! * PARAM     : wait_counter
//! * PURPOSE   : to wait the number of counter times
//! ***********************************************************************
void wait_loop( unsigned int wait_counter )
{
    unsigned int    i   =   0   ;

    for( i=0 ; i<wait_counter ; i++ ){;}

}//! end - wait_loop

void REG_DUMP(unsigned int BASE_GRP)
{
	int addr = 0;
	unsigned int value = 0;
	prn_string("\n");
	
	for(addr = 0; addr < 32; addr++)
	{
		value = GRP(BASE_GRP, addr);
		prn_string(">>>G");
		prn_decimal(BASE_GRP);
		prn_string(".");
		prn_decimal(addr);
		prn_string("= 0x");
		prn_dword(value);
		prn_string("\n");
	}
	
}

int DRAM_ACC_TEST( unsigned int DRAM_TEST_SIZE , unsigned int DRAM_ADDR )
{
	unsigned int    i = 0   ;
	unsigned int    j = 0   ;
	unsigned int dram_value = 0   ;
	unsigned int count = 0;
	unsigned int data = 0;
	volatile unsigned int *ram = (volatile unsigned int *)DRAM_ADDR;

	//#define TRIM_ACC_TEST_EN
	//#define DBG_EN_ACC_TEST

	#if defined(DBG_EN_ACC_TEST) || defined(TRIM_ACC_TEST_EN)
	prn_string("\n\n@DRAM ADDR ");prn_decimal(DRAM_ADDR);prn_string(" CPU access start !!!\n");
	#endif

	//! test DRAM
	//! write data to DRAM
	#ifdef DBG_EN_ACC_TEST
	  prn_string("\nReady to WR ...\n");
	#endif
	for( i=0 ; i<DRAM_TEST_SIZE ; i++ ){
		ram[i] = ~(data+i);
		#ifdef DBG_EN_ACC_TEST
			 prn_dword(ram[i]);
		#endif
	}

	//! read fata from DRAM
	#ifdef DBG_EN_ACC_TEST
		prn_string("\nReady to READ ...\n");
	#endif
	data = 0;
	for( i=0 ; i<DRAM_TEST_SIZE ; i++ ){
		dram_value = ram[i] ;
		#ifdef DBG_EN_ACC_TEST
			prn_dword(dram_value) ;
		#endif
		if( dram_value != ~(data+i) ){
			#ifdef DBG_EN_ACC_TEST
				prn_string("DRAM-0 CPU access FAIL !!!\n");
			#endif
			#ifdef TRIM_ACC_TEST_EN
				prn_string("\nDRAM-0 CPU access FAIL !!!\n");
				prn_string("ram[");
				prn_decimal(i);
				prn_string("]=");
				prn_byte(dram_value);
				prn_string(", correct will be ");
				prn_byte(~(data+i));
				prn_string("\n");
			#endif
			return 0 ;
		}
	}//! end for		

	//clear DRAM data
	for( i=0 ; i<DRAM_TEST_SIZE ; i++ ){
		ram[i] = 0;
		#ifdef DBG_EN_ACC_TEST
			 prn_dword(ram[i]);
		#endif
	}
	
	#if defined(DBG_EN_ACC_TEST) || defined(TRIM_ACC_TEST_EN)
		prn_string("@DRAM ADDR ");prn_decimal(DRAM_ADDR);prn_string(" CPU access PASS !!!\n");
	#endif

	return 1 ;

}//! end - DRAM_ACC_TEST

void SET_RDQS_IPRD_TAP_NO( unsigned int DRAM_ID )
{
    unsigned int SDC_BASE_GRP       = 0 ;
    unsigned int PHY_BASE_GRP       = 0 ;
    unsigned int temp_a             = 0 ;
    unsigned int temp_b             = 0 ;
    unsigned int temp_c             = 0 ;
    unsigned int only_dump_PSD      = 0 ;
    unsigned int RDQS_IPRD_TAP_NO         = 0 ;
    
    //! -------------------------------------------------------
    //! 0. DDR_PHY RGST GRP selection
    //! -------------------------------------------------------
    switch ( DRAM_ID ){
        case 0: {
        	  SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE      ;
              PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE      ;
            break;
        }
        case 1: {
        	  SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE      ;
              PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE      ;
            break;
        }
    }//! end switch

    temp_a = (GRP((PHY_BASE_GRP+2), 3)  >> 16) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 3)  >> 16) & 0xff;

    if ( temp_a > temp_b) {
        RDQS_IPRD_TAP_NO = temp_b;
    } else {
        RDQS_IPRD_TAP_NO = temp_a;
    }    

	//L_SIDE_RSL
	temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>   0) & 0x1f;
	temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>   0) & 0x1f;
	//prn_string("\tDX0 : L_SIDE_RSL =");prn_byte(temp_a);prn_string("\tDX1 : L_SIDE_RSL =");prn_byte(temp_b);prn_string("\n") ;
	data_byte_0_RDQSG_left_total_tap = temp_a * RDQS_IPRD_TAP_NO;
	data_byte_1_RDQSG_left_total_tap = temp_b * RDQS_IPRD_TAP_NO;


	//L_SIDE_PHA
	temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>   5) & 0x3;
	temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>   5) & 0x3;
	//prn_string("\tDX0 : L_SIDE_PHA =");prn_byte(temp_a);prn_string("\tDX1 : L_SIDE_PHA =");prn_byte(temp_b);prn_string("\n") ;
	data_byte_0_RDQSG_left_total_tap = (temp_a * RDQS_IPRD_TAP_NO/2) + data_byte_0_RDQSG_left_total_tap;
	data_byte_1_RDQSG_left_total_tap = (temp_b * RDQS_IPRD_TAP_NO/2) + data_byte_1_RDQSG_left_total_tap;


	//L_SIDE_PSD
	temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>   8) & 0xff;
	temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>   8) & 0xff;
	//prn_string("\tDX0 : L_SIDE_PSD =");prn_byte(temp_a);prn_string("\tDX1 : L_SIDE_PSD =");prn_byte(temp_b);prn_string("\n") ;
	data_byte_0_RDQSG_left_total_tap = temp_a + data_byte_0_RDQSG_left_total_tap;
	data_byte_1_RDQSG_left_total_tap = temp_b + data_byte_1_RDQSG_left_total_tap;


	//R_SIDE_RSL
	temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>  16) & 0x1f;
	temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>  16) & 0x1f;
	//prn_string("\tDX0 : R_SIDE_RSL =");prn_byte(temp_a);prn_string("\tDX1 : R_SIDE_RSL =");prn_byte(temp_b);prn_string("\n") ;
	data_byte_0_RDQSG_right_total_tap = temp_a * RDQS_IPRD_TAP_NO;
	data_byte_1_RDQSG_right_total_tap = temp_b * RDQS_IPRD_TAP_NO;


	//R_SIDE_PHA
	temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>  21) & 0x3;
	temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>  21) & 0x3;
	//prn_string("\tDX0 : R_SIDE_PHA =");prn_byte(temp_a);prn_string("\tDX1 : R_SIDE_PHA =");prn_byte(temp_b);prn_string("\n") ;
	data_byte_0_RDQSG_right_total_tap = (temp_a * RDQS_IPRD_TAP_NO/2) + data_byte_0_RDQSG_right_total_tap;
	data_byte_1_RDQSG_right_total_tap = (temp_b * RDQS_IPRD_TAP_NO/2) + data_byte_1_RDQSG_right_total_tap;


	//R_SIDE_PSD
	temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>  24) & 0xff;
	temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>  24) & 0xff;
	//prn_string("\tDX0 : R_SIDE_PSD =");prn_byte(temp_a);prn_string("\tDX1 : R_SIDE_PSD =");prn_byte(temp_b);prn_string("\n\n") ;
	data_byte_0_RDQSG_right_total_tap = temp_a + data_byte_0_RDQSG_right_total_tap;
	data_byte_1_RDQSG_right_total_tap = temp_b + data_byte_1_RDQSG_right_total_tap;
}

//! ***********************************************************************
//! * FUNC      : DPCU_DT_RESULT_DUMP
//! * PARAM     : DRAM_ID
//! * PURPOSE   : Dump DPCU Training information
//! ***********************************************************************
void DPCU_DT_RESULT_DUMP( unsigned int DRAM_ID )
{
    unsigned int SDC_BASE_GRP       = 0 ;
    unsigned int PHY_BASE_GRP       = 0 ;
    unsigned int temp_a             = 0 ;
    unsigned int temp_b             = 0 ;
    unsigned int temp_c             = 0 ;
    unsigned int only_dump_PSD      = 0 ;
    unsigned int RDQS_IPRD_TAP_NO         = 0 ;

    if( DRAM_ID==0 ){
        prn_string("DPCU_DT_INFO : ----- DUMP DRAM-0 delay line status -----\n\n") ;
    }else{
        prn_string("DPCU_DT_INFO : ----- DUMP DRAM-1 delay line status -----\n\n") ;
    }

    //! -------------------------------------------------------
    //! 0. DDR_PHY RGST GRP selection
    //! -------------------------------------------------------
    switch ( DRAM_ID ){
        case 0: {
        	  SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE      ;
              PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE      ;
            break;
        }
        case 1: {
        	  SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE      ;
              PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE      ;
            break;
        }
    }//! end switch

    //! DUMP SSCPLL Speed
    prn_string("DPCU_DT_INFO : \t********** DRAM SPEED **********\n") ;
    temp_a = (GRP(PHY_BASE_GRP, 12) >> 0) & 0x3f;
    prn_string("  SSCPLL Setting =");prn_byte(temp_a);

    //! DUMP SDCTRL parameter
    prn_string("DPCU_DT_INFO : \t********** SDCTRL Setting **********\n") ;
    temp_a = (GRP(SDC_BASE_GRP, 11) >> 25) & 0x3f;
    temp_b = (GRP(SDC_BASE_GRP, 11) >> 20) & 0x1f;
    temp_c = (GRP(SDC_BASE_GRP, 11) >>  8) & 0x0f;

    prn_string("  STR_DQS_IN=");prn_decimal(temp_b);
    prn_string("  EXT_CL_CNT=");prn_decimal(temp_a);
    prn_string("  INT_WL_CNT=");prn_decimal(temp_c);
    prn_string("  \n\n");

    //! DUMP CK0BD
    prn_string("DPCU_DT_INFO : \t********** DDRPHY Setting **********\n") ;
    temp_a = (GRP(PHY_BASE_GRP, 17) >> 16) & 0x3f;
    temp_b = (GRP(PHY_BASE_GRP, 17) >>  8) & 0x3f;
    temp_c = (GRP(PHY_BASE_GRP, 17) >>  0) & 0x3f;

    prn_string("  ACK0BD=");prn_decimal(temp_a);
    prn_string("  AC0BD=");prn_decimal(temp_b);
    prn_string("  CK0BD=");prn_decimal(temp_c);
    prn_string("  \n\n");

    //! DUMP INIT & Training flag
    temp_a = GRP(PHY_BASE_GRP, 2) & 0x3;
    temp_b = temp_a & 0x1;
    temp_c = (temp_a >> 1) & 0x1 ;
    prn_string("DPCU_DT_INFO : \t********** DUMP APHY INIT flag **********\n") ;
    prn_string("  Init  done flag =");prn_decimal(temp_b);prn_string("(0 : means don't init, 1 : means init  done) \n");
    prn_string("  Init error flag =");prn_decimal(temp_c);prn_string("(0 : means  init pass, 1 : means init error) \n\n");

    // initial error
    if(temp_c == 1) {
        prn_string("DPCU_DT_INFO : \t********** DUMP APHY INIT error information **********\n") ;
        temp_a = (GRP(PHY_BASE_GRP, 2) >> 8 ) & 0x1;
        temp_b = (GRP(PHY_BASE_GRP, 2) >> 9 ) & 0x1;
        prn_string("\tCTCAL_ERR flag =");prn_decimal(temp_a);prn_string("\tSSCPLL_ERR flag =");prn_decimal(temp_b);prn_string("\n");

        temp_a = (GRP(PHY_BASE_GRP, 2) >> 10) & 0x1;
        temp_b = (GRP(PHY_BASE_GRP, 2) >> 11) & 0x1;
        prn_string("\tDDL_ERR flag =");prn_decimal(temp_a);prn_string("\tPZQ_ERR flag =");prn_decimal(temp_b);prn_string("\n");
    }

    temp_a = GRP((PHY_BASE_GRP+1), 0) & 0x1;
    temp_b = (GRP((PHY_BASE_GRP+1), 0) >> 8) & 0x3f;

    prn_string("DPCU_DT_INFO : \t********** DUMP Training flag **********\n") ;
    prn_string("  Training  done flag =   ");prn_decimal(temp_a);prn_string("(0:don't train,     1 : means training  done) \n");
    prn_string("  Training error flag =");prn_byte(temp_b);prn_string("(0:train  pass, other : means training error) \n\n");

    // training error
    if(temp_b != 0) {
        prn_string("DPCU_DT_INFO : \t********** DUMP APHY DX0 training error information **********\n") ;
        temp_a = (GRP((PHY_BASE_GRP+2), 2) >> 8 ) & 0x1;
        temp_b = (GRP((PHY_BASE_GRP+2), 2) >> 9 ) & 0x1;
        prn_string("\tWL_ERR flag =");prn_decimal(temp_a);prn_string("\tRG_ERR flag =");prn_decimal(temp_b);prn_string("\n");

        temp_a = (GRP((PHY_BASE_GRP+2), 2) >> 10) & 0x1;
        temp_b = (GRP((PHY_BASE_GRP+2), 2) >> 11) & 0x1;
        prn_string("\tRDE_ERR flag =");prn_decimal(temp_a);prn_string("\tREYE_ERR flag =");prn_decimal(temp_b);prn_string("\n");

        temp_a = (GRP((PHY_BASE_GRP+2), 2) >> 12) & 0x1;
        temp_b = (GRP((PHY_BASE_GRP+2), 2) >> 13) & 0x1;
        prn_string("\tWDE_ERR flag =");prn_decimal(temp_a);prn_string("\tWEYE_ERR flag =");prn_decimal(temp_b);prn_string("\n");

        temp_a = (GRP((PHY_BASE_GRP+2), 2) >> 16) & 0xf;
        temp_b = (GRP((PHY_BASE_GRP+2), 2) >> 20) & 0xf;
        prn_string("\tWL_ERR infor =");prn_byte(temp_a);prn_string("\tRG_ERR infor =");prn_byte(temp_b);prn_string("\n");

        temp_a = (GRP((PHY_BASE_GRP+2), 2) >> 24) & 0xf;
        temp_b = (GRP((PHY_BASE_GRP+2), 2) >> 28) & 0xf;
        prn_string("\tREYE_ERR infor =");prn_byte(temp_a);prn_string("\tWEYE_ERR infor =");prn_byte(temp_b);prn_string("\n");

        prn_string("DPCU_DT_INFO : \t********** DUMP APHY DX1 training error information **********\n") ;
        temp_a = (GRP((PHY_BASE_GRP+3), 2) >> 8 ) & 0x1;
        temp_b = (GRP((PHY_BASE_GRP+3), 2) >> 9 ) & 0x1;
        prn_string("\tWL_ERR flag =");prn_decimal(temp_a);prn_string("\tRG_ERR flag =");prn_decimal(temp_b);prn_string("\n");

        temp_a = (GRP((PHY_BASE_GRP+3), 2) >> 10) & 0x1;
        temp_b = (GRP((PHY_BASE_GRP+3), 2) >> 11) & 0x1;
        prn_string("\tRDE_ERR flag =");prn_decimal(temp_a);prn_string("\tREYE_ERR flag =");prn_decimal(temp_b);prn_string("\n");

        temp_a = (GRP((PHY_BASE_GRP+3), 2) >> 12) & 0x1;
        temp_b = (GRP((PHY_BASE_GRP+3), 2) >> 13) & 0x1;
        prn_string("\tWDE_ERR flag =");prn_decimal(temp_a);prn_string("\tWEYE_ERR flag =");prn_decimal(temp_b);prn_string("\n");

        temp_a = (GRP((PHY_BASE_GRP+3), 2) >> 16) & 0xf;
        temp_b = (GRP((PHY_BASE_GRP+3), 2) >> 20) & 0xf;
        prn_string("\tWL_ERR infor =");prn_byte(temp_a);prn_string("\tRG_ERR infor =");prn_byte(temp_b);prn_string("\n");

        temp_a = (GRP((PHY_BASE_GRP+3), 2) >> 24) & 0xf;
        temp_b = (GRP((PHY_BASE_GRP+3), 2) >> 28) & 0xf;
        prn_string("\tREYE_ERR infor =");prn_byte(temp_a);prn_string("\tWEYE_ERR infor =");prn_byte(temp_b);prn_string("\n");
    }

    //! DUMP IPRD register
    prn_string("DPCU_DT_INFO : \t********** DUMP initial DDR period **********\n") ;
    prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 3)  >>  0) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 3)  >>  0) & 0xff;
    prn_string("\tDX0 :   WL_IPRD =");prn_byte(temp_a);prn_string("\tDX1 :   WL_IPRD =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 3)  >>  8) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 3)  >>  8) & 0xff;
    prn_string("\tDX0 :   RG_IPRD =");prn_byte(temp_a);prn_string("\tDX1 :   RG_IPRD =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 3)  >> 16) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 3)  >> 16) & 0xff;
    prn_string("\tDX0 : RDQS_IPRD =");prn_byte(temp_a);prn_string("\tDX1 : RDQS_IPRD =");prn_byte(temp_b);prn_string("\n") ;

    if ( temp_a > temp_b) {
        RDQS_IPRD_TAP_NO = temp_b;
    } else {
        RDQS_IPRD_TAP_NO = temp_a;
    }

    temp_a = (GRP((PHY_BASE_GRP+2), 3)  >> 24) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 3)  >> 24) & 0xff;
    prn_string("\tDX0 : WDQS_IPRD =");prn_byte(temp_a);prn_string("\tDX1 : WDQS_IPRD =");prn_byte(temp_b);prn_string("\n\n") ;

    //! DUMP PSD register
    prn_string("DPCU_DT_INFO : \t********** DUMP Training PSD status **********\n") ;
    prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 4)  >>  0) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 4)  >>  0) & 0xff;
    prn_string("\tDX0 :   WL_PSD =");prn_byte(temp_a);prn_string("\tDX1 :   WL_PSD =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 4)  >>  8) & 0x3;
    temp_b = (GRP((PHY_BASE_GRP+3), 4)  >>  8) & 0x3;
    prn_string("\tDX0 :   WL_SEL =");prn_byte(temp_a);prn_string("\tDX1 :   WL_SEL =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 6)  >>  0) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 6)  >>  0) & 0xff;
    prn_string("\tDX0 : REYE_PSD =");prn_byte(temp_a);prn_string("\tDX1 : REYE_PSD =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 7)  >>  0) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 7)  >>  0) & 0xff;
    prn_string("\tDX0 : WEYE_PSD =");prn_byte(temp_a);prn_string("\tDX1 : WEYE_PSD =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 14)  >>  0) & 0x1f;
    temp_b = (GRP((PHY_BASE_GRP+3), 14)  >>  0) & 0x1f;
    prn_string("\tDX0 :   RG_RSL =");prn_byte(temp_a);prn_string("\tDX1 :   RG_RSL =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 14)  >>  8) & 0x3;
    temp_b = (GRP((PHY_BASE_GRP+3), 14)  >>  8) & 0x3;
    prn_string("\tDX0 :   RG_PHA =");prn_byte(temp_a);prn_string("\tDX1 :   RG_PHA =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 14)  >>  16) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 14)  >>  16) & 0xff;
    prn_string("\tDX0 :   RG_PSD =");prn_byte(temp_a);prn_string("\tDX1 :   RG_PSD =");prn_byte(temp_b);prn_string("\n\n") ;

    prn_string("DPCU_DT_INFO : \t********** DUMP RG L-side & R-side status **********\n") ;
    prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>   0) & 0x1f;
    temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>   0) & 0x1f;
    prn_string("\tDX0 : L_SIDE_RSL =");prn_byte(temp_a);prn_string("\tDX1 : L_SIDE_RSL =");prn_byte(temp_b);prn_string("\n") ;

    data_byte_0_RDQSG_left_total_tap = temp_a * RDQS_IPRD_TAP_NO;
    data_byte_1_RDQSG_left_total_tap = temp_b * RDQS_IPRD_TAP_NO;

    temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>   5) & 0x3;
    temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>   5) & 0x3;
    prn_string("\tDX0 : L_SIDE_PHA =");prn_byte(temp_a);prn_string("\tDX1 : L_SIDE_PHA =");prn_byte(temp_b);prn_string("\n") ;

    data_byte_0_RDQSG_left_total_tap = (temp_a * RDQS_IPRD_TAP_NO/2) + data_byte_0_RDQSG_left_total_tap;
    data_byte_1_RDQSG_left_total_tap = (temp_b * RDQS_IPRD_TAP_NO/2) + data_byte_1_RDQSG_left_total_tap;

    temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>   8) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>   8) & 0xff;
    prn_string("\tDX0 : L_SIDE_PSD =");prn_byte(temp_a);prn_string("\tDX1 : L_SIDE_PSD =");prn_byte(temp_b);prn_string("\n") ;

    data_byte_0_RDQSG_left_total_tap = temp_a + data_byte_0_RDQSG_left_total_tap;
    data_byte_1_RDQSG_left_total_tap = temp_b + data_byte_1_RDQSG_left_total_tap;

    temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>  16) & 0x1f;
    temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>  16) & 0x1f;
    prn_string("\tDX0 : R_SIDE_RSL =");prn_byte(temp_a);prn_string("\tDX1 : R_SIDE_RSL =");prn_byte(temp_b);prn_string("\n") ;

    data_byte_0_RDQSG_right_total_tap = temp_a * RDQS_IPRD_TAP_NO;
    data_byte_1_RDQSG_right_total_tap = temp_b * RDQS_IPRD_TAP_NO;

    temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>  21) & 0x3;
    temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>  21) & 0x3;
    prn_string("\tDX0 : R_SIDE_PHA =");prn_byte(temp_a);prn_string("\tDX1 : R_SIDE_PHA =");prn_byte(temp_b);prn_string("\n") ;

    data_byte_0_RDQSG_right_total_tap = (temp_a * RDQS_IPRD_TAP_NO/2) + data_byte_0_RDQSG_right_total_tap;
    data_byte_1_RDQSG_right_total_tap = (temp_b * RDQS_IPRD_TAP_NO/2) + data_byte_1_RDQSG_right_total_tap;

    temp_a = (GRP((PHY_BASE_GRP+2), 15)  >>  24) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 15)  >>  24) & 0xff;
    prn_string("\tDX0 : R_SIDE_PSD =");prn_byte(temp_a);prn_string("\tDX1 : R_SIDE_PSD =");prn_byte(temp_b);prn_string("\n\n") ;

    data_byte_0_RDQSG_right_total_tap = temp_a + data_byte_0_RDQSG_right_total_tap;
    data_byte_1_RDQSG_right_total_tap = temp_b + data_byte_1_RDQSG_right_total_tap;

    prn_string("DPCU_DT_INFO : \t********** DUMP R/W EYE status **********\n") ;
    prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 16)  >>   0) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 16)  >>   0) & 0xff;
    prn_string("\tDX0 : REYE_MIN =");prn_byte(temp_a);prn_string("\tDX1 : REYE_MIN =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 16)  >>   8) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 16)  >>   8) & 0xff;
    prn_string("\tDX0 : REYE_MAX =");prn_byte(temp_a);prn_string("\tDX1 : REYE_MAX =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 16)  >>  16) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 16)  >>  16) & 0xff;
    prn_string("\tDX0 : WEYE_MIN =");prn_byte(temp_a);prn_string("\tDX1 : WEYE_MIN =");prn_byte(temp_b);prn_string("\n") ;

    temp_a = (GRP((PHY_BASE_GRP+2), 16)  >>  24) & 0xff;
    temp_b = (GRP((PHY_BASE_GRP+3), 16)  >>  24) & 0xff;
    prn_string("\tDX0 : WEYE_MAX =");prn_byte(temp_a);prn_string("\tDX1 : WEYE_MAX =");prn_byte(temp_b);prn_string("\n\n") ;

    if( !only_dump_PSD ){
        prn_string("DPCU_DT_INFO : \t********** DUMP REYE BDD status **********\n") ;
        prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 11)  >>   0) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 11)  >>   0) & 0x3f;
        prn_string("\tDX0 : RDQS_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQS_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 12)  >>   0) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 12)  >>   0) & 0x3f;
        prn_string("\tDX0 : RDQ0_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQ0_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 12)  >>   8) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 12)  >>   8) & 0x3f;
        prn_string("\tDX0 : RDQ1_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQ1_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 12)  >>  16) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 12)  >>  16) & 0x3f;
        prn_string("\tDX0 : RDQ2_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQ2_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 12)  >>  24) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 12)  >>  24) & 0x3f;
        prn_string("\tDX0 : RDQ3_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQ3_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 13)  >>   0) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 13)  >>   0) & 0x3f;
        prn_string("\tDX0 : RDQ4_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQ4_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 13)  >>   8) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 13)  >>   8) & 0x3f;
        prn_string("\tDX0 : RDQ5_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQ5_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 13)  >>  16) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 13)  >>  16) & 0x3f;
        prn_string("\tDX0 : RDQ6_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQ6_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 13)  >>  24) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 13)  >>  24) & 0x3f;
        prn_string("\tDX0 : RDQ7_BDD =");prn_byte(temp_a);prn_string("\tDX1 : RDQ7_BDD =");prn_byte(temp_b);prn_string("\n\n") ;

        prn_string("DPCU_DT_INFO : \t********** DUMP WEYE BDD status **********\n") ;
        prn_string("\t[DATx8-0]\t\t[DATx8-1]\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2),  8)  >>   0) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3),  8)  >>   0) & 0x3f;
        prn_string("\tDX0 :     WDM_BDD =");prn_byte(temp_a);prn_string("\tDX1 :     WDM_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2),  8)  >>   8) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3),  8)  >>   8) & 0x3f;
        prn_string("\tDX0 :    WDQS_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQS_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2),  8)  >>  16) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3),  8)  >>  16) & 0x3f;
        prn_string("\tDX0 : WDQS_OE_BDD =");prn_byte(temp_a);prn_string("\tDX1 : WDQS_OE_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2),  8)  >>  24) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3),  8)  >>  24) & 0x3f;
        prn_string("\tDX0 :  WDQ_OE_BDD =");prn_byte(temp_a);prn_string("\tDX1 :  WDQ_OE_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2),  9)  >>   0) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3),  9)  >>   0) & 0x3f;
        prn_string("\tDX0 :    WDQ0_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQ0_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2),  9)  >>   8) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3),  9)  >>   8) & 0x3f;
        prn_string("\tDX0 :    WDQ1_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQ1_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2),  9)  >>  16) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3),  9)  >>  16) & 0x3f;
        prn_string("\tDX0 :    WDQ2_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQ2_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2),  9)  >>  24) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3),  9)  >>  24) & 0x3f;
        prn_string("\tDX0 :    WDQ3_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQ3_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 10)  >>   0) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 10)  >>   0) & 0x3f;
        prn_string("\tDX0 :    WDQ4_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQ4_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 10)  >>   8) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 10)  >>   8) & 0x3f;
        prn_string("\tDX0 :    WDQ5_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQ5_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 10)  >>  16) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 10)  >>  16) & 0x3f;
        prn_string("\tDX0 :    WDQ6_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQ6_BDD =");prn_byte(temp_b);prn_string("\n") ;

        temp_a = (GRP((PHY_BASE_GRP+2), 10)  >>  24) & 0x3f;
        temp_b = (GRP((PHY_BASE_GRP+3), 10)  >>  24) & 0x3f;
        prn_string("\tDX0 :    WDQ7_BDD =");prn_byte(temp_a);prn_string("\tDX1 :    WDQ7_BDD =");prn_byte(temp_b);prn_string("\n\n") ;
    }

    prn_string("\t-----------------------------------------------------\n\n") ;
    //! finish
    prn_string("DPCU_INFO : ----- DPCU dump done -----\n\n") ;
}//! end function => DPCU_DT_RESULT_DUMP


//! ***********************************************************************
//! * FUNC      : ASSERT_SDC_PHY_RESET
//! * PARAM     : DRAM_ID
//! * PURPOSE   : assert reset
//! ***********************************************************************
void assert_sdc_phy_reset ( unsigned int DRAM_ID )
{
#ifdef PLATFORM_3502 // 3502 module reset
    if(DRAM_ID == 0 ) {
        GRP(0, 16)  = 0x00014000    ;   // Asserted reset of SDC0/1 & PHY0/1
    } else {
        GRP(0, 16)  = 0x00028000    ;   // Asserted reset of SDC0/1 & PHY0/1
    }
#else // gemini
    if(DRAM_ID == 0 ) {
        GRP(0, 17) |= (
		(1 << 14) | // SDCTRL0_RESET
		(1 << 16)   // DDR_PHY0_RESET
	);
    }
#endif
} //! end function => assert_sdc_phy_reset

//! ***********************************************************************
//! * FUNC      : release_sdc_phy_reset
//! * PARAM     : void
//! * PURPOSE   : release reset
//! ***********************************************************************
void release_sdc_phy_reset ( void )
{
#ifdef PLATFORM_3502 // 3502 module reset release
    GRP(0, 16)  = 0x00000000    ;
#else // gemini
    GRP(0, 17) &= ~(
                (1 << 14) | // SDCTRL0_RESET
                (1 << 16)   // DDR_PHY0_RESET
        );
#endif
}//! end of release_sdc_phy_reset

//! ***********************************************************************
//! * FUNC      : do_system_reset_flow
//! * PARAM     : DRAM_ID
//! * PURPOSE   : do SDC & PHY reset flow
//! ***********************************************************************
void do_system_reset_flow( unsigned int DRAM_ID )
{
    assert_sdc_phy_reset ( DRAM_ID ) ;
    wait_loop(1000) ;
    release_sdc_phy_reset () ;
} //!end of do_system_reset_flow

#if 0 // DRAM_SELF_REFRESH_ON, this function is not used, mark it for reducing size
//! ***********************************************************************
//! * FUNC      : DRAM_SELF_REFRESH_ON
//! * PARAM     : DRAM_TEST_SIZE , DRAM_ID
//! * PURPOSE   : 1. Write data
//! *             2. Setting sref-refresh flow
//! *             3. DDR IO pad ISO enable
//! *             4. MO_RST close SDCTRL and DPCU vaule(or power down )
//! ***********************************************************************
int DRAM_SELF_REFRESH_ON( unsigned int DRAM_TEST_SIZE , unsigned int DRAM_ID )
{
    unsigned int    i = 0   ;
    unsigned int    rgst_value ;
    unsigned int    dram_value_sref_on = 0   ;
    unsigned int    dram_0_value_sref_on = 0   ;
    volatile unsigned int *ram_sref_0 = (volatile unsigned int *)TEST_DRAM_0_SREF;
    volatile unsigned int *ram_sref_1 = (volatile unsigned int *)TEST_DRAM_1_SREF;

    prn_string("\n\nDRAM-");prn_decimal(DRAM_ID);prn_string(" CPU access start for self_refresh on !!!");

    if( DRAM_ID==0 ) {
    //! ***********************************************************************
    //! Step 1:  Write data to dram0
    //! ***********************************************************************
        //! write data to DRAM0
        prn_string("\nReady to WR DRAM 0...\n");
        for( i=0 ; i<DRAM_TEST_SIZE ; i++ ){
            dram_value_sref_on = ram_sref_0+i ;
            ram_sref_0[i] = i;

			  prn_string("\n Write Address of DRAM 0 Access sref on test :");prn_dword( ram_sref_0+i);
			  prn_string(" ; Write sref on value to DRAM 0:");prn_dword(i); prn_string("\n :");
        }

    prn_string("\n DRAM 0 wait some time \n");
    for (i=0; i<10000; i++) {
    asm volatile ("nop");
    }
    //! ***********************************************************************
    //! Step 2:  Setting sref-refresh flow
    //! ***********************************************************************
    //! ***********************************************************************
    prn_string("\n DRAM-0 self-refresh rgst setting start \n");
    //! ***********************************************************************

    // SREF_EN_MODE=2'b10, bit[9:8] ;
    rgst_value = GRP(DRAM_0_SDC_REG_BASE, 8) ;
    prn_string("G33 ADDR 08=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 8));prn_string("\n ");
    //GRP(33, 8) = (rgst_value | (2<<8)) ;
    GRP(DRAM_0_SDC_REG_BASE, 8) = 0x00000283 ;

    // Disable init_done
    rgst_value = GRP(DRAM_0_SDC_REG_BASE, 18)  ;
    prn_string("G33 ADDR18=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 18));prn_string("\n ");
    GRP(DRAM_0_SDC_REG_BASE, 18) = 0x00000000 ;

    // Single CMD trigger enable
    rgst_value = GRP(DRAM_0_SDC_REG_BASE, 8)  ;
    prn_string("G33 ADDR 08=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 8));prn_string("\n ");
    GRP(DRAM_0_SDC_REG_BASE, 8) = (rgst_value | (1<<16)) ;

    // Single CMD PREA CMD setting
    rgst_value = GRP(DRAM_0_SDC_REG_BASE, 8)  ;
    prn_string("G33 ADDR 08=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 8));prn_string("\n ");
    GRP(DRAM_0_SDC_REG_BASE, 8) = (rgst_value | (3<<16)) ;

    //!// CKE = SREF_CKE, SREF_CA_EN = 1;
    //!rgst_value = GRP(DRAM_0_SDC_REG_BASE, 10)  ;
    //!prn_string("G33 ADDR10=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 10));prn_string("\n ");
    //!GRP(DRAM_0_SDC_REG_BASE, 10) = 0x00006013;
    //!
    //!// Setting SD_DDR3_CKE = 0 ;
    //!rgst_value = GRP(DRAM_0_SDC_REG_BASE, 31)  ;
    //!prn_string("G33 ADDR31=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 31));prn_string("\n ");
    //!GRP(DRAM_0_SDC_REG_BASE, 31) = 0x00000002;
    //!
    //!// Setting SREF_LEN = 0 ;
    //!GRP(DRAM_0_SDC_REG_BASE,  8) = 0x00030282;
    //!
    //!// SREF_CMD and setting SREF_MUX_EN
    //!rgst_value = GRP(DRAM_0_SDC_REG_BASE, 10)  ;
    //!prn_string("G33 ADDR10=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 10));prn_string("\n ");
    //!GRP(DRAM_0_SDC_REG_BASE, 10) = 0x00004813;
    
    /// CKE = SREF_CKE, SREF_CA_EN = 1;
    rgst_value = GRP(DRAM_0_SDC_REG_BASE, 10)  ;
    GRP(DRAM_0_SDC_REG_BASE, 10) = 0x00006013;
    
    // SREF_CMD 
    rgst_value = GRP(DRAM_0_SDC_REG_BASE, 10)  ;
    GRP(DRAM_0_SDC_REG_BASE, 10) = 0x00006813;

    //! ***********************************************************************
    //! Step 3:  DDR IO pad ISO enable
    //! ***********************************************************************
    //! ***********************************************************************
    prn_string("\n DRAM-0 self-refresh on rgst setting done \n");
    //! ***********************************************************************

    // Setting DDR IO PAD ISO on
    rgst_value = GRP(8, 0)  ;
    // prn_string("G8 ADDR 0=");prn_dword(GRP(8, 0));prn_string("\n ");
    GRP(8, 0) = (rgst_value | (1<<8));

    //! ***********************************************************************
    //! Step 4:  MO_RST close SDCTRL and DPCU vaule
    //! ***********************************************************************

     prn_string("\n DRAM 0 wait some time \n");
     for (i=0; i<10000; i++) {
     asm volatile ("nop");
     };
     //Function from plf_dram_3502A.c
     //GRP(0,16) = 0x00014000; DRAM0
     //assert_sdc_phy_reset( DRAM_ID );

    } //end DRAM-0 self-refresh setting

      if( DRAM_ID==1 ) {

    //! ***********************************************************************
    //! Step 1:  Write data to dram1
    //! ***********************************************************************

        //! write data to DRAM 1
        prn_string("\nReady to WR DRAM 1...\n");
        for( i=0 ; i<DRAM_TEST_SIZE ; i++ ){
            dram_value_sref_on = ram_sref_1+i ;
            ram_sref_1[i] = i;

			  prn_string("\n Write Address of DRAM 1 Access sref on test :");prn_dword( ram_sref_1+i);
			  prn_string(" ; Write sref on value to DRAM 1 :");prn_dword(i); prn_string("\n :");
        }

    prn_string("\n DRAM 1 wait some time \n");
    for (i=0; i<10000; i++) {
    asm volatile ("nop");
    }
    //! ***********************************************************************
    //! Step 2:  Setting sref-refresh flow
    //! ***********************************************************************
    //! ***********************************************************************
    prn_string("\n DRAM-1 self-refresh rgst setting start \n");
    //! ***********************************************************************

    // SREF_EN_MODE=2'b10, bit[9:8] ;
    rgst_value = GRP(DRAM_1_SDC_REG_BASE, 8) ;
    prn_string("G46 ADDR 08=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 8));prn_string("\n ");
    GRP(DRAM_1_SDC_REG_BASE, 8) = 0x00000283 ;

    // Disable init_done
    rgst_value = GRP(DRAM_1_SDC_REG_BASE, 18)  ;
    prn_string("G46 ADDR18=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 18));prn_string("\n ");
    GRP(DRAM_1_SDC_REG_BASE, 18) = 0x00000000 ;

    // Single CMD trigger enable
    rgst_value = GRP(DRAM_1_SDC_REG_BASE, 8)  ;
    prn_string("G46 ADDR 08=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 8));prn_string("\n ");
    GRP(DRAM_1_SDC_REG_BASE, 8) = (rgst_value | (1<<16)) ;

    // Single CMD PREA CMD setting
    rgst_value = GRP(DRAM_1_SDC_REG_BASE, 8)  ;
    prn_string("G46 ADDR 08=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 8));prn_string("\n ");
    GRP(DRAM_1_SDC_REG_BASE, 8) = (rgst_value | (3<<16)) ;

    //!// CKE = SREF_CKE, SREF_CA_EN = 1;
    //!rgst_value = GRP(DRAM_1_SDC_REG_BASE, 10)  ;
    //!prn_string("G46 ADDR10=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 10));prn_string("\n ");
    //!GRP(DRAM_1_SDC_REG_BASE, 10) = 0x00006013;
    //!
    //!// Setting SD_DDR3_CKE = 1 ;
    //!rgst_value = GRP(DRAM_1_SDC_REG_BASE, 31)  ;
    //!prn_string("G46 ADDR31=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 31));prn_string("\n ");
    //!GRP(DRAM_1_SDC_REG_BASE, 31) = 0x00000002;
    //!
    //!// Setting SREF_LEN = 0 ;
    //!GRP(DRAM_1_SDC_REG_BASE,  8) = 0x00030282;
    //!
    //!// SREF_CMD and setting SREF_MUX_EN
    //!rgst_value = GRP(DRAM_1_SDC_REG_BASE, 10)  ;
    //!prn_string("G46 ADDR10=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 10));prn_string("\n ");
    //!GRP(DRAM_1_SDC_REG_BASE, 10) = 0x00004813;

    // CKE = SREF_CKE, SREF_CA_EN = 1;
    rgst_value = GRP(DRAM_1_SDC_REG_BASE, 10)  ;
    GRP(DRAM_1_SDC_REG_BASE, 10) = 0x00006013;
    
    // SREF_CMD 
    rgst_value = GRP(DRAM_1_SDC_REG_BASE, 10)  ;
    GRP(DRAM_1_SDC_REG_BASE, 10) = 0x00006813;

    //! ***********************************************************************
    prn_string("\n DRAM-1 self-refresh on rgst setting done \n");
    //! ***********************************************************************

    //! ***********************************************************************
    //! Step 3:  DDR IO pad ISO enable
    //! ***********************************************************************

    // Setting DDR IO PAD ISO on
    rgst_value = GRP(8, 0)  ;
    // prn_string("G8 ADDR 0=");prn_dword(GRP(8, 0));prn_string("\n ");
    GRP(8, 0) = (rgst_value | (1<<10));
		    //Gino
		    rgst_value = GRP(8, 0)  ; 
		    prn_string("(GRP(8,0)=");prn_dword((GRP((8), 0)));prn_string("!!\n"); 
		    prn_string("(DRAM_ID)=132123");
    //! ***********************************************************************
    //! Step 4:  MO_RST close SDCTRL and DPCU vaule
    //! ***********************************************************************

     prn_string("\n DRAM 0 wait some time \n");
     for (i=0; i<10000; i++) {
     asm volatile ("nop");
     };
     //Function from plf_dram_3502A.c
     //GRP(0,16) = 0x00028000; DRAM1
     //assert_sdc_phy_reset( DRAM_ID );

    }//end DRAM-1 self-refresh setting


    prn_string("\n DRAM wait some time \n");
    for (i=0; i<100000; i++) {
      asm volatile ("nop");
    }
    return 1 ;

}//! end - DRAM_SELF_REFRESH_ON
#endif // DRAM_SELF_REFRESH_ON

#if 0 // not used
//! ***********************************************************************
//! * FUNC      : NORMAL_TRIM_TEST
//! * PARAM     : test_round , DRAM_ID
//! * PURPOSE   : to do the random DRAM access by SDCTRL.TRIMMER
//! ***********************************************************************
int NORMAL_TRIM_TEST_( unsigned int test_round , unsigned int DRAM_ID)
{
    volatile unsigned int pass_cnt = 0  ;
    volatile unsigned int fail_cnt = 0  ;
    volatile unsigned int test_cnt = 1  ;
    volatile unsigned int i = 0         ;

    prn_string("SDCTRL-");prn_decimal(DRAM_ID);prn_string("trimmer test start ......\n");


    while( test_cnt <= test_round ) {

        prn_string("\n!!! SDCTRL TRIMMER Round-");prn_decimal(test_cnt);prn_string(" Test start !!! \n");

        pass_cnt = 0 ;
        fail_cnt = 0 ;

        //! SDRAM trimmer - code :
        for(i=0; i<100; i++) {
            if( SDCTRL_TRIMMER_TEST_(DRAM_ID) == 0 )
            {
                prn_string("        ---- DRAM-");prn_decimal(DRAM_ID);
                prn_string(" TRIMMER Test Result : Round-");prn_decimal(i);prn_string(" - Fail ?????\n");
                // RID_FAIL() ;
                fail_cnt += 1 ;
            }
            else
            {
                pass_cnt += 1 ;
            }
        }//! end for

        prn_string("        ---- DRAM-");prn_decimal(DRAM_ID);
        prn_string("TRIMMER Test PASS-");prn_decimal(pass_cnt);
        prn_string(" FAIL-");prn_decimal(fail_cnt);prn_string("-----\n\n");

        test_cnt += 1 ;
    } //! end while

    if( fail_cnt==0 ){
        prn_string("SDCTRL-");prn_decimal(DRAM_ID);prn_string("trimmer test PASS ...\n\n");
        return  1   ;
    } else {
        prn_string("SDCTRL-");prn_decimal(DRAM_ID);prn_string("trimmer test FAIL ...\n\n");
        return  0   ;
    }
}//! end of NORMAL_TRIM_TEST
#endif


//! ***********************************************************************
//! * FUNC      : SDCTRL_TRIMMER_TEST_MODES
//! * PARAM     : Trimmer_mode ; DRAM_ID
//! * PURPOSE   : trigger SDC.trimmer
//! * FLOW      :
//! - 1. SET Trimmer parameters
//! - 2. Enable Trimmer Only
//! - 3. Monitor Trimming Finish
//! - 4. Check if All Pass.
//! * PS : the same function in dram_scan.c
//! ***********************************************************************
int SDCTRL_TRIMMER_TEST_MODES_( unsigned int mode , unsigned int DRAM_ID )
{
    unsigned int        i, rgst_value       ;

    unsigned int        SDC_BASE_GRP=0      ,
                        TRM_BASE_GRP=0      ;   //! trimmer base rgst group

    unsigned int        trim_test_done      ;

    unsigned int        trim_rd_err_cnt     ;

    //! -------------------------------------------------------
    //! 0. SDCTRL / DDR_PHY RGST GRP selection
    //! -------------------------------------------------------
    switch ( DRAM_ID ){
        case 0: {
            SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE     ; //! SDC_0 is 33
            TRM_BASE_GRP    =   DRAM_0_SDC_REG_BASE + 9 ; //! TRM_0 is 42
            break;
        }
        case 1: {
            SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE     ; //! SDC_1 is 46
            TRM_BASE_GRP    =   DRAM_1_SDC_REG_BASE + 9 ; //! TRM_0 is 55
            break;
        }
    }//! end switch

    //! -------------------------------------------------------
    //! 1. set trimmer parameters
    //! -------------------------------------------------------
    GRP(SDC_BASE_GRP, 3) = 0x00060 ; // DISABLE Max Burst length, (96 words)

    //! Disable Trimmer Only
    //! TRIMMER_ONLY_EN = 1'b0;
    //! DATA = {DATA[31:17], BYTE_CHECK_EN[7:0], TRIMMER_ID[4:0], TRIM_REPEAT_INF,
    //! DATA[2], TRIM_ONLY_EN, NORMAL_AND_TRIM_EN};
    GRP(TRM_BASE_GRP, 1) = 0x0001ffe0;

    //! SDRAM trimmer error record
    rgst_value = GRP(TRM_BASE_GRP, 9)   ;
    trim_test_done = rgst_value & 0xff  ;


    switch(mode){

	    case 0: {
            //! //prn_string(" !!!!! In SDCLTR_TRIMMER_TEST mode-0 !!!!\n");
            //  TWITCH
            //  DATA = {TOTAL_DATA_LEN[31:16],  DATA[15:8], TRIM_REPEAT_NUM[7:0]};
            GRP(TRM_BASE_GRP, 2) = 0x1000000a   ;   //! total length to 256Byte, repeat 10 time

            //  DATA = {W_BL_ADDER_SEED[3:0], W_BL_MODE[3:0], R_BL_ADDER_SEED[3:0], R_BL_MODE[3:0],
            //  DATA[15:7], CON_WDATA_BOND_SEL[2:0], CMD_MODE[3:0]};
            GRP(TRM_BASE_GRP, 4) = 0x08080000   ; // CMD_MODE: WWWW......RRR  // W_BL:16, R_BL:16

            //  DATA = {DATA[31] ,DATA_JUMP_SEL[2:0] ,DATA_PRIME_SEED[7:0] ,DATA_MODE[3:0] ,
            //  DATA[15:12],ADR_BL_OFFSET[7:0] ,ADR_MODE[3:0] };
            GRP(TRM_BASE_GRP, 5) = 0x10000000   ; // initial data to let data bus critical
            GRP(TRM_BASE_GRP, 6) = 0x12345678   ;
            GRP(TRM_BASE_GRP, 7) = 0xfedcba90   ; // initial data to let data bus critical
            GRP(TRM_BASE_GRP, 8) = 0x00000000   ; // Address : VA: 0xa0000000, DRAM Address: 0x0
            break;
        }

	    case 1: { // CMD_CRITICAL
            //! //prn_string(" !!!!! In SDCLTR_TRIMMER_TEST mode-1 !!!!\n");
            //  DATA = {TOTAL_DATA_LEN[31:16],  DATA[15:8], TRIM_REPEAT_NUM[7:0]};
            GRP(TRM_BASE_GRP, 2) =0x0080000a    ; //! total length to 256Byte, repeat 10 time

            //  DATA = {W_BL_ADDER_SEED[3:0], W_BL_MODE[3:0], R_BL_ADDER_SEED[3:0], R_BL_MODE[3:0],
            //  DATA[15:7], CON_WDATA_BOND_SEL[2:0], CMD_MODE[3:0]};
            GRP(TRM_BASE_GRP, 4) = 0x04040012   ; // CMD_MODE: WWWW...WRWR...RRR  // W_BL:5, R_BL:5

            //  DATA = {DATA[31] ,DATA_JUMP_SEL[2:0] ,DATA_PRIME_SEED[7:0] ,DATA_MODE[3:0] ,
            //  DATA[15:12],ADR_BL_OFFSET[7:0] ,ADR_MODE[3:0] };
            GRP(TRM_BASE_GRP, 5) = 0x00080003   ;  // ADDR_MODE: change page
            GRP(TRM_BASE_GRP, 6) = 0x00010000   ;
            GRP(TRM_BASE_GRP, 7) = 0xfffffffe   ; // initial data to let data bus critical
            GRP(TRM_BASE_GRP, 8) = 0x00000000   ; // Address : VA: 0xa0000000, DRAM Address: 0x0
            break;
        }

        case 2: {   // DATA_CRITICAL
            //prn_string(" !!!!! In SDCLTR_TRIMMER_TEST mode-2 !!!!\n");
            //  DATA = {TOTAL_DATA_LEN[31:16],  DATA[15:8], TRIM_REPEAT_NUM[7:0]};
            GRP(TRM_BASE_GRP, 2) =0x0080000a; //! total length to 256Byte, repeat 10 time

            //  DATA = {W_BL_ADDER_SEED[3:0], W_BL_MODE[3:0], R_BL_ADDER_SEED[3:0], R_BL_MODE[3:0],
            //  DATA[15:7], CON_WDATA_BOND_SEL[2:0], CMD_MODE[3:0]};
            GRP(TRM_BASE_GRP, 4) = 0x08080012; // CMD_MODE: WWWW...WRWR...RRR

            //  DATA = {DATA[31] ,DATA_JUMP_SEL[2:0] ,DATA_PRIME_SEED[7:0] ,DATA_MODE[3:0] ,
            //  DATA[15:12],ADR_BL_OFFSET[7:0] ,ADR_MODE[3:0] };
            GRP(TRM_BASE_GRP, 5) = 0x00080000;  // DATA_MODE: DATA_N = DATA + 64'h0000_0000_0002_0002 - 64'h0002_0002_0000_0000
            GRP(TRM_BASE_GRP, 6) = 0x00010000;
            GRP(TRM_BASE_GRP, 7) = 0xfffffffe; // initial data to let data bus critical
            GRP(TRM_BASE_GRP, 8) = 0x00000000; // Address : VA: 0xa0000000, DRAM Address: 0x0
            break;
        }

        case 3: { // RANDOM
            //prn_string(" !!!!! In SDCLTR_TRIMMER_TEST mode-3 !!!!\n");
            //  DATA = {TOTAL_DATA_LEN[31:16],  DATA[15:8], TRIM_REPEAT_NUM[7:0]};
            GRP(TRM_BASE_GRP, 2) =0x1000000a; //! total length to 256Byte, repeat 10 time

            //  DATA = {W_BL_ADDER_SEED[3:0], W_BL_MODE[3:0], R_BL_ADDER_SEED[3:0], R_BL_MODE[3:0],
            //  DATA[15:7], CON_WDATA_BOND_SEL[2:0], CMD_MODE[3:0]};
            GRP(TRM_BASE_GRP, 4) = 0x3a3a0012; // CMD_MODE: WWWW...WRWR...RRR  // W_BL:random, R_BL:8
            //  DATA = {DATA[31] ,DATA_JUMP_SEL[2:0] ,DATA_PRIME_SEED[7:0] ,DATA_MODE[3:0] ,
            //  DATA[15:12],ADR_BL_OFFSET[7:0] ,ADR_MODE[3:0] };
            GRP(TRM_BASE_GRP, 5) = 0x0a5a0000;  // DATA_MODE: DATA_N = DATA_C + 8{SEED}; SEED: a5
            GRP(TRM_BASE_GRP, 6) = 0xf2edab56;
            GRP(TRM_BASE_GRP, 7) = 0x1bed2ca5; // initial data random
            GRP(TRM_BASE_GRP, 8) = 0x00000000; // Address : VA: 0xa0000000, DRAM Address: 0x0
            break;
        }

        case 4: { // RANDOM
            //prn_string(" !!!!! In SDCLTR_TRIMMER_TEST mode-4 !!!!\n");
            //  DATA = {TOTAL_DATA_LEN[31:16],  DATA[15:8], TRIM_REPEAT_NUM[7:0]};
            GRP(TRM_BASE_GRP, 2) =0x0080000a; //! total length to 256Byte, repeat 10 time

            //  DATA = {W_BL_ADDER_SEED[3:0], W_BL_MODE[3:0], R_BL_ADDER_SEED[3:0], R_BL_MODE[3:0],
            //  DATA[15:7], CON_WDATA_BOND_SEL[2:0], CMD_MODE[3:0]};
            GRP(TRM_BASE_GRP, 4) = 0x32320010; // CMD_MODE: WWWW...WRWR...RRR  // W_BL:random, R_BL:8
            //  DATA = {DATA[31] ,DATA_JUMP_SEL[2:0] ,DATA_PRIME_SEED[7:0] ,DATA_MODE[3:0] ,
            //  DATA[15:12],ADR_BL_OFFSET[7:0] ,ADR_MODE[3:0] };
            GRP(TRM_BASE_GRP, 5) = 0x0a510081;  // DATA_MODE: DATA_N = DATA_C + 8{SEED}; SEED: a5
            GRP(TRM_BASE_GRP, 6) = 0xfedcab0f;
            GRP(TRM_BASE_GRP, 7) = 0x01234567; // initial data random
            GRP(TRM_BASE_GRP, 8) = 0x00000000; // Address : VA: 0xa0000000, DRAM Address: 0x0S
            break;
        }

	    default : {  break; }
    }//! end switch => SDCLTR_TRIMMER_TEST_MODES


    //! -------------------------------------------------------
    //! 2. Enable Trimmer Only
    //! -------------------------------------------------------
    GRP(TRM_BASE_GRP, 1)= 0x0001ffe2 ;

    trim_test_done = 0 ;

    //! -------------------------------------------------------
    //! 3. Monitor Trimming Finish
    //! -------------------------------------------------------
    for ( i=0 ; (i<10000000) && (trim_test_done== 0) ; i++ ) {
        trim_test_done = GRP(TRM_BASE_GRP,1);
        trim_test_done = (trim_test_done & 0x0004);
    }//! end for


    //! -------------------------------------------------------
    //! 4. Check trimmer result
    //! -------------------------------------------------------
    if( i >= 10000000 ) {
        //! trimmer timeout failed
        prn_string(" Trimmer Time Out : Trimmer-");prn_decimal(DRAM_ID);prn_string("test failed !!!\n");
        return 0;
    }

    //! load the trimmer error flag
    trim_test_done = GRP(TRM_BASE_GRP,9) & 0xff ;

    if( (trim_test_done == 0) ) {
        //prn_string(" ***** DRAM_%d Trimmer Test Pass ***** \n", DRAM_ID);
        //! Disable Trimmer Only
        //! TRIMMER_ONLY_EN = 1'b0;
        GRP(TRM_BASE_GRP, 1)= 0x0001ffe0 ;
        return 1;
    } else {
        //! Display Trimmer Fial message when CPU Test pass case.
        #ifdef DBG_EN_TRIM_INFO
            trim_rd_err_cnt = (GRP(TRM_BASE_GRP,9) >> 16) ;
            prn_string("TRIM_INFO => DRAM-");prn_decimal(DRAM_ID);prn_string(" Trimmer Test Fail\n");
            prn_string("TRIM_INFO => Trim RD Error Count = ");prn_decimal(trim_rd_err_cnt);prn_string(" !!!\n");
            prn_string("---------------------------------------------------------------------------\n");

            //! 1st error data
            prn_string("TRIM_INFO => 1st ERR_CMD_NO = [");prn_decimal((GRP(TRM_BASE_GRP, 22)>>16));
            prn_string("], 1st ERR_DATA_NO = [");prn_decimal((GRP(TRM_BASE_GRP, 22)&0xFFFF));prn_string("]\n");
            prn_string("---------------------------------------------------------------------------\n");

            //! 1st error & expected data
            prn_string("TRIM_INFO => 1st ERR-DATA_L = 0x");prn_dword(GRP(TRM_BASE_GRP, 10));prn_string(" ");
            prn_string("TRIM_INFO => 1st ERR-DATA_H = 0x");prn_dword(GRP(TRM_BASE_GRP, 11));prn_string(" ...\n");

            prn_string("TRIM_INFO => 1st EXP-DATA_L = 0x");prn_dword(GRP(TRM_BASE_GRP, 16));prn_string(" ");
            prn_string("TRIM_INFO => 1st EXP-DATA_H = 0x");prn_dword(GRP(TRM_BASE_GRP, 17));prn_string(" ...\n");
            prn_string("---------------------------------------------------------------------------\n");

            //! last error & expected data
            prn_string("TRIM_INFO => last ERR-DATA_L = 0x");prn_dword(GRP(TRM_BASE_GRP, 12));prn_string(" ");
            prn_string("TRIM_INFO => last ERR-DATA_H = 0x");prn_dword(GRP(TRM_BASE_GRP, 13));prn_string(" ...\n");

            prn_string("TRIM_INFO => last EXP-DATA_L = 0x");prn_dword(GRP(TRM_BASE_GRP, 18));prn_string(" ");
            prn_string("TRIM_INFO => last EXP-DATA_H = 0x");prn_dword(GRP(TRM_BASE_GRP, 19));prn_string(" ...\n");
            prn_string("---------------------------------------------------------------------------\n");

            //! next error & expected data
            prn_string("TRIM_INFO => next ERR-DATA_L = 0x");prn_dword(GRP(TRM_BASE_GRP, 14));prn_string(" ");
            prn_string("TRIM_INFO => next ERR-DATA_H = 0x");prn_dword(GRP(TRM_BASE_GRP, 15));prn_string(" ...\n");

            prn_string("TRIM_INFO => next EXP-DATA_L = 0x");prn_dword(GRP(TRM_BASE_GRP, 20));prn_string(" ");
            prn_string("TRIM_INFO => next EXP-DATA_H = 0x");prn_dword(GRP(TRM_BASE_GRP, 21));prn_string(" ...\n");
            prn_string("---------------------------------------------------------------------------\n");
        #endif

        //! Disable Trimmer function
        //! TRIMMER_ONLY_EN = 1'b0;
        GRP(TRM_BASE_GRP, 1)= 0x0001ffe0 ;
        return 0;
    }

}//! end function => SDCTRL_TRIMMER_TEST_MODES


//! ***********************************************************************
//! * FUNC      : SDCTRL_TRIMMER_TEST
//! * PARAM     : DRAM_ID
//! * PURPOSE   : trigger SDC.trimmer 4 modes to do random DRAM access test
//! ***********************************************************************
int SDCTRL_TRIMMER_TEST_( unsigned int DRAM_ID, unsigned int start_addr, unsigned int TEST_DATA_LENGTH )
{
#if 0
    if(SDCTRL_TRIMMER_TEST_MODES_(0, DRAM_ID)==0)     {
        // TWITCH MODE
		prn_string("\n Trimmer-test Mode -0 : Fail !!!\n");
        return 0;
    }
	else if(SDCTRL_TRIMMER_TEST_MODES_(1, DRAM_ID)==0){
        // CMD CRITICAL MODE
		prn_string("\n Trimmer-test Mode -1 : Fail !!!\n");
        return 0;
    }
	else if(SDCTRL_TRIMMER_TEST_MODES_(2, DRAM_ID)==0){
        // DATA CRITICAL MODE
		prn_string("\n Trimmer-test Mode -2 : Fail !!!\n");
        return 0;
    }
	else if(SDCTRL_TRIMMER_TEST_MODES_(3, DRAM_ID)==0){
        // RANDOM MODE
		prn_string("\n Trimmer-test Mode -3 : Fail !!!\n");
        return 0;
    }
    else if(SDCTRL_TRIMMER_TEST_MODES_(4, DRAM_ID)==0){
        // RANDOM MODE
		prn_string("\n Trimmer-test Mode -4 : Fail !!!\n");
        return 0;
    }
        //prn_string("\n Trimmer-test Mode 0-4 : Pass !!!\n");
        return 1;
#else
	//CBDMA
	//#define CBDMA0
	//#define DEBUG
	//#define DEBUG_ERROR
	#ifdef DEBUG
		prn_string("\n[DRAM_SCAN] Trimmer-test by CBDMA !!!\n");
	#endif
	volatile unsigned int *dram = 0;
	unsigned int dram_value = 0;
	
	#ifdef CBDMA0
		volatile unsigned int *sram = (volatile unsigned int *)0x9e800040;//CBDMA0
		unsigned int DMA_GROUP = 26;
		unsigned int CBDMA_SRAM_SIZE = TEST_DATA_LENGTH;
	#else
		volatile unsigned int *sram = (volatile unsigned int *)0x9e820000;//CBDMA1
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
	unsigned int DATA_PATTERN[7] = {0XAAAAAAAA, 0X55555555, 0xAAAA5555, 0x5555AAAA, 0xAA57AA57, 0xFFDDFFDD, 0x55D755D7};
	unsigned int DATA_PATTERN_INDEX = 0;
	const unsigned int DATA_PATTERNS = sizeof(DATA_PATTERN)>>2;

	
	//step1. SET DRAM ADDRESS
	if( 0 == DRAM_ID )
	{
		dram = (volatile unsigned int *)(DRAM0_BASE_ADDR);
	}
	else
	{
		dram = (volatile unsigned int *)(DRAM1_BASE_ADDR);
	}

	
	for(loop_time=0; loop_time<MAX_LOOP_TIMES; loop_time++)
	{
		//SET DRAM START ADDDRESS
		CURRENT_PROCESS_ADDR = (start_addr>>2) + (loop_time * TEST_DATA_LENGTH);

		//step2. SET SRAM VALUE & CLEAR DRAM DATA
		//DEBUG
		#ifdef DEBUG
		prn_string("SET SRAM VALUE & CLEAR DRAM DATA:\n");
		#endif		
		for( i=0; i<TEST_DATA_LENGTH; i++ )
		{
			sram[i] = DATA_PATTERN[i%DATA_PATTERNS];
			dram[CURRENT_PROCESS_ADDR+i] = 0;
			#ifdef DEBUG
			prn_string("sram[");
			prn_dword(i);
			prn_string("]");
			prn_string("=");
			prn_dword(sram[i]);
			prn_string("\n");
			#endif
		}

		//DEBUG
		#ifdef DEBUG
		prn_string("before write DRAM Data by CBDMA:\n");
		for( i=0; i<TEST_DATA_LENGTH; i++ )
		{
			prn_string("dram[");
			prn_dword(CURRENT_PROCESS_ADDR+i);
			prn_string("]");
			prn_string("=");
			prn_dword(dram[CURRENT_PROCESS_ADDR+i]);
			prn_string("\n");
		}		
		#endif


		//step3. WRITE SRAM DATA TO DRAM
		GRP(DMA_GROUP, 2) = TEST_DATA_LENGTH * DATA_BYTE; //data length
		#ifdef CBDMA0
			GRP(DMA_GROUP, 3) = 0x00000040; //set data start addr from SRAM 
		#else
			GRP(DMA_GROUP, 3) = 0x00000000; //set data start addr from SRAM 
		#endif
		GRP(DMA_GROUP, 4) = &dram[CURRENT_PROCESS_ADDR]; //set data end addr to dram
		GRP(DMA_GROUP, 8) = 0x000000ff; //set sdram size
		GRP(DMA_GROUP, 1) = 0x00000101; //write data from internal sram to main memory
		
		while(watting_status_counter < WAIT_TIMEOUT)
		{
			temp = GRP(DMA_GROUP, 1) >> 8;
			if( 0 == temp )
			{
				break;
			}
			watting_status_counter++;
		}
		
		if(watting_status_counter >= WAIT_TIMEOUT)
		{
			#ifdef DEBUG
				prn_string("CBDMA write to dram failed !!!\n");		
			#endif		
			isPassed = 0;
			break;
		}


		//DEBUG
		#ifdef DEBUG
		prn_string("after write DRAM data by CBDMA:\n");
		for( i=0; i<TEST_DATA_LENGTH; i++ )
		{
			prn_string("dram[");
			prn_dword(CURRENT_PROCESS_ADDR+i);
			prn_string("]");
			prn_string("=");
			prn_dword(dram[CURRENT_PROCESS_ADDR+i]);
			prn_string("\n");
		}		
		#endif
		
		//step4. CLEAR SRAM DATA
		for( i=0; i<TEST_DATA_LENGTH; i++ )
		{
			sram[i] = 0;
		}
		//DEBUG
		#ifdef DEBUG
			prn_string("CLEAR SRAM DATA:\n");
			for( i=0; i<TEST_DATA_LENGTH; i++ )
			{
				prn_string("sram[");
				prn_dword(i);
				prn_string("]");
				prn_string("=");
				prn_dword(sram[i]);
				prn_string("\n");
			}
		#endif		


		//step5. READ DATA FROM DRAM TO SRAM
		watting_status_counter = 0;
		GRP(DMA_GROUP, 2) = TEST_DATA_LENGTH * DATA_BYTE; //data length
		GRP(DMA_GROUP, 3) = &dram[CURRENT_PROCESS_ADDR]; //set data start addr from DRAM 
		#ifdef CBDMA0
			GRP(DMA_GROUP, 4) = 0x00000040; //set data end addr to SRAM
		#else
			GRP(DMA_GROUP, 4) = 0x00000000; //set data end addr to SRAM
		#endif
		GRP(DMA_GROUP, 8) = 0x000000ff; //set sdram size
		GRP(DMA_GROUP, 1) = 0x00000102; //read data from main memory to internal sram
		while(watting_status_counter < WAIT_TIMEOUT)
		{
			temp = GRP(DMA_GROUP, 1) >> 8;
			if( 0 == temp )
			{
				break;
			}
			watting_status_counter++;
		}
		if(watting_status_counter >= WAIT_TIMEOUT)
		{
			#ifdef DEBUG
				prn_string("CBDMA read from dram failed !!!\n");		
			#endif		
			isPassed = 0;
			break;
		}

		//STEP6. VERIFY SRAM DATA
		#ifdef DEBUG
		prn_string("VERIFY SRAM DATA:\n");
		#endif		
		for( i=0 ; i<TEST_DATA_LENGTH ; i++ )
		{		
			#ifdef DEBUG
			prn_string("sram[");
			prn_dword(i);
			prn_string("]:");
			prn_dword(sram[i]);
			prn_string("\n");			
			#endif
			if( DATA_PATTERN[i%DATA_PATTERNS] != sram[i] )
			{
				#ifdef DEBUG_ERROR
					prn_string("VERIFY SRAM DATA FAILED!!\n");
					prn_string("CURRENT_PROCESS_ADDR=");
					prn_dword(CURRENT_PROCESS_ADDR);
					prn_string("\nsram[");
					prn_dword(i);
					prn_string("]:");
					prn_dword(sram[i]);
					prn_string(", correct will be ");
					prn_dword(DATA_PATTERN[i%DATA_PATTERNS]);
					prn_string("\n");
				#endif		
				isPassed = 0;
				break;
			}
		}

		//step7. CLEAR SRAM & DRAM DATA
		for( i=0; i<TEST_DATA_LENGTH; i++ )
		{
			sram[i] = 0;
			dram[CURRENT_PROCESS_ADDR+i] = 0;
		}		

		//step8. CHECK RESULT
		if( isPassed )
		{
			#ifdef DEBUG
				prn_string("TRIM BY CBDMA PASSED\n\n");
			#endif
		}
		else
		{
			#ifdef DEBUG
				prn_string("TRIM BY CBDMA FAILED\n\n");
			#endif
			break;
		}		
		
	}//end for
	

	//step9. CLEAR SRAM & DRAM DATA
	for( i=0; i<TEST_DATA_LENGTH; i++ )
	{
		sram[i] = 0;
		dram[CURRENT_PROCESS_ADDR+i] = 0;
	}
	return isPassed;  	
#endif	
}//! end function => SDCTRL_TRIMMER_TEST


//! ***********************************************************************
//! * FUNC      : DPCU_CMD_ISSUE_SW_CMD
//! * PARAM     : CMD, RANK,BANK,ADDR, DATA_MASK, DATA, TRIGGER
//! * PURPOSE   : using CMD ISSUE issue
//! ***********************************************************************
void DPCU_CMD_ISSUE_SW_CMD ( unsigned int DRAM_ID, unsigned int CMD, unsigned int RANK, unsigned int BANK, unsigned int ADDR,
    unsigned int SW_WRDATA_MASK, unsigned int SW_WRDATA1_HIGH, unsigned int SW_WRDATA1_LOW,
    unsigned int SW_WRDATA0_HIGH, unsigned int SW_WRDATA0_LOW, unsigned int CMD_TRIGGER)
{
    unsigned int        temp            ;
    unsigned int        loop_x          ;
    unsigned int        PHY_BASE_GRP=0  ;

    switch ( DRAM_ID ){
        case 0: {
            PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE      ;
            break;
        }
        case 1: {
            PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE      ;
            break;
        }
    }//! end switch

    temp                            = GRP((PHY_BASE_GRP+1),27) & 0xf0000000;
    //! fill cmd , RANK, BANK, ADDR infor
    GRP((PHY_BASE_GRP+1),27)        = temp | (CMD << 24) | (RANK << 20) |(BANK << 16) | (ADDR);
    //! fill WRdata mask
    GRP((PHY_BASE_GRP+2),21)        =  SW_WRDATA_MASK       & 0xff;
    GRP((PHY_BASE_GRP+3),21)        = (SW_WRDATA_MASK >> 8) & 0xff;
    //! fill WRDATA
    GRP((PHY_BASE_GRP+2),22)        = SW_WRDATA0_LOW;
    GRP((PHY_BASE_GRP+2),23)        = SW_WRDATA0_HIGH;
    GRP((PHY_BASE_GRP+3),22)        = SW_WRDATA1_LOW;
    GRP((PHY_BASE_GRP+3),23)        = SW_WRDATA1_HIGH;
    //! Commit the cmd setting to DPCU CMD CUE
    temp                            = GRP((PHY_BASE_GRP+1),10);
    wait_loop(10);
    GRP((PHY_BASE_GRP+1),10)        = temp | (1 << 8);     //! commit
    wait_loop(10);
    //! Trigger CMD if need
    temp                            = GRP((PHY_BASE_GRP+1),10);
    wait_loop(10);
    GRP((PHY_BASE_GRP+1),10)        = temp | (CMD_TRIGGER << 9);     //! Trigger
}  //! End    DPCU_CMD_ISSUE_SW_CMD

//! ***********************************************************************
//! * FUNC      : dram_booting_flow
//! * PARAM     : DRAM_ID
//! * PURPOSE   : to do the following sequences
//! *           : (1). DDR_APHY initial sequence (CTCAL->SSCPLL->PZQ)
//! ***********************************************************************
int dram_booting_flow( unsigned int DRAM_ID )
{
    unsigned int        SDC_BASE_GRP=0  ,
                        PHY_BASE_GRP=0  ;
    prn_string( ">>> enter dram_booting_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
    //! -------------------------------------------------------
    //! 0. SDCTRL / DDR_PHY RGST GRP selection
    //! -------------------------------------------------------
    switch ( DRAM_ID ) {
        case 0: {
            SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE      ;
            PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE      ;
            break;
        }
        case 1: {
            SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE      ;
            PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE      ;
            break;
        }
    }//! end switch

    //! -------------------------------------------------------
    //! 0. Set DRAM-0 size in CBUS( CBUS to MBUS Bridge address decode/mapping ) , soft-configure-reg 13
    // DRAM-0 size : Group-1, reg-13 : bit[22:21] = 0 => 128Mb , 1=>256MB, 2=>512MB, 3=>1GB
    // DRAM-1 size : Group-1, reg-13 : bit[24:23] = 0 => 128Mb , 1=>256MB, 2=>512MB, 3=>1GB
    //! -------------------------------------------------------
    GRP((1), 13)    =   GRP((1), 13) &  ( ~( 15 << 21) );
    GRP((1), 13)    =   GRP((1), 13) | ( ( MO_SDRAM_A_SIZE << 21) ) | ( ( MO_SDRAM_B_SIZE << 23) );

    //! -------------------------------------------------------
    //! 1. DPCU_APHY_INIT setting => a001
    //! -------------------------------------------------------
#ifdef SDRAM_FPGA
    //! There are no APHY circuit in FPGA platform, so bypass this flow
#else
//++
    if(resume_function) {
        rgst_value = GRP(8, 0)  ;
        prn_string("\tGRP(8, 0) = "); prn_dword(GRP(8, 0));
        if(DRAM_ID == 0) {
            GRP(8, 0) = (rgst_value | ((1 << 8)) );
        } else if (DRAM_ID == 1) {
            GRP(8, 0) = (rgst_value | ((1 << 10)) );
        }
        prn_string("\tGRP(8, 0) = "); prn_dword(GRP(8, 0));
        prn_string("\t+++ System IO ISO function for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
    }
//++
    do_system_reset_flow( DRAM_ID );
    dbg_stamp(0xa000);
    

    GRP((PHY_BASE_GRP+0), 0)    =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_DPCU);
    //! set MPLL_DIV to operation freq.
    GRP((PHY_BASE_GRP+0),12)    =   DPCU_MPLL_CFG1 ;
    //! set MPLL_DIV to operation freq.
    GRP((PHY_BASE_GRP+0), 3)    =   DPCU_INIT_TIMMER ;
    //! Set DDRIO CFG
    GRP((PHY_BASE_GRP+0), 21)   =   DPCU_DDRIO_CFG3 ;
    //! set PZQ to internal mode for Q571
    #ifdef USING_INTERNAL_PZQ_CAL
        prn_string("\tDPCU initial : Using Internal PZQ!!\n");
        GRP((PHY_BASE_GRP+0),18)    =   DPCU_PZQ_CFG0 | PZQ_REGI_ZQ_INTR(n_PZQ_ZQ_INTR_EN);
    #else
        prn_string("\tDPCU initial : Using External PZQ!!\n");
        GRP((PHY_BASE_GRP+0),18)    =   DPCU_PZQ_CFG0;
    #endif

    if(resume_function) {
        //! setting PZQ CFG1
        GRP((PHY_BASE_GRP+0),19)    =   DPCU_PZQ_CFG1;
        //! setting AI CFG
        GRP((PHY_BASE_GRP+0), 1)    =   DPCU_AI_CFG0;
        //! enable APHY_INIT start
        GRP((PHY_BASE_GRP+0), 1)    =   DPCU_AI_CFG0 | AI_INIT_START(n_AI_INIT_START_EN) ;
    } else {
        //! setting PZQ CFG1
        GRP((PHY_BASE_GRP+0),19)    =   DPCU_PZQ_CFG1;
        //! setting AI CFG
        GRP((PHY_BASE_GRP+0), 1)    =   DPCU_AI_CFG0_SELECT1;
        //! enable APHY_INIT start
        GRP((PHY_BASE_GRP+0), 1)    =   DPCU_AI_CFG0_SELECT1 | AI_INIT_START(n_AI_INIT_START_EN) ;
    }

    //! wait aphy init done
    wait_flag   =   0   ;
    do{
        wait_flag   =   GRP((PHY_BASE_GRP+0), 2) & 0x00000001 ;
    }while( (wait_flag==0) );

    // restore PZQ
    if(resume_function) {
        unsigned int tmp0 = 0;
        unsigned int tmp1 = 0;
        unsigned int tmp2 = 0;
        rgst_value = GRP(PHY_BASE_GRP, 19);
        prn_string("\tPZQ auto setting = "); prn_dword(rgst_value);
        rgst_value &= ~(0x1FFFFE00);

        /* PZQ register mapping, restore saved 2 x (4+16) bits from iop_data7-10
           (8, 15/17) bit 0     -> (PHY, 19) bit 9
                      bit 1     -> (PHY, 19) bit 14
                      bit 2     -> (PHY, 19) bit 19
                      bit 3     -> (PHY, 19) bit 24
           (8, 16/18) bit 3-0   -> (PHY, 19) bit 13-10
                      bit 7-4   -> (PHY, 19) bit 18-15
                      bit 11-8  -> (PHY, 19) bit 23-20
                      bit 15-12 -> (PHY, 19) bit 28-25 */
        tmp0 = GRP(8, 8);
        if(DRAM_ID == 0) {
            tmp1 = GRP(8, 15);
            tmp2 = GRP(8, 16);
        } else if(DRAM_ID == 1) {
            tmp1 = GRP(8, 17);
            tmp2 = GRP(8, 18);
        }
        //prn_string("tmp1 = "); prn_dword(tmp1); prn_string("tmp2 = "); prn_dword(tmp2); prn_string("\n");
        rgst_value |= ((tmp1 & 0x01) << 9) | ((tmp1 & 0x02) << (14-1)) |
                          ((tmp1 & 0x04) << (19-2)) | ((tmp1 & 0x08) << (24-3)) |
                      ((tmp2 & 0x0f) << 10) | ((tmp2 & 0x00f0) << (15-4)) |
                          ((tmp2 & 0x0f00) << (20-8)) | ((tmp2 & 0xf000) << (25-12)) |
                      0x100; // bit8 is enable bit
        prn_string("\tPZQ restore setting = "); prn_dword(rgst_value);
        GRP(PHY_BASE_GRP, 19) = rgst_value;
    }

    rgst_value = (GRP((PHY_BASE_GRP+0), 2) >> 8)  & 0xf;

    if(resume_function) { // Terry's flow will end here, for resume first
        if(rgst_value != 0) {
            prn_string( "<<< leave dram_booting_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
            return 0 ;
        }
    } else { // server's flow will do more, for normal boot
        aphy_select1_value = rgst_value;
        //! Disable DDR IO PAD Retention flag
        rgst_value = GRP(8, 0)  ;
        if((DRAM_ID == 0) && ( (rgst_value & (1 << 8)) == 0x100)) {
            GRP(8, 0) = (rgst_value & (~(1 << 8)) );
            prn_string("\tDRAM 0 System resume function!!\n");
            resume_function = 1;
        } else if ((DRAM_ID == 1) && ((rgst_value & (1 << 10)) == 0x400)) {
            GRP(8, 0) = (rgst_value & (~(1 << 10)) );
            prn_string("\tDRAM 1 System resume function!!\n");
            resume_function = 1 ;
        }
        //! setting AI CFG
        GRP((PHY_BASE_GRP+0), 1)    =   DPCU_AI_CFG0_SELECT2;
        //! enable APHY_INIT start
        GRP((PHY_BASE_GRP+0), 1)    =   DPCU_AI_CFG0_SELECT2 | AI_INIT_START(n_AI_INIT_START_EN) ;
        //! wait aphy init done
        wait_flag   =   0   ;
        do {
            wait_flag   =   GRP((PHY_BASE_GRP+0), 2) & 0x00000001 ;
        } while( (wait_flag==0) );
        rgst_value = (GRP((PHY_BASE_GRP+0), 2) >> 8)  & 0xf;

        aphy_select2_value = rgst_value;
        aphy_select_value = (aphy_select1_value | aphy_select2_value);

        if(rgst_value != 0) {
            prn_string( "<<< leave dram_booting_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
            return 0 ;
        }
    }
#endif
    prn_string( "<<< leave dram_booting_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
    return 1;
} // end of dram_booting_flow

//! ***********************************************************************
//! * FUNC      : dram_training_flow
//! * PARAM     : DRAM_ID
//! * PURPOSE   : to do the following sequences
//! *           : (1). DPCU DRAM inital setting
//! *           : (2). SDCTRL RGST setting
//! *           : (3). DRAM initial setting by SDCTRL
//! *           : (5). DDR_DPHY+APHY data training
//! ***********************************************************************
int dram_training_flow( unsigned int DRAM_ID )
{
    unsigned int        SDC_BASE_GRP=0  ,
                        PHY_BASE_GRP=0  ,
                        waiting_status_counter=0;
    unsigned int        temp_1, temp_2;
    int i = 0;

    prn_string( ">>> enter dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
    //! -------------------------------------------------------
    //! 0. SDCTRL / DDR_PHY RGST GRP selection
    //! -------------------------------------------------------
    switch ( DRAM_ID ){
        case 0: {
        //prn_string("DRAM_0 Training Setting ...\n");
            SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE      ;
            PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE      ;
            break;
        }
        case 1: {
        //prn_string("DRAM_1 Training Setting ...\n");
            SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE      ;
            PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE      ;
            break;
        }
    }//! end switch

#ifdef SDRAM_FPGA
    //! There are no APHY circuit in FPGA platform, so bypass this flow
#else
    //! mazar test print PZQ impedence
    //rgst_value = (GRP((PHY_BASE_GRP+0), 10) >> 8)  & 0xffff;
    //prn_string("PZQ IO impedance=");prn_dword(rgst_value);prn_string("\n");
    //! end

    if(!resume_function) {
        prn_string("\tGRP(PHY_BASE_GRP, 0) = "); prn_dword(GRP(PHY_BASE_GRP,0)); prn_string("\n");
        prn_string("\tGRP(PHY_BASE_GRP+1, 10) = "); prn_dword(GRP((PHY_BASE_GRP+1),10)); prn_string("\n");
        if (GRP((PHY_BASE_GRP),0) == 0x0 ) {
            prn_string( "<<< 1 leave dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
            return WAIT_FLAG_FAIL;
        }
        if( GRP((PHY_BASE_GRP+1),10) == 0x0  ) {
            prn_string( "<<< 2 leave dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
            return WAIT_FLAG_FAIL;
        }
    }

    if ( ckobd_training_flag == 1 ) {
        // 20140812: Add CB0DB+4 to solve WL (DQS/CK wire path issue) and RDQSG change phase issue
        // #define DPCU_CMD_BDD_SETTING            (DPCU_ACK0BD<<16) | (DPCU_AC0BD << 8) | (DPCU_CK0BD << 0)
        rgst_value = (DPCU_CK0BD+(ckobd_re_training_number*4));
        prn_string("\tSet CK0BD value to : ");prn_decimal(rgst_value);prn_string("\n");
        GRP((PHY_BASE_GRP+0), 17) = ( ( (DPCU_ACK0BD+(ckobd_re_training_number*4)) <<16) | ( (DPCU_AC0BD+(ckobd_re_training_number*4)) << 8) | ( (DPCU_CK0BD+(ckobd_re_training_number*4)) << 0) );
    } else {
        //!mazar :  Add DPCU APHY CK,AC,ACK BDD Delay for socket and direct IC can use the same SDCTRL RGST setting 2014.0719
        // rgst_value = (DPCU_CK0BD);
        // prn_string("\n\t Set new CK0BD value to Ellison : ");prn_decimal(rgst_value);prn_string(" ------------------\n");
        GRP((PHY_BASE_GRP+0), 17)    =   DPCU_CMD_BDD_SETTING;
    }

    dbg_stamp(0xa001) ;

    //! release SerDes' reset
    GRP((PHY_BASE_GRP+0), 8)    =   (GRP((PHY_BASE_GRP+0), 8) & 0xfffffff8) | 0x00000007 ;

    //! clean CMD_ISSUE MRS field
    GRP((PHY_BASE_GRP+1),25)    =   0x0 ;
    GRP((PHY_BASE_GRP+1),26)    =   0x0 ;

    //! sync. MRS setting to CMD_ISSUE
    //dbg_stamp(CI_MRS_0_VAL_SET);
    //dbg_stamp(CI_MRS_1_VAL_SET);
    //dbg_stamp(CI_MRS_2_VAL_SET);
    //dbg_stamp(CI_MRS_3_VAL_SET);

    //GRP((PHY_BASE_GRP+1),25)  =   (CI_MRS_1_VAL_SET<<16) | (CI_MRS_0_VAL_SET << 0) ;
    GRP((PHY_BASE_GRP+1),25)    =   CI_MRS_1_VAL_SET ;
    GRP((PHY_BASE_GRP+1),25)    =   (GRP((PHY_BASE_GRP+1),25)<<16) | (CI_MRS_0_VAL_SET << 0) ;
    GRP((PHY_BASE_GRP+1),26)    =   (CI_MRS_3_VAL_SET<<16) | (CI_MRS_2_VAL_SET << 0) ;

    #ifdef CSIM_ASIC
    dbg_stamp(0xa002) ;
    //! csim simulation only, for YES_QUICK_SIM and DPCU_FAST (speedup csim flow)
    //! RGST_CI_RST_B_HIGH & RGST_CKE_HIGH
    //GRP((PHY_BASE_GRP+1),15)    =   0x00F000FF      ;
    //! RGST_CI_RST_B_LOW
    //GRP((PHY_BASE_GRP+1),16)    =   0x00000FF0      ;
    #else
    //! IC - Real initial flow (ASIC & FPGA)
    #endif

    dbg_stamp(0xa003) ;

    if(resume_function) {
        //! trigger CMD_ISSUE DRAM_INIT sequence
        GRP((PHY_BASE_GRP+1),10)    =   (GRP((PHY_BASE_GRP+1),10) & 0xffffffef) | 0x00000010 ;
        //! wait CMD_ISSUE flag
        wait_flag   =   0   ;
        do{
            wait_flag   =   (GRP((PHY_BASE_GRP+1), 10) >> 18) & 0x1 ;
        }while( (wait_flag==0) );
    } else {
        //! trigger CMD_ISSUE DRAM_INIT sequence
        if(GRP((PHY_BASE_GRP+1),10) == 0x0) {
            prn_string( "<<< 3 leave dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
            return WAIT_FLAG_FAIL;
        }
        prn_string("\tbefore GRP((PHY_BASE_GRP+1),10) = ");prn_dword(GRP((PHY_BASE_GRP+1),10));prn_string("\n");
        GRP((PHY_BASE_GRP+1),10)    =   (GRP((PHY_BASE_GRP+1),10) & 0xffffffef) | 0x00000010;//0x00021011;

        waiting_status_counter = 0;
    }

    //! wait CMD_ISSUE flag
#endif//! endif - SDRAM_FPGA

    dbg_stamp(0xa004) ;

    //! -------------------------------------------------------
    //! 2. SDCTRL RGST setting => a002
    //! -------------------------------------------------------
#ifdef DISABLE_L3_CACHE
    GRP(24, 0)  =   GRP(24, 0)  &  ( ~(3<<12) ) ;
#endif
#ifdef DISABLE_L3_ACCESS_DRAM_TO_MBUS
    GRP((SDC_BASE_GRP+7), 5)    =   ( GRP((SDC_BASE_GRP+7), 5) & ~(1<<1)) ;
#endif
//#ifdef L3C_SIZE_64KB
//    GRP((SDC_BASE_GRP+7), 1)    =   0x0010                      ;
//#endif
    GRP((SDC_BASE_GRP-1), 1)    =   MCPP_BKLEN_CFG_VAL          ;
    GRP((SDC_BASE_GRP-1), 7)    =   MCPP_LIFE_VAL               ;
    GRP((SDC_BASE_GRP+0), 3)    =   DATA_NO_LIM_CFG_VAL         ;  // ASIC ONLY
    GRP((SDC_BASE_GRP+0), 8)    =   AREF_REG_DIS_VAL            ;
    GRP((SDC_BASE_GRP+0), 9)    =   AREF_INTVAL(nAREF_INTVAL)   ;  // ASIC DIFF

    if( DRAM_ID==1 ){
        GRP((SDC_BASE_GRP+0), 5)    =   SDRAM1_SIZE_TYPE_VAL         ;  // ASIC DIFF
    }else{
        GRP((SDC_BASE_GRP+0), 5)    =   SDRAM0_SIZE_TYPE_VAL         ;  // ASIC DIFF
    }
    GRP((SDC_BASE_GRP+0), 6)    =   SD_SYS_MISC                 ;
    GRP((SDC_BASE_GRP+0), 7)    =   0                           ;
    if( DRAM_ID==1 ){
        GRP((SDC_BASE_GRP+0),11)    =   SCAN_SD1_ACC_LATENCY        ;
    }else{
        GRP((SDC_BASE_GRP+0),11)    =   SCAN_SD0_ACC_LATENCY        ;
    }
    GRP((SDC_BASE_GRP+0),12)    =   SD_PAR_INTERVAL_VAL         ;
    GRP((SDC_BASE_GRP+0),13)    =   SD_PAR_TIMING0_VAL          ;
    GRP((SDC_BASE_GRP+0),14)    =   SD_PAR_TIMING1_VAL          ; // ASIC DIFF TRFC
    GRP((SDC_BASE_GRP+0),15)    =   SD_PAR_TIMING2_VAL          ;
    GRP((SDC_BASE_GRP+0),16)    =   SD_PAR_TIMING3_VAL          ;
    GRP((SDC_BASE_GRP+0),19)    =   ODT_SIGNAL_TIMING_VAL       ;
    GRP((SDC_BASE_GRP+0),27)    =   ZQCL_CFG_VAL                ;
    GRP((SDC_BASE_GRP+0),28)    =   ZQCS_CFG_VAL                ;
    GRP((SDC_BASE_GRP+0),29)    =   SCPP_CFG_VAL                ;
    //! -------------------------------------------------------
    //! 3. SDCTRL-0 initial task sequence
    //! -------------------------------------------------------
    //! switch DFI path to SDCTRL
    GRP((PHY_BASE_GRP+0), 0)    =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL) ;
    //! assert SDCTRL's DDR3_RST & DDR3_CKE register
    GRP((SDC_BASE_GRP+0),31)    =   DDR3_RST_CKE_DISABLE        ;
    wait_loop (10)   ;  //! wait for DRAM RST

    //! release SDCTRL's DDR3_RST & DDR3_CKE register
    GRP((SDC_BASE_GRP+0),31)    =   DDR3_RST_EN                 ;
    wait_loop (5)   ;   //! wait for RST high

    GRP((SDC_BASE_GRP+0),31)    =   DDR3_RST_CKE_EN             ;
    wait_loop (5)   ;   //! wait for CKE high

    //! DRAM MRS SETTING
    //! DDR2: PREA -> MRS2 -> MRS3 -> MRS1 ->
    //!       MRS0(DLL_RESET0) -> MRS0(DLL_RESET1) -> MRS1(OCD DFLT) -> MRS1(OCD EXIT)
    //! DDR3: MRS2 -> MRS3 -> MRS1 -> MRS0

#ifdef MPEG_DRAM_TYPE_DDR2
    //! PREA
    GRP((SDC_BASE_GRP+0), 8)    =   PREA_CMD_TRIG_VAL           ;
#endif

    //! MRS_MODE2
    GRP((SDC_BASE_GRP+0),17)    =   MRS_MODE2_VAL_SET           ;
    GRP((SDC_BASE_GRP+0), 8)    =   MRS_CMD_TRIG_VAL            ;
    //! MRS_MODE3
    GRP((SDC_BASE_GRP+0),17)    =   MRS_MODE3_VAL_SET           ;
    GRP((SDC_BASE_GRP+0), 8)    =   MRS_CMD_TRIG_VAL            ;
    //! MRS_MODE1
    //! OCD Exit & DLL_ENABLE
    GRP((SDC_BASE_GRP+0),17)    =   MRS_MODE1_VAL_SET           ;
    GRP((SDC_BASE_GRP+0), 8)    =   MRS_CMD_TRIG_VAL            ;

    //! MRS_MODE0
    //! SDRAM DLL Reset
    GRP((SDC_BASE_GRP+0),17)    =   MRS_MODE0_VAL_SET           ;
    GRP((SDC_BASE_GRP+0), 8)    =   MRS_CMD_TRIG_VAL            ;

    #ifdef MPEG_DRAM_TYPE_DDR2
    GRP((SDC_BASE_GRP+0), 8)    =   PREA_CMD_TRIG_VAL           ;
    GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
    GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
    //! Write_recovery_6T , SDRAM DLL Without Reset
    GRP((SDC_BASE_GRP+0),17)    =   MRS_MODE0_VAL_SET_1         ;
    GRP((SDC_BASE_GRP+0), 8)    =   MRS_CMD_TRIG_VAL            ;
    //! OCD Default
    GRP((SDC_BASE_GRP+0),17)    =   MRS_MODE1_VAL_SET_1         ;
    GRP((SDC_BASE_GRP+0), 8)    =   MRS_CMD_TRIG_VAL            ;
    //! OCD Exit
    GRP((SDC_BASE_GRP+0),17)    =   MRS_MODE1_VAL_SET           ;
    GRP((SDC_BASE_GRP+0), 8)    =   MRS_CMD_TRIG_VAL            ;
    #endif
    #ifdef DRAM_ZQ_CFG
    //! ZQCL setting
    GRP((SDC_BASE_GRP+0), 8)    =   ZQCL_CMD_TRIG_VAL           ;
    #endif

    dbg_stamp(0xa002) ;

/* removed, move before DT_GO
    if(resume_function) {
        int i;
        //! Disable DDR IO PAD Retention flag
        prn_string("\tGRP(8, 0) = "); prn_dword(GRP(8, 0)); prn_string("\n");
        rgst_value = GRP(8, 0)  ;
        if((DRAM_ID == 0) && ( (rgst_value & (1 << 8)) == 0x100)) {
            GRP(8, 0) = (rgst_value & (~(1 << 8)) );
            GRP (33, 10)=0x00006013 ;

            for(i = 0; i < 200; i++) {
                asm volatile ("nop");
            }

            GRP (33, 8 )=0x00050683 ;
            GRP (33, 18) = 0x00000002 ;//initial done
            prn_string("\t+++ DRAM 0 System resume function!!\n");
        } else if ((DRAM_ID == 1) && ((rgst_value & (1 << 10)) == 0x400)) {
            GRP(8, 0) = (rgst_value & (~(1 << 10)) );
            GRP (46, 10)=0x00006013 ;

            for(i = 0; i < 200; i++) {
                asm volatile ("nop");
            }

            GRP (46, 8 )=0x00050683 ;//issue PREA
            GRP (46, 18) = 0x00000002 ;//initial done
            prn_string("\t+++ DRAM 1 System resume function!!\n");
        }
        prn_string("\tGRP(8, 0) = "); prn_dword(GRP(8, 0)); prn_string("\n");
    }
*/

    //! -------------------------------------------------------
    //! 1. DPCU data training => a003
    //! -------------------------------------------------------
    
    //! Setting RGST_DT_WREYE_MIN_RANGE
    //! WR deskew issue
    //!    GRP((PHY_BASE_GRP+1),6)    =   (GRP((PHY_BASE_GRP+1),6) & 0xffffff00) | 0x0000000a;        
    //!    prn_string("\n\t Set G37 ADDR6 value to : ");prn_dword(GRP((PHY_BASE_GRP+1),6));prn_string(" ------------------\n");
#ifdef SDRAM_FPGA
    //! bypass data training at FPGA setting
#else
    #ifdef MPEG_DRAM_8BIT
    GRP((PHY_BASE_GRP+3), 0)    =   0x00000000                  ; //! disable 1 serdes
    #endif
    //! Set DPCU Deskew manual pattern and access address range
    //! 1. Pattern type select; G37 addr 1 bit[29:28]
    //! 2. Pattern bank select; G37 addr 1 bit[25:24]
    //! 3. Pattern address select; G37 addr 2(bank, rol,col)
    //! 4. Pattern select; G37 addr 3(burst3_0), G37 addr 4(burst7_4)
    //! Pattern 1
        //! 1.&2. Pattern type and bank select
        GRP((PHY_BASE_GRP+1), 1) = DT_PAT_TYPE_SEL(n_DESKEW_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_0);
        //! 3. Address select
        GRP((PHY_BASE_GRP+1), 2) = DT_DRAM0_ADDRESS;
        //! 4. Patterns select
        GRP((PHY_BASE_GRP+1), 3) = DT_PAT0_DESKEW_3_0;
        GRP((PHY_BASE_GRP+1), 4) = DT_PAT0_DESKEW_7_4;
    //! Pattern 2
        //! 1.&2. Pattern type and bank select
        GRP((PHY_BASE_GRP+1), 1) = DT_PAT_TYPE_SEL(n_DESKEW_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_1);
        //! 3. Address select
        GRP((PHY_BASE_GRP+1), 2) = DT_DRAM0_ADDRESS;
        //! 4. Patterns select
        GRP((PHY_BASE_GRP+1), 3) = DT_PAT1_DESKEW_3_0;
        GRP((PHY_BASE_GRP+1), 4) = DT_PAT1_DESKEW_7_4;
    //! Pattern 3
        //! 1.&2. Pattern type and bank select
        GRP((PHY_BASE_GRP+1), 1) = DT_PAT_TYPE_SEL(n_DESKEW_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_2);
        //! 3. Address select
        GRP((PHY_BASE_GRP+1), 2) = DT_DRAM0_ADDRESS;
        //! 4. Patterns select
        GRP((PHY_BASE_GRP+1), 3) = DT_PAT2_DESKEW_3_0;
        GRP((PHY_BASE_GRP+1), 4) = DT_PAT2_DESKEW_7_4;
    //! Pattern 4
        //! 1.&2. Pattern type and bank select
        GRP((PHY_BASE_GRP+1), 1) = DT_PAT_TYPE_SEL(n_DESKEW_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_3);
        //! 3. Address select
        GRP((PHY_BASE_GRP+1), 2) = DT_DRAM0_ADDRESS;
        //! 4. Patterns select
        GRP((PHY_BASE_GRP+1), 3) = DT_PAT3_DESKEW_3_0;
        GRP((PHY_BASE_GRP+1), 4) = DT_PAT3_DESKEW_7_4;

    //! EYE manual pattern
    //! 1. Pattern type select; G37 addr 1 bit[29:28]
    //! 2. Pattern bank select; G37 addr 1 bit[25:24]
    //! 3. Pattern address select; G37 addr 2(bank, rol,col)
    //! 4. Pattern select; G37 addr 3(burst3_0), G37 addr 4(burst7_4)
    //! Pattern 1
        //! 1.&2. Pattern type and bank select
        GRP((PHY_BASE_GRP+1), 1) = DT_PAT_TYPE_SEL(n_EYE_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_0);
        //! 3. Address select
        GRP((PHY_BASE_GRP+1), 2) = DT_DRAM0_ADDRESS;
        //! 4. Patterns select
        GRP((PHY_BASE_GRP+1), 3) = DT_PAT0_EYE_3_0;
        GRP((PHY_BASE_GRP+1), 4) = DT_PAT0_EYE_7_4;
    //! Pattern 2
        //! 1.&2. Pattern type and bank select
        GRP((PHY_BASE_GRP+1), 1) = DT_PAT_TYPE_SEL(n_EYE_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_1);
        //! 3. Address select
        GRP((PHY_BASE_GRP+1), 2) = DT_DRAM0_ADDRESS;
        //! 4. Patterns select
        GRP((PHY_BASE_GRP+1), 3) = DT_PAT1_EYE_3_0;
        GRP((PHY_BASE_GRP+1), 4) = DT_PAT1_EYE_7_4;
    //! Pattern 3
        //! 1.&2. Pattern type and bank select
        GRP((PHY_BASE_GRP+1), 1) = DT_PAT_TYPE_SEL(n_EYE_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_2);
        //! 3. Address select
        GRP((PHY_BASE_GRP+1), 2) = DT_DRAM0_ADDRESS;
        //! 4. Patterns select
        GRP((PHY_BASE_GRP+1), 3) = DT_PAT2_EYE_3_0;
        GRP((PHY_BASE_GRP+1), 4) = DT_PAT2_EYE_7_4;
    //! Pattern 4
        //! 1.&2. Pattern type and bank select
        GRP((PHY_BASE_GRP+1), 1) = DT_PAT_TYPE_SEL(n_EYE_TYPE) | DT_PAT_BANK_SLE(n_BANK_SEL_3);
        //! 3. Address select
        GRP((PHY_BASE_GRP+1), 2) = DT_DRAM0_ADDRESS;
        //! 4. Patterns select
        GRP((PHY_BASE_GRP+1), 3) = DT_PAT3_EYE_3_0;
        GRP((PHY_BASE_GRP+1), 4) = DT_PAT3_EYE_7_4;

            //! step-1: SWITCH PATH to DPCU
            if(resume_function) {
                // self refresh move this switch later
                //GRP((PHY_BASE_GRP+0),  0)   =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_DPCU)  ;
            }else {
                GRP((PHY_BASE_GRP+0),  0)   =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_DPCU)  ;
            }
            #ifdef DPCU_TRAINING_ODT_ON  //! open IC_ODT and DRAM_ODT
            //! step-0: SWITCH PATH to DPCU
//--        GRP((PHY_BASE_GRP+0),  0)   =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_DPCU)  ; // redundant
            //! step a switch to CMD ISSUE SW mux
            rgst_value                     =   GRP((PHY_BASE_GRP+1),  10);
            wait_loop(10);

            GRP((PHY_BASE_GRP+1),  10)     =   rgst_value | (1<<1) ;
            //prn_string("(GRP((PHY_BASE_GRP+1),  10)=");prn_dword((GRP((PHY_BASE_GRP+1),  10)));prn_string("!!\n");
            //prn_string("(GRP((PHY_BASE_GRP+1),  11)=");prn_dword((GRP((PHY_BASE_GRP+1),  11)));prn_string("!!\n");

            //! step b issue CMD ISSUE IC ODT on command
            //! DRAM_ID, CMD , RANK, BANK, ADDR, DM, DATA1_HIGH, DATA1_LOW, DATA0_HIGH, DATA0_LOW, TRIGGER
            DPCU_CMD_ISSUE_SW_CMD(DRAM_ID, 0x8, 0x0, 0x0, 0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);  // COMMIT and TIGGER // ODT ON SPECIAL COMMAND
            //! step b issue CMD ISSUE ODT on command
            //! DRAM_ID, CMD , RANK, BANK, ADDR, DM, DATA1_HIGH, DATA1_LOW, DATA0_HIGH, DATA0_LOW, TRIGGER
            DPCU_CMD_ISSUE_SW_CMD(DRAM_ID, 0x8, 0x0, 0x0, 0x5, 0x0, 0x0, 0x0, 0x0, 0x0, 0x1);  // COMMIT and TIGGER // ODT ON SPECIAL COMMAND
            wait_flag = 0   ;
            do {
                wait_loop(1000);
                wait_flag   =   (GRP((PHY_BASE_GRP+1),  10) >> 17) & 0x0001    ;
            }while ( (wait_flag==0) );
            //! step c Switch back to HW
            rgst_value                     =   GRP((PHY_BASE_GRP+1),  10);
            GRP((PHY_BASE_GRP+1),  10)     =   rgst_value & (0xfffffffd) ;
            #endif
            #ifdef  DPCU_TRAINING_MANUAL_PAT
                    prn_string("\tDPCU Training by Manual pattern!!\n");
                    rgst_value                =    GRP((PHY_BASE_GRP+1),  1) & 0xffffff;  // extract [23:0]
                    //B0
                    GRP((PHY_BASE_GRP+1),  1) =    rgst_value | (0 << 24) | (3 << 28);  //BANK0, WR/RD EYE setting
                    GRP((PHY_BASE_GRP+1),  3) =    0x02488024;
                    GRP((PHY_BASE_GRP+1),  4) =    0x42088420;
                    //B1
                    GRP((PHY_BASE_GRP+1),  1) =    rgst_value | (1 << 24) | (3 << 28);  //BANK1, WR/RD EYE setting
                    GRP((PHY_BASE_GRP+1),  3) =    0x28040482;
                    GRP((PHY_BASE_GRP+1),  4) =    0x28404082;
                    //B2
                    GRP((PHY_BASE_GRP+1),  1) =    rgst_value | (2 << 24) | (3 << 28);  //BANK2, WR/RD EYE setting
                    GRP((PHY_BASE_GRP+1),  3) =    0x84024208;
                    GRP((PHY_BASE_GRP+1),  4) =    0x80242048;
                    //B3
                    GRP((PHY_BASE_GRP+1),  1) =    rgst_value | (3 << 24) | (3 << 28);  //BANK2, WR/RD EYE setting
                    GRP((PHY_BASE_GRP+1),  3) =    0x40284802;
                    GRP((PHY_BASE_GRP+1),  4) =    0x20848204;
            #endif

            /* by default,
             * DPCU_TRAINING_READ_MPR is not entered
             * DPCU_TRAINING_RG_LINEAR is entered, but we do not want to enter it while resume
             */
            #ifdef  DPCU_TRAINING_READ_MPR
               prn_string("\tDPCU Training by MPR mode!!\n");
               rgst_value                =    GRP((PHY_BASE_GRP+1),  5);
               GRP((PHY_BASE_GRP+1),  5) =    rgst_value | DT_RD_MPR(n_DT_RD_MPR_EN);
            #endif
            //! step-2: trigger 1st data training start
            #ifdef  DPCU_TRAINING_RG_LINEAR
            //if(resume_function) {
            //} else {
                prn_string("\tDPCU RG Training by Linear Search\n");
                rgst_value                  =   GRP((PHY_BASE_GRP+1),  5)   ;
                GRP((PHY_BASE_GRP+1),  5)   =   rgst_value | DT_RG_LINEAR(n_DT_RG_LINEAR_EN);
            //}
            #endif
//++ add self refresh exit before DT_GO
            // refresh period
            rgst_value = GRP((PHY_BASE_GRP+1), 1);
            rgst_value &= (~(0x7fff) << 8);
            rgst_value |= 0xA20 << 8;
            GRP((PHY_BASE_GRP+1), 1) = rgst_value;
            rgst_value = GRP((PHY_BASE_GRP+1), 1);
            prn_string("\trefresh period = "); prn_dword(rgst_value);
            // exit Sref
            if(resume_function) {
                //! Disable DDR IO PAD Retention flag
                //prn_string("\tGRP(8, 0) = "); prn_dword(GRP(8, 0)); prn_string("\n");
                rgst_value = GRP(8, 0)  ;
                if((DRAM_ID == 0) && ( (rgst_value & (1 << 8)) == 0x100)) {
                    GRP(8, 0) = (rgst_value & (~(1 << 8)) );
                } else if ((DRAM_ID == 1) && ((rgst_value & (1 << 10)) == 0x400)) {
                    GRP(8, 0) = (rgst_value & (~(1 << 10)) );
                }
                // exit self refresh
                GRP (SDC_BASE_GRP, 10)=0x00006013 ;
                for(i = 0; i < 200; i++) {
                    asm volatile ("nop");
                }

                // trigger fresh before 0x00050683
                GRP((SDC_BASE_GRP+0), 8)    =   MRS_TRG_DIS                 ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   MRS_TRG_DIS                 ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   MRS_TRG_DIS                 ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   MRS_TRG_DIS                 ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;
                GRP((SDC_BASE_GRP+0), 8)    =   AREF_CMD_TRIG_VAL           ;

                /*for(i = 0; i < 600; i++) {
                    asm volatile ("nop");
                }*/

                // auto refresh
                //GRP (SDC_BASE_GRP, 8 )=0x00050683 ;
                GRP (SDC_BASE_GRP, 8 ) = AREF_REG_EN_VAL;
                GRP (SDC_BASE_GRP, 18) = 0x00000002 ;//initial done
                // move switch path here
                GRP((PHY_BASE_GRP+0),  0)   =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_DPCU)  ;
            }
//++ 
            GRP((PHY_BASE_GRP+1),  1)   =   DPCU_DT_GO                  ;

            //! wait_dpcu_1st_training
            wait_flag = 0   ;
            do {
                wait_loop (1000)   ;
                wait_flag   =   GRP((PHY_BASE_GRP+1),  0) & 0x0001    ;
                //prn_string("111 training GRP((PHY_BASE_GRP+1),  0)=");prn_dword(GRP((PHY_BASE_GRP+1),  0));prn_string("\n");
            }while ( (wait_flag==0) );

            //! check_1st_dt_result
            rgst_value = (GRP((PHY_BASE_GRP+1), 0) >> 8) & 0x3F ;

            if( rgst_value != 0x0 ){
                //! dpcu training failed
                GRP((PHY_BASE_GRP+0), 0)    =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL)    ;
                dbg_stamp(0xdeada003) ;
                prn_string("<<< 4 leave dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
                return 0 ;
            }//! end- if check dt result

            //! step-3: dpcu_2nd_training - trigger RDQSG training
            GRP((PHY_BASE_GRP+1), 1)    =   DPCU_DT_CFG0 | DT_RG(n_RG_EN) | DPCU_TRAIN_START(n_DT_START) ;
            wait_loop (10000)   ;  //! wait for clear DPCU DT done

            //rgst_value = GRP((PHY_BASE_GRP+1), 0);
            //prn_string("DPCU mid 2sec Training Status G37.0: 0x");prn_dword(rgst_value);prn_string("!!\n");

            //! wait_dpcu_2nd_training
            wait_flag = 0   ;
            do {
                wait_flag   =   (GRP((PHY_BASE_GRP+1),  0) & 0x00000001)    ;
                //prn_string("222 training GRP((PHY_BASE_GRP+1),  0)=");prn_dword(GRP((PHY_BASE_GRP+1),  0));prn_string("\n");
            }while ( (wait_flag==0) );

            //! check_2nd_dt_result
            rgst_value = (GRP((PHY_BASE_GRP+1), 0) >> 9) & 0x01;

            if( rgst_value != 0x0 ){
                //! dpcu training failed
                GRP((PHY_BASE_GRP+0), 0)    =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL)    ;
                dbg_stamp(0xdeada003)   ;
                prn_string( "<<< 5 leave dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
                return 0 ;
            }//! end- if check dt result

            // move switch SDCTRL path before dump
            GRP((PHY_BASE_GRP+0), 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL);
			SET_RDQS_IPRD_TAP_NO(DRAM_ID);

            // Ellison : 20140815 : Check RDQSG Eye boundary result of Each byte correct or not !? ( Left-side < Right-side)
            if( (data_byte_0_RDQSG_left_total_tap >= data_byte_0_RDQSG_right_total_tap) ||
                (data_byte_1_RDQSG_left_total_tap >= data_byte_1_RDQSG_right_total_tap) ) {
                GRP((PHY_BASE_GRP+0), 0) = DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL); // can be removed ?
                prn_string("\tRDQSG Training result : Left side boundary > Right side boundary !!!\n");
                prn_string( "<<< 6 leave dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
                return 0;
            }

            // +- offset(1/8)*2 RG_PSD	
		   temp_2 = (GRP((PHY_BASE_GRP+2), 3)  >>  8) & 0xff;//DX0_RG_IPRD
		   rgst_value = (GRP((PHY_BASE_GRP+2), 14)  >>  16) & 0xff;  //DX0_RG_PSD
		   temp_1 = temp_2/4; //1/4 offset
			prn_string("\t DX0_RG_IPRD = ");prn_dword(temp_2);prn_string("\n");
			prn_string("\t DX0_RG_PSD = ");prn_dword(rgst_value);prn_string("\n");
			prn_string("\t offset(1/4 RG_IPRD)  = ");prn_dword(temp_1);prn_string("\n");
            if( (rgst_value+temp_1) >= (temp_2<<1) || rgst_value < temp_1 ){
				prn_string( "RG_PSD is too big or small\n");
				prn_string( "<<< 7 leave dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
                return 0 ;
            }//! end- if RG_PSD is too big or small

			temp_2 = (GRP((PHY_BASE_GRP+3), 3)  >>  8) & 0xff;//DX1_RG_IPRD
            rgst_value = (GRP((PHY_BASE_GRP+3), 14)  >>  16) & 0xff;  //DX1_RG_PSD
			temp_1 = temp_2/4; //1/4 offset
			prn_string("\t DX1_RG_IPRD = ");prn_dword(temp_2);prn_string("\n");
			prn_string("\t DX1_RG_PSD = ");prn_dword(rgst_value);prn_string("\n");
			prn_string("\t offset(1/4 RG_IPRD)  = ");prn_dword(temp_1);prn_string("\n");
            if( (rgst_value+temp_1) >= (temp_2<<1) || rgst_value < temp_1 ){
                //! dpcu training WL_PSD is too big
				prn_string( "RG_PSD is too big or small\n");
				prn_string( "<<< 7 leave dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
                return 0 ;
            }//! end- if RG_PSD is too big or small		

            //! step-4: switch path to SDCTRL
            /* move this setting before DPCU_DT_RESULT_DUMP, it will switch DFI path to SDCTRL.
             * That is, auto refresh will be releaseed, and dump cost time. We need auto refresh earlier.
            GRP((PHY_BASE_GRP+0), 0)    =   DPCU_GLB_CFG0 | DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL); */
    #endif //! SDRAM_FPGA

    dbg_stamp(0xa003); //! phy-training done !!!
    //! issue PREA & enable AREF after training
    GRP((SDC_BASE_GRP+0), 8)    =   PREA_CMD_TRIG_VAL   ;
    GRP((SDC_BASE_GRP+0), 8)    =   AREF_REG_EN_VAL     ;
    //! SDRAM power initial task completed:
    GRP((SDC_BASE_GRP+0),18)    =   0x0002              ;
    rgst_value = GRP((SDC_BASE_GRP+0),18) ;

    prn_string( "<<< leave 8 dram_training_flow for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
    return 1;
} // end of dram_training_flow

//! ***********************************************************************
//! * FUNC      : dram_init
//! * PARAM     : DRAM_ID
//! * PURPOSE   : to do the following sequences
//! *           : (1). DDR_APHY initial sequence (CTCAL->SSCPLL->PZQ)
//! *           : (2). SDCTRL RGST setting
//! *           : (3). DRAM initial setting by SDCTRL
//! *           : (4). DDR_DPHY+APHY data training
//! ***********************************************************************
int dram_init( unsigned int DRAM_ID )
{
    unsigned int        SDC_BASE_GRP=0  ,
                        PHY_BASE_GRP=0  ;
    unsigned int        temp_1         =0 ;
    unsigned int        temp_2         =0 ;
    unsigned int        temp_3         =0 ;
    unsigned int        package_256_flag;  //! this flag only using in DRAM_ID == 1
    unsigned int        max_init_fail_cnt = 15;
    unsigned int        loop_time       ;
    unsigned int        ret = 0;
    //! -------------------------------------------------------
    //! 0. SDCTRL / DDR_PHY RGST GRP selection
    //! -------------------------------------------------------
    switch ( DRAM_ID ){
        case 0: {
            SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE      ;
            PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE      ;
            break;
        }
        case 1: {
            SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE      ;
            PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE      ;
            break;
        }
    }//! end switch

    loop_time = 0;
    ckobd_training_flag = 0;
    ckobd_re_training_number = 0;
	

    //! 20140728 mazar : add max_init_fail_cnt for sometime training fail
    for(loop_time=0;loop_time<max_init_fail_cnt;loop_time++) {
        ckobd_re_training_number = loop_time;
        ckobd_training_flag = 1;
        package_256_flag = 1; //! this flag only using in DRAM_ID == 1

        //! dram_bootint_flow pass return 1, error return 0;
        //! 20140727 mazar : do not add any action if we encounter APHY INIT ERR, just dump error flag
DRAM_BOOT_FLOW_AGAIN:
        if(!dram_booting_flow( DRAM_ID )){
            //! error check flow
            //! 1. DRAM_ID = 1 and all initial error flag are asserted (note)
            //! => we consider this is 216 package, so package_256_flag = 0;
            //! note : SSCPLL isn't assert, so check PZQ, CTCAL, DDL error flag
            //! and pass all DRAM 1 training flow
            //! 2. DRAM_ID = 1 and not all initial error flag are asserted => we consider this is 256 package, so package_256_flag = 1;
            //! and print the error message
            //! 3. DRAM_ID = 0, print the error message
            prn_string("DPCU_INFO : \t********** DUMP APHY INIT************************\n");
            prn_string("aphy_select_value =");prn_dword(aphy_select_value);
            temp_3 = (GRP(PHY_BASE_GRP, 2) >> 8 ) & 0xf;
            if((DRAM_ID==1) && (aphy_select_value == 0xd)) {
                //! case 1, we think this is 216 pin package, and don't need to dump initial error message
                //! only check PZQ, CTCAL, DDL error flag
                package_256_flag = 0 ;
                // prn_string("is this 216 package !?\n");
            } else {
                //! case 2 or case 3, we dump the initial error flag
                //! DUMP INIT flag
                prn_string("DPCU_INFO : \t********** DUMP APHY INIT error infor @ loop_time = "); prn_decimal(loop_time); prn_string(" ***\n");
                temp_1 = (GRP(PHY_BASE_GRP, 2) >> 8 ) & 0x1;
                temp_2 = (GRP(PHY_BASE_GRP, 2) >> 9 ) & 0x1;
                prn_string("\tCTCAL_ERR flag =");prn_decimal(temp_1);prn_string("\tSSCPLL_ERR flag =");prn_decimal(temp_2);prn_string("\n");
                temp_1 = (GRP(PHY_BASE_GRP, 2) >> 10) & 0x1;
                temp_2 = (GRP(PHY_BASE_GRP, 2) >> 11) & 0x1;
                prn_string("\tDDL_ERR flag =");prn_decimal(temp_1);prn_string("\tPZQ_ERR flag =");prn_decimal(temp_2);prn_string("\n");
                prn_string("DPCU_INFO : \t********** DUMP APHY INIT error information end **********\n") ;
                goto DRAM_BOOT_FLOW_AGAIN;
            }
        } else {
            //prn_string("DRAM-");prn_decimal(DRAM_ID);prn_string("booting PASS @ loop_time =");
            //prn_decimal(loop_time);prn_string("!!\n");
        }
        prn_string("(DRAM_ID)=");prn_decimal(DRAM_ID);prn_string("!!\n");
        prn_string("(package_256_flag)=");prn_decimal(package_256_flag);prn_string("!!\n");

        if ( (DRAM_ID == 0) && ( resume_function == 1 ) ) {
            //! ***********************************************************************
            //! Step 4(DRAM0):  Setting exit Self-refresh flow
            //! ***********************************************************************
            prn_string("\n DRAM-0  self-refresh  exit setting \n");

            // SREF_CMD = 0
            rgst_value = GRP(DRAM_0_SDC_REG_BASE, 10)  ;
            prn_string("G33 ADDR10=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 10));prn_string("\n ");
            GRP(DRAM_0_SDC_REG_BASE, 10) = 0x00006013 ;

            rgst_value = GRP(DRAM_0_SDC_REG_BASE, 8)  ; //Enable AREF_EN
            prn_string("G33 ADDR 8=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 8));prn_string("\n ");
            GRP(DRAM_0_SDC_REG_BASE,  8) = 0x00030683 ;
            // Enable init_done
            //rgst_value = GRP(DRAM_0_SDC_REG_BASE, 18)  ;
            //prn_string("G33 ADDR18=");prn_dword(GRP(DRAM_0_SDC_REG_BASE, 18));prn_string("\n ");
            //GRP(DRAM_0_SDC_REG_BASE, 18) = 0x00000002 ;
            prn_string("\n DRAM-0  self-refresh exit setting done \n");
        }

        //!if (( DRAM_ID == 1) &&(package_256_flag == 1) && ( ( rgst_value & (1<<10) ) != 0 )&& ( resume_function == 1 ) ) {
        if (( DRAM_ID == 1) &&(package_256_flag == 1) && ( resume_function == 1 )) {
            //! ***********************************************************************
            //! Step 4(DRAM1):  Setting exit Self-refresh flow
            //! ***********************************************************************
            prn_string("\n DRAM-1  self-refresh  exit setting \n");

            // SREF_CMD = 0
            rgst_value = GRP(DRAM_1_SDC_REG_BASE, 10)  ;
            prn_string("G46 ADDR10=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 10));prn_string("\n ");
            GRP(DRAM_1_SDC_REG_BASE, 10) = 0x00006013 ;

            rgst_value = GRP(DRAM_1_SDC_REG_BASE, 8)  ; //Enable AREF_EN
            prn_string("G46 ADDR 8=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 8));prn_string("\n ");
            GRP(DRAM_1_SDC_REG_BASE,  8) = 0x00030683 ;
            // Enable init_done
            //rgst_value = GRP(DRAM_1_SDC_REG_BASE, 18)  ;
            //prn_string("G46 ADDR18=");prn_dword(GRP(DRAM_1_SDC_REG_BASE, 18));prn_string("\n ");
            //GRP(DRAM_1_SDC_REG_BASE, 18) = 0x00000002 ;
            prn_string("\n DRAM-1  self-refresh exit setting done \n");
        }
        #ifdef DPCU_PZQ_IMPEDANCE_FROM_REG
           //! setting PZQ CFG1
           prn_string("DPCU PZQ impedance come from RGST!!!!\n");
           GRP((PHY_BASE_GRP+0),19)    =   DPCU_PZQ_CFG1 | DPCU_PZQ_OVRD_ST(n_DPCU_PZQ_OVRD_EN);
           //prn_dword(GRP((PHY_BASE_GRP+0),19));
        #endif

        //! dram_training_flow pass return 1, error return 0;
        //! 20140727 mazar : training flow chart
        //! check DRAM ID , 0 : do training,
        //!                 1 : check package_256_flag, 0 : pass training flow
        //!                                             1 : do training flow
        //! 20140727 mazar :  do not add any action if we encounter APHY Training ERR, just dump error flag
        temp_3 = (GRP(PHY_BASE_GRP, 2) >> 8 ) & 0x1;
        if( ((DRAM_ID==1) && (package_256_flag ==0 ))|| (temp_3 == 1)){
            //! case 1 : (DRAM_ID==1) && (package_256_flag ==0 ) : we think this is 216 package, so don't do data training
            //! case 2 : (temp_3 == 1)                           : because SSCPLL isn't assert error flag, we consider CTCAL error flag
            //! as SSCPLL error flag, so don't do data training too
            return PACKAGE_216; //! break for_loop if encounter case1 or case2
        }

        prn_string("GRP(PHY_BASE_GRP, 9) =  ");prn_dword(GRP(PHY_BASE_GRP, 9));prn_string("\n ");
        ret = dram_training_flow( DRAM_ID );

        if(ret == WAIT_FLAG_FAIL ) {
            prn_string("wait flag or register G(37,10) fail!!!!\n");
            //goto DRAM_BOOT_FLOW_AGAIN;
            return FAIL;
        } else if(ret == 0) {
            //! (DRAM_ID=0) or (DRAM_ID=1 and package_256_flag==1) => do training flow
            //! dump error message

            prn_string("DPCU_INFO : \t********** DUMP init & training error info @ loop_time = "); prn_decimal(loop_time);prn_string(" ***\n") ;
            DPCU_DT_RESULT_DUMP( DRAM_ID );
        } else {
            //! training pass
            //! double check RSL result for SDCTRL setting
            rgst_value = (GRP((PHY_BASE_GRP+2), 14)  >>  0) & 0x1f;
            //prn_string("\tDX0 :   RG_RSL =");prn_byte(temp_1);prn_string("\tDX1 :   RG_RSL =");prn_byte(temp_2);prn_string("\n") ;
            if(rgst_value == 1 ) {
                //! modify SDCTRL STR_DQS_IN if RSL ==1 (STR_DQS_IN -1)
                temp_1 = (GRP(SDC_BASE_GRP, 11) >> 25) & 0x3f;
                temp_2 = (GRP(SDC_BASE_GRP, 11) >> 20) & 0x1f;
                temp_3 = (GRP(SDC_BASE_GRP, 11) >>  8) & 0x0f;
                temp_2 = temp_2 - 1 ;
                GRP(SDC_BASE_GRP, 11) = (temp_1 << 25) | (temp_2 << 20) | (temp_3 << 8);
            } else if (rgst_value == 3) {
                //! modify SDCTRL STR_DQS_IN if RSL ==1 (STR_DQS_IN +1)
                temp_1 = (GRP(SDC_BASE_GRP, 11) >> 25) & 0x3f;
                temp_2 = (GRP(SDC_BASE_GRP, 11) >> 20) & 0x1f;
                temp_3 = (GRP(SDC_BASE_GRP, 11) >>  8) & 0x0f;
                temp_2 = temp_2 + 1 ;
                GRP(SDC_BASE_GRP, 11) = (temp_1 << 25) | (temp_2 << 20) | (temp_3 << 8);
            }
            if (resume_function == 0) {
				int i = 0;
				int pass_count = 0;
				unsigned int TEST_ADDRESS[3] = {0x00000000, 0x08000000, 0x0c800000};
				const unsigned int TEST_COUNT = sizeof(TEST_ADDRESS)>>2;
				for(i=0; i<TEST_COUNT; i++)
				{
                	if(SDCTRL_TRIMMER_TEST_(DRAM_ID, TEST_ADDRESS[i], 0x100) !=0)
						pass_count++;
                }

                if( pass_count == TEST_COUNT ) {
                    prn_string("  ==>  DDR-PHY Training Pass & DRAM Trimmer PASS ....\n\n");
                    break;
                } else {
                    prn_string("  ==>  DDR-PHY Training Pass but DRAM Trimmer FAIL ....\n\n");
                }
            } else if (resume_function == 1 && DRAM_ID==0){
                prn_string("DRAM-0 CPU access PASS(Resume) !!!\n");
                break;
            } else if (resume_function == 1 && DRAM_ID==1){
                prn_string("DRAM-1 CPU access PASS(Resume) !!!\n");
                break;
            }
        }
        if(loop_time + 1 == max_init_fail_cnt){ // robert fix
            prn_string("DRAM-");prn_decimal(DRAM_ID);prn_string("initial failed\n\n");
            //RID_FAIL();
            //while(1); // robert: return fail rather than hang
            return FAIL;
        } //! all loop training fail
    } //! end of for loop :: loop_time for initial & training time control

    prn_string("DRAM-");prn_decimal(DRAM_ID);prn_string("initial done !!!!\n\n") ;
    return SUCCESS;
}//! end dram_init

#ifdef SUPPORT_STR
//! ***********************************************************************
//! * FUNC      : dram_init
//! * PARAM     : DRAM_ID
//! * PURPOSE   : to do the following sequences
//! *           : (1). DDR_APHY initial sequence (CTCAL->SSCPLL->PZQ)
//! *           : (2). SDCTRL RGST setting
//! *           : (3). DRAM initial setting by SDCTRL
//! *           : (4). DDR_DPHY+APHY data training
//! ***********************************************************************
void dram_init_resume( unsigned int DRAM_ID )
{
    unsigned int        SDC_BASE_GRP=0  ,
                        PHY_BASE_GRP=0  ;
    unsigned int        temp_1         =0 ;
    unsigned int        temp_2         =0 ;
    unsigned int        temp_3         =0 ;
    unsigned int        package_256_flag;  //! this flag only using in DRAM_ID == 1
    unsigned int        max_init_fail_cnt = 15;
    unsigned int        loop_time       ;

    //! -------------------------------------------------------
    //! 0. SDCTRL / DDR_PHY RGST GRP selection
    //! -------------------------------------------------------
    switch ( DRAM_ID ){
        case 0: {
            SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE      ;
            PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE      ;
            break;
        }
        case 1: {
            SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE      ;
            PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE      ;
            break;
        }
    }//! end switch

    loop_time = 0;
    ckobd_training_flag = 0;
    ckobd_re_training_number = 0;

    //! 20140728 mazar : add max_init_fail_cnt for sometime training fail
    for(loop_time=0;loop_time<max_init_fail_cnt;loop_time++) {
        ckobd_re_training_number = loop_time;
        ckobd_training_flag = 1;
        package_256_flag = 1; //! this flag only using in DRAM_ID == 1

        prn_string("\n\nloop_time = "); prn_decimal(loop_time); prn_string(" DRAM_ID = "); prn_decimal(DRAM_ID); prn_string("\n");
        /********************/
        /*   booting flow   */
        /********************/
        //! dram_bootint_flow pass return 1, error return 0;
        //! 20140727 mazar : do not add any action if we encounter APHY INIT ERR, just dump error flag
        if(!dram_booting_flow( DRAM_ID )) {
            //! error check flow
            //! 1. DRAM_ID = 1 and all initial error flag are asserted (note)
            //! => we consider this is 216 package, so package_256_flag = 0;
            //! note : SSCPLL isn't assert, so check PZQ, CTCAL, DDL error flag
            //! and pass all DRAM 1 training flow
            //! 2. DRAM_ID = 1 and not all initial error flag are asserted => we consider this is 256 package, so package_256_flag = 1;
            //! and print the error message
            //! 3. DRAM_ID = 0, print the error message
            temp_3 = (GRP(PHY_BASE_GRP, 2) >> 8 ) & 0xf;

            if((DRAM_ID==1) && (temp_3 == 0xd)){
                //! case 1, we think this is 216 pin package, and don't need to dump initial error message
                //! only check PZQ, CTCAL, DDL error flag
                package_256_flag = 0 ;
                // prn_string("is this 216 package !?\n");
            } else {
                //! case 2 or case 3, we dump the initial error flag
                //! DUMP INIT flag
                prn_string("DPCU_INFO : \t********** DUMP APHY INIT error info @ loop_time = "); prn_decimal(loop_time); prn_string(" ***\n");
                temp_1 = (GRP(PHY_BASE_GRP, 2) >> 8 ) & 0x1;
                temp_2 = (GRP(PHY_BASE_GRP, 2) >> 9 ) & 0x1;
                prn_string("\tCTCAL_ERR flag =");prn_decimal(temp_1);prn_string("\tSSCPLL_ERR flag =");prn_decimal(temp_2);prn_string("\n");

                temp_1 = (GRP(PHY_BASE_GRP, 2) >> 10) & 0x1;
                temp_2 = (GRP(PHY_BASE_GRP, 2) >> 11) & 0x1;
                prn_string("\tDDL_ERR flag =");prn_decimal(temp_1);prn_string("\tPZQ_ERR flag =");prn_decimal(temp_2);prn_string("\n");
                prn_string("DPCU_INFO : \t********** DUMP APHY INIT error info end **********\n") ; 
            }
        } else {
            //prn_string("DRAM-");prn_decimal(DRAM_ID);prn_string("booting PASS @ loop_time =");
            //prn_decimal(loop_time);prn_string("!!\n");
        }

#ifdef DPCU_PZQ_IMPEDANCE_FROM_REG
        //! setting PZQ CFG1
        prn_string("DPCU PZQ impedance come from RGST!!!!\n");
        GRP((PHY_BASE_GRP+0),19)    =   DPCU_PZQ_CFG1 | DPCU_PZQ_OVRD_ST(n_DPCU_PZQ_OVRD_EN);
        //prn_dword(GRP((PHY_BASE_GRP+0),19));
#endif
        /*********************/
        /*   training flow   */
        /*********************/
        //! dram_training_flow pass return 1, error return 0;
        //! 20140727 mazar : training flow chart
        //! check DRAM ID , 0 : do training,
        //!                 1 : check package_256_flag, 0 : pass training flow
        //!                                             1 : do training flow
        //! 20140727 mazar :  do not add any action if we encounter APHY Training ERR, just dump error flag
        temp_3 = (GRP(PHY_BASE_GRP, 2) >> 8 ) & 0x1;
            //prn_string("DRAM ID = ");prn_decimal(DRAM_ID);prn_string("package_256_flag = ");prn_decimal(package_256_flag);prn_string("\n");
        if( ((DRAM_ID==1) && (package_256_flag ==0 ))|| (temp_3 == 1)){
            //! case 1 : (DRAM_ID==1) && (package_256_flag ==0 ) : we think this is 216 package, so don't do data training
            //! case 2 : (temp_3 == 1)                           : because SSCPLL isn't assert error flag, we consider CTCAL error flag
            //! as SSCPLL error flag, so don't do data training too
            break; //! break for_loop if encounter case1 or case2
        } else if(!dram_training_flow( DRAM_ID )) {
            //! (DRAM_ID=0) or (DRAM_ID=1 and package_256_flag==1) => do training flow
            //! dump error message
            prn_string("DPCU_INFO : \t********** DUMP init & training error info @ loop_time = "); prn_decimal(loop_time); prn_string(" ***\n") ;
            DPCU_DT_RESULT_DUMP( DRAM_ID );
        } else {
            //! training pass
            //! double check RSL result for SDCTRL setting
            rgst_value = (GRP((PHY_BASE_GRP+2), 14)  >>  0) & 0x1f;
            if(rgst_value == 1) {
                //! modify SDCTRL STR_DQS_IN if RSL ==1 (STR_DQS_IN -1)
                temp_1 = (GRP(SDC_BASE_GRP, 11) >> 25) & 0x3f;
                temp_2 = (GRP(SDC_BASE_GRP, 11) >> 20) & 0x1f;
                temp_3 = (GRP(SDC_BASE_GRP, 11) >>  8) & 0x0f;
                temp_2 = temp_2 - 1 ;
                GRP(SDC_BASE_GRP, 11) = (temp_1 << 25) | (temp_2 << 20) | (temp_3 << 8);
            } else if (rgst_value == 3) {
                //! modify SDCTRL STR_DQS_IN if RSL ==1 (STR_DQS_IN +1)
                temp_1 = (GRP(SDC_BASE_GRP, 11) >> 25) & 0x3f;
                temp_2 = (GRP(SDC_BASE_GRP, 11) >> 20) & 0x1f;
                temp_3 = (GRP(SDC_BASE_GRP, 11) >>  8) & 0x0f;
                temp_2 = temp_2 + 1 ;
                GRP(SDC_BASE_GRP, 11) = (temp_1 << 25) | (temp_2 << 20) | (temp_3 << 8);
            }
            if(SDCTRL_TRIMMER_TEST_(DRAM_ID) !=0) {
                prn_string("  ==>  DDR-PHY Training Pass & DRAM Trimmer PASS ....\n\n");
                break;
            } else {
                prn_string("  ==>  DDR-PHY Training Pass but DRAM Trimmer FAIL ....\n\n");
            }
        }
        // sleep and iso again
        if(( resume_function == 1)) {
            int i = 0;
            prn_string("ENTER SELF-REFRESH for DRAM"); prn_decimal(DRAM_ID); prn_string("\n");
            GRP (SDC_BASE_GRP, 8 )=0x00000283;
            GRP (SDC_BASE_GRP, 18)=0x00000000;
            GRP (SDC_BASE_GRP, 8 )=0x00010283;
            //GRP (SDC_BASE_GRP, 8 )=0x00010287 ;//fot HT
            //test1 delay after stop AREF for tRFC
            GRP (SDC_BASE_GRP, 8 )=0x00010286 ;//STOP AREF first
            for (i=0; i<200; i++) {
                asm volatile ("nop");
            }
            //test1
            GRP (SDC_BASE_GRP, 8 )=0x00030286 ;//issue PREA
            GRP (SDC_BASE_GRP, 10)=0x00006013 ;
            GRP (SDC_BASE_GRP, 10)=0x00006813 ;//enter SREF
            // iso
            rgst_value = GRP(8, 0);
            if(DRAM_ID == 0) {
                GRP(8, 0) = (rgst_value | ((1 << 8)) );
            } else if ((DRAM_ID == 1) && (resume_function == 1)) {
                GRP(8, 0) = (rgst_value | ((1 << 10)) );
            }
        } else {
            // do nothing
        }

        if(loop_time == max_init_fail_cnt) {
            prn_string("DRAM-");prn_decimal(DRAM_ID);prn_string("initial failed\n\n");
            //RID_FAIL();
            while(1);
        } //! all loop training fail
    } //! for loop :: loop_time for initial & training time control
    prn_string("DRAM-");prn_decimal(DRAM_ID);prn_string("initial done !!!!\n\n");
}// end of dram_init_resume
#endif // SUPPORT_STR

//! ***********************************************************************
//! * FUNC      : pll_init_setting
//! * PARAM     : void
//! * PURPOSE   : to call dram_init function for DRAM0/DRAM1
//! ***********************************************************************
void pll_init_setting(void)
{
#if 0 //gemini skip pll-related settings (8700)
    // Whole-Chip PLL Global Setting
    // PLLA: 147.456MHz
    GRP(1, 24) = ( GRP(1, 24) & 0x0000FFFF) | 0x30810000; // bit20:0 PLLA=147.456MHz
    GRP(1, 12) = ( GRP(1, 12) & 0xFFFFF000) | 0x008; // CA9 DFS bit3:1 switch to PLLC
    //GRP(1, 12) = ( GRP(1, 12) & 0xFFFFF000); // CA9 DFS bit3:1 switch to PLLC
#if 1
    // PLLH & DFS for CODA(360MHz)/SGX(216MHz)
    GRP(1, 29) = 0x0015AA05; // PLLH=2160MHz, CLK27_54=54MHz(bit20), enable DCG0/DCG2/DCG4
    GRP(1, 30) = 0x00080703; // DCG0(CODA)=3(360MHz) , DCG2(SGX)=7(216MHz), DCG4(TRACER)=8(180MHz)
    // DFS for CODA & SGX
    GRP(1, 12) = ( GRP(1, 12) & 0xFF000FFF);         // CODA DFS disable
    GRP(1, 13) = ( GRP(1, 13) & 0xFFFC003F);         // SGX  DFS disable
#else
    // PLLH & DFS for CODA(180MHz)/SGX(180MHz)
    GRP(1, 29) = 0x0015AA05; // PLLH=2160MHz, CLK27_54=54MHz(bit20), enable DCG0/DCG2/DCG4
    GRP(1, 30) = 0x00080808; // DCG0(CODA)=9(180MHz) , DCG2(SGX)=9(180MHz), DCG4(TRACER)=8(180MHz)
    // DFS for CODA & SGX
    GRP(1, 12) = ( GRP(1, 12) & 0xFF000FFF);         // CODA DFS disable
    GRP(1, 13) = ( GRP(1, 13) & 0xFFFC003F);         // SGX  DFS disable
#endif
    // PLLTV: 148.5MHz
    GRP(1, 24) = (GRP(1, 24)& 0xFFFFF000) | 0x801 ;  // PLLTV ON
    GRP(1, 25)  = 0x0B454909;                        // bit [12:11]:  1:PLLTV=148.5MHz
    // PLLDIS: 30MHz
    GRP(1, 26) = 0x001A2B01;
    GRP(1, 27) = 0xD2B00142;   // PLLDIS=30MHz(PLLDIS_R=2)
    //reset GPU :"GPU RST setting : bit-3: SGX540_CORE_RESET , bit-4:SGX540_SYS_TESET, bit-5:SGX540_MEM_RESET
    GRP(0, 17)=GRP(0, 17) | 7<<3;
    GRP(0, 17)=GRP(0, 17) & ~(7<<3);
    //reset GMAC
    GRP(0, 17)=GRP(0, 17) | 1<<9;
    GRP(0, 17)=GRP(0, 17) & ~(1<<9);
#endif
}

#ifdef SUPPORT_STR
void io_power_on()
{
    // io power on
    GRP (36, 20)=0x02B6AAD0;
    GRP (36, 21)=0x00009100;
    GRP (49, 20)=0x02B6AAD0;
    GRP (49, 21)=0x00009100;
}

void init_done()
{
    int i;
    GRP (8 , 0 )=0x00000400 ;
    GRP (33, 10)=0x00006013 ;
    GRP (33, 8 )=0x00050687 ;
    GRP (33, 18) = 0x00000002 ;//initial done

    GRP (8 , 0 )=0x00000000 ;
    GRP (46, 10)=0x00006013 ;
    GRP (46, 8 )=0x00050687 ;//issue PREA
    GRP (46, 18) = 0x00000002 ;//initial done

    for (i=0; i<5000; i++) {
        asm volatile ("nop");
    }
}

/* undo self refresh main flow */
void undo_self_refresh()
{
    io_power_on();
    resume_function = 1;
    dram_init_resume(0);
    DPCU_DT_RESULT_DUMP( 0 );

    dram_init_resume(1);

    with_dram_b_package_flag = 1; // check if we have second dram(dram_b)
    rgst_value = (GRP(49, 2) >> 8) & 0xf; // check DRAM 1 APHY INIT error flag
    if(rgst_value == 0xd) {
        //! case 1, we think this is 216 pin package, and don't need to dump initial error message
        //! only check PZQ, CTCAL, DDL error flag
        prn_string("(49, 2) = 0xd, do not do dump\n");
        with_dram_b_package_flag = 0;
    } else {
       DPCU_DT_RESULT_DUMP( 1 );
    }

    //prn_string("[resume] init done for dram 0/1\n");
    //init_done();
}

/* return 1 : resume flag is on */
int is_resume_flag_on()
{
    return 0;
}
#endif // SUPPORT_STR


#ifdef SW_REFINE_DT_RESULT
	/**
	**FOR 8388 , DDR3-1600 ONLY
	**/
	//! ***********************************************************************
	//! * FUNC      : trim_WDM
	//! * PARAM     : void
	//! * PURPOSE   : refine DPCU traing WDM result for DRAM0/DRAM1
	//! ***********************************************************************
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
		rgst_value = GRP(PHY_REG_BASE, 8) & ~(0x3F);
		//#define DEBUG_TRIM_WDM
		
		#ifdef DEBUG_TRIM_WDM
			prn_string("\t DX");prn_decimal(PHY_REG_BASE-52);prn_string(":\n");
		#endif

		//Get IPRD
    	wdq_iprd = ( (GRP((PHY_REG_BASE), 3)  >> 24) & 0xff);//WDQ_IPRD
		#ifdef DEBUG_TRIM_WDM
			prn_string("\t DX");prn_decimal(PHY_REG_BASE-52);prn_string(" IPRD = ");prn_byte(wdq_iprd);prn_string("\n");
		#endif
	
		//Cal each PSD pico second
		for(psd=0; psd<MAX_PSD; psd++)
		{
			if( (psd * wdq_iprd) > IPRD_VALUE )
			{
				psd--;
				break;
			}
		}
		#ifdef DEBUG_TRIM_WDM
			prn_string("\t DX");prn_decimal(PHY_REG_BASE-52);prn_string(" 1 PSD PICO = ");prn_decimal(psd);prn_string("\n");
		#endif

		//FIND FAILED EDGE
		ori_wdm = GRP(PHY_REG_BASE, 8) & 0x3F;
		do{
			new_wdm = ori_wdm + (++trim_count);
			GRP(PHY_REG_BASE, 8) = rgst_value | new_wdm;
			#ifdef DEBUG_TRIM_WDM
				prn_string("\t WDM TRIM COUNT =");prn_decimal(trim_count);prn_string("\n");
			#endif
		}while( DRAM_ACC_TEST(0xff, DRAM0_BASE_ADDR) );

		//CAL OFFSET PSD
		new_wdm = ori_wdm;
		right_psd = SAFE_MARGIN_VALUE - (psd * trim_count);
		if( right_psd > 0xff )
		{
			GRP(PHY_REG_BASE, 8) = rgst_value | ori_wdm;
		}
		else
		{
			#ifdef DEBUG_TRIM_WDM
				prn_string("\t 265<-ORI = ");prn_decimal(right_psd);prn_string(" PICO\n");
			#endif
			for(offset_psd=0; offset_psd<MAX_PSD; offset_psd++)
			{
				if( (psd * offset_psd) > right_psd )
				{
					offset_psd--;
					#ifdef DEBUG_TRIM_WDM
						prn_string("\t DX");prn_decimal(PHY_REG_BASE-52);
						prn_string(" OFFSET_PSD = ");prn_byte(offset_psd);prn_string("\n");
					#endif
					if( ori_wdm >= offset_psd )
						new_wdm = ori_wdm - offset_psd;
					else
					{
						#ifdef DEBUG_TRIM_WDM
							prn_string("\t DX");prn_decimal(PHY_REG_BASE-52);
							prn_string(" WARNING!!! ori_wdm < offset_psd, force set wdm = 0\n");
							prn_string(" OFFSET_PSD = ");prn_byte(ori_wdm);prn_string("\n");
						#endif
						new_wdm = 0;
					}
					break;
				}
			}
			#ifdef DEBUG_TRIM_WDM
				prn_string("\t DX");prn_decimal(PHY_REG_BASE-52);
				prn_string(" ORI_BDD_WDM =");prn_byte(ori_wdm);prn_string("\n");
				prn_string("\t DX");prn_decimal(PHY_REG_BASE-52);
				prn_string(" NEW_BDD_WDM =");prn_byte(new_wdm);prn_string("\n");
			#endif
			GRP(PHY_REG_BASE, 8) = rgst_value | new_wdm;
		}
	}

	//! ***********************************************************************
	//! * FUNC      : dram_refine_flow
	//! * PARAM     : int
	//! * PURPOSE   : refine DPCU traing result for DRAM0/DRAM1
	//! ***********************************************************************
	int dram_refine_flow( unsigned int DRAM_ID )
	{
	    unsigned int        SDC_BASE_GRP=0  ,
	                        PHY_BASE_GRP=0  ;
	    switch ( DRAM_ID ){
	        case 0: {
	            SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE      ;
	            PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE      ;
	            break;
	        }
	        case 1: {
	            SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE      ;
	            PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE      ;
	            break;
	        }
	    }//! end switch
	
		//refine WDM
		trim_WDM(PHY_BASE_GRP+2);
		trim_WDM(PHY_BASE_GRP+3);
	
		return PASS;
	}
#endif



//! ***********************************************************************
//! * FUNC      : dram_init_main
//! * PARAM     : void
//! * PURPOSE   : to call dram_init function for DRAM0/DRAM1
//! ***********************************************************************
int dram_init_main()
{
	int ret, i;	// for DRAM-1
	unsigned int temp_value = 0;

  	/* get mp bit */
	mp = read_mp_bit();

    prn_string(DRAMINIT_BIN_REL_DATE);

#ifdef SUPPORT_STR
    if(is_resume_flag_on()) {
        prn_string("resume is on, run undo self refresh flow\n");
        undo_self_refresh();
        pll_init_setting();
        return 1;
    }
#endif

    //! DRAM-0 initial process
    dram_init( 0 ) ;
    DPCU_DT_RESULT_DUMP( 0 );
#ifdef SW_REFINE_DT_RESULT	
	dram_refine_flow( 0 );
#endif



	#ifdef SDRAM_MPSD_VT_ON
		//SET MPSD_VT THREADHOLD
		rgst_value = GRP(DRAM_0_PHY_REG_BASE+1, 8);
		rgst_value = rgst_value & ~(0xFF0);
		rgst_value = rgst_value | (0xFF<<4);
		GRP(DRAM_0_PHY_REG_BASE+1, 8) = rgst_value;
		
		//Number of MPSD_VT average
		rgst_value = GRP(DRAM_0_PHY_REG_BASE+1, 9);
		rgst_value = rgst_value & ~(0x3);
		rgst_value = rgst_value | 0x3;
        GRP(DRAM_0_PHY_REG_BASE+1, 9) = rgst_value;
		
		//MPSD_VT ON
		rgst_value = GRP(DRAM_0_PHY_REG_BASE+2, 19) | 0x1;
		GRP(DRAM_0_PHY_REG_BASE+2, 19) = rgst_value;
		
		//MPSD_VT ON
		rgst_value = GRP(DRAM_0_PHY_REG_BASE+3, 19) | 0x1;
		GRP(DRAM_0_PHY_REG_BASE+3, 19) = rgst_value;
	#endif
	
	#ifdef SDRAM_RDQSG_VT_ON
		//RGST_VT_DYN_DET_OFFSET
		temp_value = ( ((GRP((DRAM_0_PHY_REG_BASE+2), 3)  >>  8) & 0xff) + ((GRP((DRAM_0_PHY_REG_BASE+3), 3)  >>  8) & 0xff) ) / 16;//avg, 1/16
		rgst_value = GRP(DRAM_0_PHY_REG_BASE+1, 8);
		rgst_value = rgst_value & ~(0xFF<<12);
		#ifdef MPEG_DRAM_DDR_1600
			temp_value = 1;
		#else
			if( temp_value == 0 )
				temp_value = 1;
		#endif
		rgst_value = rgst_value | (temp_value << 12);
		GRP(DRAM_0_PHY_REG_BASE+1, 8) = rgst_value;
		
		//DX0 RDQSG_VT
		rgst_value = GRP(DRAM_0_PHY_REG_BASE+2, 19) | 0x2;
		GRP(DRAM_0_PHY_REG_BASE+2, 19) = rgst_value;
		
		//DX1 RDQSG_VT
		rgst_value = GRP(DRAM_0_PHY_REG_BASE+3, 19) | 0x2;
		GRP(DRAM_0_PHY_REG_BASE+3, 19) = rgst_value;
	#endif


#ifdef SUPPORT_DRAM1
    with_dram_b_package_flag = 1;
    //! DRAM-1 initial process
    for (i = 0; i<10; i++) {
        ret = dram_init( 1 ) ;
		if (ret == PACKAGE_216) {
            //! case 1, we think this is 216 pin package, and don't need to dump initial error message
            //! only check PZQ, CTCAL, DDL error flag
            with_dram_b_package_flag = 0 ;
            return PACKAGE_216_PIN;
	     } else if (ret == FAIL) {
			prn_string("do something...  for GRP(49, 0) or GRP(50, 10)\n");
			prn_string("for loop @ i =  ");prn_dword( i );prn_string("\n ");
			GRP(0, 16) = (GRP(0, 16) | (1<<15) | (1<<17));
			wait_loop(10000);
			prn_string("GRP((16, 0) =  ");prn_dword(GRP(16, 0) );prn_string("\n ");

			GRP(0, 16) = GRP(0, 16) & ( ~ (1<<15) | (1<<17) )  ;
			wait_loop(10000);
			prn_string("GRP((16, 0) =  ");prn_dword(GRP(16, 0) );prn_string("\n ");	
		} else if (ret == SUCCESS) {
			DPCU_DT_RESULT_DUMP( 1 );
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
	
	GRP(DRAM_0_PHY_REG_BASE, 12) = GRP(DRAM_0_PHY_REG_BASE, 12) & ~(0x60000F00);
	GRP(DRAM_0_PHY_REG_BASE, 12) = GRP(DRAM_0_PHY_REG_BASE, 12) | (DPCU_RI_MPLL_DIV_S << 8) | (DPCU_RI_MPLL_SP << 29);
	GRP(DRAM_0_PHY_REG_BASE, 11) = GRP(DRAM_0_PHY_REG_BASE, 11) | (DPCU_RI_MPLL_SSCEN << 2);
	prn_string("DDR PLL SETTING:");prn_dword(DPCU_RI_MPLL_SP);prn_string("\n ");	
#endif
	
    //pll_init_setting();
	return 1;
}
