#include <types.h>
#include <config.h>
#include <common.h>

// Show draminit msg
//#define DBG_SHOW_DRAMINIT_MSG

//! --- DRAM scan DEBUG define ---
//#define DBG_EN_ACC_TEST
//#define DBG_EN_TRIM_INFO
//#define TRIM_ACC_TEST_EN

#define GRP(group, addr)    *((volatile unsigned int *)(RGST_BASE + (group)*128 + ((addr)*4)))

//! --- DRAM scan define ---
#define DRAM_0_SDC_REG_BASE     33
#define DRAM_0_PHY_REG_BASE     50
#define DRAM_1_SDC_REG_BASE     46
#define DRAM_1_PHY_REG_BASE     49

//! Note: DRAM size access range is defined at Makefile.in
//! for 3502 FPGA setting - DRAM1/0 is 256MB/256MB (2Gb/2Gb)
//! for 3502 ASIC setting - please reference production spec.
//#define TEST_LEN                0x4000000000
#define TEST_LEN                0x20
#define DRAM0_BASE_ADDR     0x00000000
#define DRAM1_BASE_ADDR     0x10000000


//! --- extern function call ---
extern int dram_init           (unsigned int)                  ;
extern void RID_PASS            (void)                          ;
extern void RID_FAIL            (void)                          ;
extern int  DRAM_ACC_TEST              (unsigned int, unsigned int)    ;


//! --- internal function declaration ---
void DRAM_WRITE_CLEAN           (unsigned int, unsigned int)    ;
int  NORMAL_TRIM_TEST           (unsigned int, unsigned int)    ;
int  SDCTRL_TRIMMER_TEST_MODES  (unsigned int, unsigned int)    ;
void DRAM_SCAN                  (unsigned int)                  ;


//! ***********************************************************************
//! * FUNC      : DRAM_WRITE_CLEAN
//! * PARAM     : DRAM_ID
//! * PURPOSE   : clean dram content of test region
//! ***********************************************************************
void DRAM_WRITE_CLEAN( unsigned int DRAM_TEST_SIZE , unsigned int DRAM_ID )
{
    unsigned int cpu_wr_index=0 ;

    volatile unsigned int *ram_0 = (volatile unsigned int *)DRAM0_BASE_ADDR;
    volatile unsigned int *ram_1 = (volatile unsigned int *)DRAM1_BASE_ADDR;

    for (cpu_wr_index = 0 ; cpu_wr_index < DRAM_TEST_SIZE ; cpu_wr_index++) {
        if( DRAM_ID == 0 ) {
            ram_0[cpu_wr_index] = 0 ;
        } else {
            ram_1[cpu_wr_index] = 0 ;
        }
    }//! end for


}//! end function => DRAM_WRITE_CLEAN



//! ***********************************************************************
//! * FUNC      : NORMAL_TRIM_TEST
//! * PARAM     : test_round , DRAM_ID
//! * PURPOSE   : to do the random DRAM access by SDCTRL.TRIMMER
//! ***********************************************************************
int NORMAL_TRIM_TEST( unsigned int test_round , unsigned int DRAM_ID)
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
        unsigned int TEST_ADDRESS[3] = {0x00000000, 0x08000000, 0x0c800000};
        const unsigned int TEST_COUNT = sizeof(TEST_ADDRESS)>>2;
        for(i=0; i<100; i++) {
            if( 0 == SDCTRL_TRIMMER_TEST_(DRAM_ID, TEST_ADDRESS[i], 0x100) )
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
    }//! end while

    if( fail_cnt==0 ){
        prn_string("SDCTRL-");prn_decimal(DRAM_ID);prn_string("trimmer test PASS ...\n\n");
        return  1   ;
    } else {
        prn_string("SDCTRL-");prn_decimal(DRAM_ID);prn_string("trimmer test FAIL ...\n\n");
        return  0   ;
    }
}//! end of NORMAL_TRIM_TEST


//! ***********************************************************************
//! * FUNC      : SDCTRL_TRIMMER_TEST_MODES
//! * PARAM     : Trimmer_mode ; DRAM_ID
//! * PURPOSE   : trigger SDC.trimmer
//! * FLOW      :
//! - 1. SET Trimmer parameters
//! - 2. Enable Trimmer Only
//! - 3. Monitor Trimming Finish
//! - 4. Check if All Pass.
//! ***********************************************************************
int SDCTRL_TRIMMER_TEST_MODES( unsigned int mode , unsigned int DRAM_ID )
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
            prn_string("], 1st ERR_DATA_NO = [");prn_decimal((GRP(TRM_BASE_GRP, 22)&0xFFFF))prn_String("]\n");
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
//! * FUNC      : DRAM_SCAN
//! * PARAM     : DRAM_ID
//! * PURPOSE   : do the DRAM_SCAN to findout the best parameters of
//!               SDCTRL and DDR_PHY
//! * RGST field: SDCTRL System Timing
//!     [30:25] : Read Data Path delay 1 cycles
//!     [24:20] : DQS IN delay cycle number =
//!     [19:16] : SUB_INTERNAK_RL = 0
//!     [11: 8] : WL_CNT
//! ***********************************************************************
void DRAM_SCAN( unsigned int DRAM_ID )
{
    unsigned int        idx                 ;
             int        ret                 ;
    unsigned int        rec_idx             ;
    unsigned int        rgst_value          ;

    unsigned int        SDC_BASE_GRP=0      ,
                        PHY_BASE_GRP=0      ;

    unsigned int        sdc_str_dqs_in      ;   //! sdctrl - internal STR_DQS_IN
    unsigned int        sdc_ext_cl_cnt      ;   //! sdctrl - internal EXTRA_CL_CNT
    unsigned int        sdc_int_wl_cnt      ;   //! sdctrl - internal WL_CNT

    unsigned int        cpu_test_result     ;   //! cpu simple write/read result
    unsigned int        trim_test_result    ;   //! trimmer random write/read result

    unsigned int        scan_pass_param[20] ;   //! record max 20 parameters
//! -------------------------------------------------------
//! 0. SDCTRL / DDR_PHY RGST GRP selection
//! -------------------------------------------------------
    switch ( DRAM_ID ){
        case 0: {
            SDC_BASE_GRP    =   DRAM_0_SDC_REG_BASE     ;
            PHY_BASE_GRP    =   DRAM_0_PHY_REG_BASE     ;
            prn_string("\n\nSCAN=> Do the DRAM_SCAN of DRAM-0 !!!\n");
            break;
        }
        case 1: {
            SDC_BASE_GRP    =   DRAM_1_SDC_REG_BASE     ;
            PHY_BASE_GRP    =   DRAM_1_PHY_REG_BASE     ;
            prn_string("\n\nSCAN=> Do the DRAM_SCAN of DRAM-1 !!!\n");
            break;
        }
    }//! end switch

    for( rec_idx=0 ; rec_idx<20 ; rec_idx++ ){
        //! reset record array
        scan_pass_param[rec_idx] = 0 ;
    }

//Ellison test
    //RID_PASS();

    rec_idx = 0 ;
    int mp_bk = 0;
//! -------------------------------------------------------
//! 1. for loop to findout str_dqs_in & ext_cl_cnt
//! -------------------------------------------------------
    for( sdc_str_dqs_in=14 ; sdc_str_dqs_in<=18 ; sdc_str_dqs_in++ ){
    //for( sdc_str_dqs_in=15 ; sdc_str_dqs_in<=15 ; sdc_str_dqs_in++ ){

        for( sdc_ext_cl_cnt=10 ; sdc_ext_cl_cnt<=25 ; sdc_ext_cl_cnt++ ){
        //for( sdc_ext_cl_cnt=21 ; sdc_ext_cl_cnt<=21 ; sdc_ext_cl_cnt++ ){

            for( sdc_int_wl_cnt=7 ; sdc_int_wl_cnt<=11 ; sdc_int_wl_cnt++ ){
            //for( sdc_int_wl_cnt=9 ; sdc_int_wl_cnt<=9 ; sdc_int_wl_cnt++ ){

        extern int mp;
#ifndef DBG_SHOW_DRAMINIT_MSG
        mp = 1; // hide msg
#endif

                mp_bk = mp;
                mp = 0;
                prn_string("\nSCAN=>");
                prn_string("  STR_DQS_IN=");prn_decimal(sdc_str_dqs_in);
                prn_string("  EXT_CL_CNT=");prn_decimal(sdc_ext_cl_cnt);
                prn_string("  INT_WL_CNT=");prn_decimal(sdc_int_wl_cnt);
                mp = mp_bk;
                
                ret = dram_init(DRAM_ID) ; //! do the initial sequence before trigger scan
                if( ret == SUCCESS )
                {
                    #ifdef DBG_SHOW_DRAMINIT_MSG
                        prn_string("\n/======================DRAM PARAMS START======================\\\n");
                        DPCU_DT_RESULT_DUMP( DRAM_ID );
                        prn_string("\n\\======================DRAM PARAMS END======================/\n");
                    #endif
                }
                else
                {
                    continue;
                }
        mp = 0; // show msg

                //! fetch GRP(33, 11) and restore new value
                rgst_value = GRP(SDC_BASE_GRP, 11) & 0x800FF0FF ;
                GRP(SDC_BASE_GRP, 11) = (rgst_value | (sdc_ext_cl_cnt<<25) | (sdc_str_dqs_in<<20) | (sdc_int_wl_cnt<<8)) ;

                //! clean dram content of test region and test result
                cpu_test_result     = 0 ;
                trim_test_result    = 0 ;
                DRAM_WRITE_CLEAN ( TEST_LEN , DRAM_ID ) ;

                //! test-1 : simple CPU W/R test
                cpu_test_result = DRAM_ACC_TEST(TEST_LEN , DRAM_ID) ;

                if( cpu_test_result ){
                    prn_string("\nSCAN=> --------- First Time: CPU Access Test Pass --------");

                    //! test-2 : random trimmer test (after cpu W/R test)
                    unsigned int TEST_ADDRESS[3] = {0x00000000, 0x08000000, 0x0c800000};
                    const unsigned int TEST_COUNT = sizeof(TEST_ADDRESS)>>2;
                    for( idx = 0 ; idx < TEST_COUNT ; idx++ ) {
                        trim_test_result = SDCTRL_TRIMMER_TEST_(DRAM_ID, TEST_ADDRESS[idx], 0x100 ) ;
                        //! check trimmer test result
                        if( trim_test_result ){
                            if( (idx+1) == TEST_COUNT ) {
                                prn_string("\nSCAN=> --------- TRIMMER Test Result : Round- 0 ~ ");
                                prn_decimal(TEST_COUNT);
                                prn_string(" - PASS --------- \n");
                                scan_pass_param[rec_idx] = GRP(SDC_BASE_GRP, 11) ;
                                rec_idx += 1 ;
                            }
                        }else{
                                prn_string("\nSCAN=> --------- TRIMMER Test Result : Round- 0 ~ ");
                                prn_decimal(TEST_COUNT);
                                prn_string(" - FAIL --------- \n");
                                break;
                        }//! end if - trim_test_result
                    }//! end for - trimmer test loop

                }else{
                    prn_string("\nSCAN=> !!!!!!!!! First Time: CPU Access Test Fail !!!!!!!!\n");
                }//! end if - cpu_test_restult
            }//! end for - sdc_int_wl_cnt
        }//! end for - sdc_ext_cl_cnt
    }//! end for - str_dqs_in

//! -------------------------------------------------------
//! 2. dump final pass parameters
//! -------------------------------------------------------
    prn_string("\n\n==================================================================================\n");
    prn_string("DUMP DRAM-");prn_decimal(DRAM_ID);prn_string("parameters\n");
    for( idx=0 ; idx<rec_idx ; idx++){
        if( scan_pass_param[idx] != 0 ){
            //! extract SDCTRL's parameters
            sdc_ext_cl_cnt = (scan_pass_param[idx] >> 25) & 0x3F ;
            sdc_str_dqs_in = (scan_pass_param[idx] >> 20) & 0x1F ;
            sdc_int_wl_cnt = (scan_pass_param[idx] >> 8 ) & 0xF  ;

            //! print out parameters
            prn_string("SCAN=> [");prn_decimal(idx);prn_string("]");
            prn_string(" ; STR_DQS_IN = ");prn_decimal(sdc_str_dqs_in);
            prn_string(" ; EXT_CL_CNT = ");prn_decimal(sdc_ext_cl_cnt);
            prn_string(" ; INT_WL_CNT = ");prn_decimal(sdc_int_wl_cnt);
            prn_string("\n");
        }
    }//! end for - idx
    prn_string("==================================================================================\n");

}//! end of DRAM_SCAN


//! ***********************************************************************
//! * FUNC      : main_function
//! * PARAM     : void
//! * PURPOSE   : DRAM_SCAN main function
//! ***********************************************************************
int dram_scan_main(void)
{
    //! check DRAM_SCAN env
#ifdef  CSIM_FPGA
    prn_string("\nDRAM_SCAN @FPGA test...\n\n");
#elif   CSIM_ASIC
    prn_string("\nDRAM_SCAN @ASIC test...\n\n");
#else
    prn_string("\nError !!! Please define SIM_TYPE at Makefile !!!");
#endif

    prn_string("\n --The program was built at : ");
    prn_string( __DATE__ "\n"  __TIME__ "\n");

    hardware_init();
    //irq_init();

    /* MON shell */
    //registerRW();

#if 0
    // Test : 20140718
    unsigned int    do_dram_init_main_flag       = 1 ;
    if( do_dram_init_main_flag == 1 ){
        dram_init_main();
    }
#endif

    //! process flag
    //unsigned int    do_dram_scan_flag       = 0 ;
    unsigned int    do_dram_scan_flag       = 1 ;
    unsigned int    do_dram_acc_test_flag   = 1 ;

    //! do dram_scan code
    if( do_dram_scan_flag ){
        DRAM_SCAN( 0 ) ;
        prn_string("\n DRAM-0 SCAN test Done !!!");

        //DRAM_SCAN( 1 ) ;
        //prn_string("\n DRAM-1 SCAN test Done !!!");
        
        prn_string("\n DRAM SPEED=");
        prn_dword(GRP((DRAM_0_PHY_REG_BASE+0),12));
    }//! end if - do_dram_scan_flag
    

#ifdef TRIM_ACC_TEST_EN
    //! do DRAM test after dram_scan
    if( do_dram_acc_test_flag ){
        int ret = 0;
        do{
            prn_string("\n TRIM_ACC_TEST:\n");
            ret = DRAM_ACC_TEST(TEST_LEN , 0) ;
            if( 0 == ret )
            {
                prn_string("\DRAM_ACC_TEST FAILED!\n");
                break;
            }            
            ret = SDCTRL_TRIMMER_TEST(0);
            if( 0 == ret )
            {
                prn_string("\nSDCTRL_TRIMMER_TEST FAILED!\n");
                break;
            }
        }while(1);
        prn_string("\n DRAM SPEED=");
        prn_dword(GRP((DRAM_0_PHY_REG_BASE+0),12));
        DPCU_DT_RESULT_DUMP(0);
        //NORMAL_TRIM_TEST(5, 0) ;

        //DRAM_ACC_TEST(5 , 1) ;

        //NORMAL_TRIM_TEST(5, 1) ;

        prn_string("\n Trimmer test Done !!!\n");
    }//! end if - do_dram_acc_test_flag

    prn_string("\n DRAM SCAN / Trimmer test Done !!!\n");

    //RID_PASS() ;
    //prn_string("\n RID-PASS !!!\n ");
#endif

    int i;
    for (i=0; i<10; i++) {
        asm volatile ("nop");
    }
    while(1);
    return 0;
}
