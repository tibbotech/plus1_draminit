// =============================================================================
// SDCTRL and DDR_PHY control and AC parameters definition
// =============================================================================
#ifdef          CSIM_ASIC
#define ASIC_CSIM
#elif defined   CSIM_FPGA
#define SDRAM_FPGA
#else
#error Please define => simulation type
#endif

// =============================================================================
// DRAM-0 & DRAM-1 Configuration Part
// =============================================================================

#define MPEG_DRAM_TYPE_DDR3

// #define MPEG_DRAM_8BIT
#define MPEG_DRAM_16BIT

// #define MPEG_DRAM_DDR_2133
// #define MPEG_DRAM_DDR_1866
// #define MPEG_DRAM_DDR_1600
// #define MPEG_DRAM_DDR_1333
// #define MPEG_DRAM_DDR_1066
// #define MPEG_DRAM_DDR_800
#ifdef PLATFORM_PENTAGRAM
#define MPEG_DRAM_DDR_1066
#elif defined(PLATFORM_I143)
#define MPEG_DRAM_DDR_1333
#endif

#ifdef PLATFORM_PENTAGRAM
#define MPEG_DRAM0_16BIT
#if 1
#define SDRAM0_SIZE_4Gb
#define SDRAM1_SIZE_4Gb
#else
#define SDRAM0_SIZE_512Mb
#define SDRAM1_SIZE_512Mb
#define SDRAM0_SIZE_1Gb
#define SDRAM1_SIZE_1Gb
#endif
#elif defined(PLATFORM_I143)
#define MPEG_DRAM0_16BIT
#if 1
#define SDRAM0_SIZE_8Gb  // due to design workaround the actual size is 4Gb
#define SDRAM1_SIZE_8Gb  // due to design workaround the actual size is 4Gb
#else
#define SDRAM0_SIZE_512Mb
#define SDRAM1_SIZE_512Mb
#define SDRAM0_SIZE_1Gb
#define SDRAM1_SIZE_1Gb
#endif
#endif

#if !(defined(DRAMSCAN) || defined(SISCOPE))
#define SDRAM_MPSD_VT_ON
#define SDRAM_RDQSG_VT_ON
#endif

#ifdef MPEG_DRAM_DDR_1866
#define SW_REFINE_DT_RESULT
#define SAFE_MARGIN_VALUE 215
#define IPRD_VALUE 535
#elif defined MPEG_DRAM_DDR_1600
#define SW_REFINE_DT_RESULT
#define SAFE_MARGIN_VALUE 265
#define IPRD_VALUE 625
#elif defined MPEG_DRAM_DDR_1333
#define SW_REFINE_DT_RESULT
#define SAFE_MARGIN_VALUE 315
#define IPRD_VALUE 750
#elif defined MPEG_DRAM_DDR_1066
#define SW_REFINE_DT_RESULT
#define SAFE_MARGIN_VALUE 395
#define IPRD_VALUE 938
#endif

// Spread Spectrum Clock (SSC)
// ======================================================||
// ||   SP   |       CLK    |    DIV     |      RATE     ||
// ------------------------------------------------------||
// ||   00   |   1333/1600  |   12/14    |   0.51%/0.38% ||
// ------------------------------------------------------||
// ||   01   |   1333/1600  |    6/8     |   1.02%/0.77% ||
// ------------------------------------------------------||
// ||   10   |   1333/1600  |    4/5     |   1.53%/1.19% ||
// ------------------------------------------------------||
// ||   11   |   1333/1600  |    3/4     |   2.04%/1.59% ||
// ======================================================||
// #define SSC_ENABLE
#ifdef SSC_ENABLE
#define SSC_RATE	3	/* 2 bits, 0 - 3 */

#if (SSC_RATE == 0)
#define DPCU_RI_MPLL_DIV_S	14
#elif (SSC_RATE == 1)
#define DPCU_RI_MPLL_DIV_S	8
#elif (SSC_RATE == 2)
#define DPCU_RI_MPLL_DIV_S	4
#elif (SSC_RATE == 3)
#define DPCU_RI_MPLL_DIV_S	4
#endif
#endif	/* SSC_ENABLE */

#define DRAM_SRT_EXTENDED

#ifdef          SDRAM0_SIZE_256Mb
#define SDRAM_SIZE_256Mb
#define MO_SDRAM_A_SIZE 0
#define MO_SDRAM_B_SIZE 0
#elif defined   SDRAM0_SIZE_512Mb
#define SDRAM_SIZE_512Mb
#define MO_SDRAM_A_SIZE 0
#define MO_SDRAM_B_SIZE 0
#elif defined   SDRAM0_SIZE_1Gb
#define SDRAM_SIZE_1Gb
#define MO_SDRAM_A_SIZE 0
#define MO_SDRAM_B_SIZE 0
#elif defined   SDRAM0_SIZE_2Gb
#define SDRAM_SIZE_2Gb
#define MO_SDRAM_A_SIZE 1
#define MO_SDRAM_B_SIZE 1
#elif defined   SDRAM0_SIZE_4Gb
#define SDRAM_SIZE_4Gb
#define MO_SDRAM_A_SIZE 2
#define MO_SDRAM_B_SIZE 2
#elif defined   SDRAM0_SIZE_8Gb
#define SDRAM_SIZE_8Gb
#define MO_SDRAM_A_SIZE 3
#define MO_SDRAM_B_SIZE 3
#else
#error Please define => DRAM size at Makefile
#endif

#ifdef NO_QUICK_SIM
#ifndef BYPASS_MODE
#define NO_QUICK_SIM_AND_NO_BYPASS
#endif
#endif

// -----------------------------------------------------------------
// SDCTRL speed define
// <NOTE>
// - QFH571 can support DRAM-speed from DDR3-800 to DDR3-2133
// - Definition of DRAM speed is defined as 3 groups
//   #1. DDR3-800 to DDR3-1333 : [#define SDRAM_SPEED_400_to_667]
//   #2. DDR3-1600             : [#define SDRAM_SPEED_800]
//   #3. DDR3-2133             : [#define SDRAM_SPEED_1066]
// -----------------------------------------------------------------

#ifdef          MPEG_DRAM_DDR_2133
#define SDRAM_SPEED_1066

#elif defined   MPEG_DRAM_DDR_1866
#define SDRAM_SPEED_933

#elif defined   MPEG_DRAM_DDR_1600
#define SDRAM_SPEED_800

#elif defined   MPEG_DRAM_DDR_1333
#define SDRAM_SPEED_667

#else  // MPEG_DRAM_DDR_1333, MPEG_DRAM_DDR_1066, MPEG_DRAM_DDR_800
#define SDRAM_SPEED_400_to_667
#endif

// =============================================================================
// SDRAM_CFG_STEP_1: System Setting/Mode/Configuration
// =============================================================================
#ifdef PLATFORM_PENTAGRAM
#define USING_INTERNAL_PZQ_CAL
#endif
// #define TRIM_INTERNAL_PZQ_FROM_DPCU
#define DPCU_TRAINING_ODT_ON
// #define DPCU_TRAINING_MANUAL_PAT
// #define DPCU_TRAINING_READ_MPR
// #define DPCU_PZQ_IMPEDANCE_FROM_REG
#define DPCU_TRAINING_RG_LINEAR

// define IC ODT when SDCTRL normal access
// #define SDCTRL_IC_ODT_OFF
// #define SDCTRL_IC_ODT_ON
#define SDCTRL_IC_ODT_DYN

// define DRAM ODT when SDCTRL normal access
// #define SDCTRL_DRAM_ODT_OFF
// #define SDCTRL_DRAM_ODT_ON
#define SDCTRL_DRAM_ODT_DYN

// defined IO impedance on IC side
// #define     SSTL15_RTT40_RO40
// #define     SSTL15_RTT40_RO34
// #define     SSTL15_RTT60_RO40
// #define     SSTL15_RTT60_RO34
// #define     SSTL15_RTT120_RO40
#define     SSTL15_RTT120_RO34

// -----------------------------------------------------------------
// SDCTRL configure define
// L3C / MCPP / Impedance setting
// -----------------------------------------------------------------

// --- IO Impedance ---
#ifdef  MPEG_DRAM_TYPE_DDR3
// DDR3 setting
// RTT-value
#define DDR3_RTT_120
// #define DDR3_RTT_60
// #define DDR3_RTT_40
// #define DDR3_RTT_0

// RO-value
// #define DDR3_ZO_40
#define DDR3_ZO_34

#else
// DDR2 setting
// RTT-value
// #define DDR2_RTT_150
// #define DDR2_RTT_75
// #define DDR2_RTT_50
#define DDR2_RTT_0
#endif

// DRAM_ZQ_CFG => Enable to issue ZQCS command
#ifndef SDRAM_FPGA
#ifdef MPEG_DRAM_TYPE_DDR2
#else
#define DRAM_ZQ_CFG
#endif
#endif

// IN DRAM Controller - MCB port
#define DISABLE_L3_CACHE

// #define DISABLE_L3_ACCESS_DRAM_TO_MBUS

// --- L3C/MCPP ---
// #define L3C_SIZE_64KB
// #define L3C_SIZE_1KB
#define ENABLE_LIM_BURST_LENGTH
// #define MCPP_BYPASS
// #define SDRAM_COMPONENT_FULL_STRENGTH

// -----------------------------------------------------------------
// STR_DQS_IN / extra CL_CNT
// -----------------------------------------------------------------

// -------------------------------------------------------------------------


// =============================================================================
// SDRAM_CFG_STEP_2: SDCTRL parameters setting
// - DRAM Spec
// #1. DRAM type and DRAM size
// #2. Row address / Column address / Bank number
// #3. SCPP/MCPP configuration
// =============================================================================
#ifdef MPEG_DRAM_TYPE_DDR3
#define nDDR_TYPE       3
#else
// DDR2
#define nDDR_TYPE       2
#endif

#if (defined(PLATFORM_PENTAGRAM) || defined(PLATFORM_I143))
#ifdef MPEG_DRAM0_16BIT
  #ifdef SDRAM0_SIZE_256Mb
    #define nROW_WIDTH      12
    #define nCOL_WIDTH       8
    #define nBANK_WIDTH      1
	#define	UMCTL2_RA_NO_12
	#define	UMCTL2_CA_NO_8
	#define	UMCTL2_BA_NO_1
  #elif defined SDRAM0_SIZE_512Mb
    #define nROW_WIDTH      11
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      2
	#define	UMCTL2_RA_NO_11
	#define	UMCTL2_CA_NO_9
	#define	UMCTL2_BA_NO_2
  #elif defined SDRAM0_SIZE_1Gb
    #define nROW_WIDTH      12
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      2
	#define	UMCTL2_RA_NO_12
	#define	UMCTL2_CA_NO_9
	#define	UMCTL2_BA_NO_2
  #elif defined SDRAM0_SIZE_2Gb
    #define nROW_WIDTH      13
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      2
	#define	UMCTL2_RA_NO_13
	#define	UMCTL2_CA_NO_9
	#define	UMCTL2_BA_NO_2
  #elif defined SDRAM0_SIZE_4Gb
    #define nROW_WIDTH      14
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      2
	#define	UMCTL2_RA_NO_14
	#define	UMCTL2_CA_NO_9
	#define	UMCTL2_BA_NO_2
  #elif defined SDRAM0_SIZE_8Gb
    #define nROW_WIDTH      15
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      2
	#define	UMCTL2_RA_NO_15
	#define	UMCTL2_CA_NO_9
	#define	UMCTL2_BA_NO_2
  #else
    #error  Please define => DRAM-0 IC_USE_DRAM_SIZE...
  #endif
#elif defined (MPEG_DRAM0_8BIT) //iam X8 => MPEG_DRAM0_8BIT
  #ifdef SDRAM0_SIZE_256MB
    #define nROW_WIDTH      12
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      1
  #elif defined SDRAM0_SIZE_512MB
    #define nROW_WIDTH      13
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      1
  #elif defined SDRAM0_SIZE_1Gb
    #define nROW_WIDTH      13
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      2
  #elif defined SDRAM0_SIZE_2Gb
    #define nROW_WIDTH      14
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      2
  #elif defined SDRAM0_SIZE_4Gb
    #define nROW_WIDTH      15
    #define nCOL_WIDTH       9
    #define nBANK_WIDTH      2
  #elif defined SDRAM0_SIZE_8Gb
    #error  NO support, please recheck
  #else
    #error  Please define => DRAM-0 IC_USE_DRAM_SIZE...
  #endif
#endif
#endif


#ifdef MPEG_DRAM_16BIT
#ifdef SDRAM1_SIZE_256Mb
#define nROW1_WIDTH      12
#define nCOL1_WIDTH       8
#define nBANK1_WIDTH      1
#elif defined SDRAM1_SIZE_512Mb
#define nROW1_WIDTH      12
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      1
#elif defined SDRAM1_SIZE_1Gb
#define nROW1_WIDTH      12
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      2
#elif defined SDRAM1_SIZE_2Gb
#define nROW1_WIDTH      13
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      2
#elif defined SDRAM1_SIZE_4Gb
#define nROW1_WIDTH      14
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      2
#elif defined SDRAM1_SIZE_8Gb
#define nROW1_WIDTH      15
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      2
#else
#error  Please define => DRAM-1 IC_USE_DRAM_SIZE...
#endif
#elif defined (MPEG_DRAM_8BIT) // iam X8 => MPEG_DRAM_8BIT
#ifdef SDRAM1_SIZE_256Mb
#define nROW1_WIDTH      12
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      1
#elif defined SDRAM1_SIZE_512Mb
#define nROW1_WIDTH      13
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      1
#elif defined SDRAM1_SIZE_1Gb
#define nROW1_WIDTH      13
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      2
#elif defined SDRAM1_SIZE_2Gb
#define nROW1_WIDTH      14
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      2
#elif defined SDRAM1_SIZE_4Gb
#define nROW1_WIDTH      15
#define nCOL1_WIDTH       9
#define nBANK1_WIDTH      2
#elif defined SDRAM1_SIZE_8Gb
#error  NO support, please recheck
#else
#error  Please define => DRAM-1 IC_USE_DRAM_SIZE...
#endif
#endif


#ifdef SDRAM_FPGA
#define nSD_RD_VALID_FROM   1   // 1: capture RD_DATA from SDCTRL to DDR_PHY
// There are no real PHY @ FPGA platform
#elif defined ASIC_CSIM
#define nSD_RD_VALID_FROM   1   // 0: output RD_DATA from DDR_PHY to SDCTRL
#else
#error  Please define => nSD_RD_VALID_FROM
#endif


#if defined(MPEG_DRAM_16BIT)
#define nSD_DATA_BUS_WIDTH  1   // 1: X16bit in DRAM I/F
#define nDATA_LIM_WORD      96  // SDCTRL/MBUS System Configure

#elif defined(MPEG_DRAM_8BIT)
#define nSD_DATA_BUS_WIDTH  0   // 0: X8bit in DRAM I/F
#define nDATA_LIM_WORD      96 // SDCTRL/MBUS System Configure

#else
#error  Please define => nSD_DATA_BUS_WIDTH / nDATA_LIM_WORD
#endif


// SCPP Reorder  Setting
#define  nSCPP_TIMEOUT_CNT      96

// MCPP exception setting
#define nMCPP_LIFE_MODE1_ID3    28
#define nMCPP_LIFE_MODE1_ID2    25
#define nMCPP_LIFE_MODE1_ID1    2
#define nMCPP_LIFE_MODE1_ID0    1
#define nMCPP_LIFE_LEN1         0


// MCPP BANK BL setting
#ifdef PROJ_QAF491
#define  nMCPP_BKLEN_OV0_MODE  0
#define  nMCPP_LBL_PREFER_EN   0
#else // PROJ_QCE483
#define  nMCPP_BKLEN_OV0_MODE  1
#define  nMCPP_LBL_PREFER_EN   1
#endif


// ODT signal timing config
#define  nODTH8_LEN         5 // use in DRAM_ODT_MODE = 3
#define  nTAOND_CNT         2 // use in DRAM_ODT_MODE = 2
#define  nTAOFD_CNT         3 // use in DRAM_ODT_MODE = 2


#ifdef        SDCTRL_IC_ODT_OFF
#define       nSDC_ODT_MODE      0 // Controller's DDR-IO ODT always -off
#elif defined SDCTRL_IC_ODT_ON
#define       nSDC_ODT_MODE      1 // Controller's DDR-IO ODT always -on
#else
// #define SDCTRL_IC_ODT_DYN
#define       nSDC_ODT_MODE      3 // Dynamic ODT extenstion 4T
#endif

//
#ifdef       SDCTRL_DRAM_ODT_OFF
#define       nDRAM_ODT_MODE 0 // DRAM ODT pin , always-off
#elif defined SDCTRL_DRAM_ODT_ON
#define       nDRAM_ODT_MODE 1 // DRAM ODT pin , always-on
#else
// #define SDCTRL_DRAM_ODT_DYN
#define       nDRAM_ODT_MODE 3 // Dynamic ODT on/off
#endif

// DRAM ZQ Setting
#define nZQCS_X_AREF_INTVAL 2048


// =============================================================================
// SDRAM_CFG_STEP_3 DRAM AC parameters
// <NOTE>
// #1. AC parameters do 'not' need to pulse "1", except for "CWL"
// #2. MRS parameters
// =============================================================================

// --- CWL/CL ---
#ifdef SDRAM_FPGA
#define n_tCL           5               // 5T  -- Case latency (set to mrs)

#elif defined(SDRAM_SPEED_400_to_667)
// #ifdef tCK_LESS_533MHz
//  // #define n_tCL           8               //  8T  -- Case latency (set to mrs) (for clock period less than 1.875ns)
//  #define n_tCL           6               //  6T  -- Case latency (set to mrs) try in 800
// #else
// #define n_tCL           10              // 10T -- Case latency (set to mrs)
// #endif

#define n_tCL           8              // 8T -- Case latency (set to mrs)  , 800 and 1066 ok
// #define n_tCL           11              // 11T -- Case latency (set to mrs)
#elif defined(SDRAM_SPEED_667)
#define n_tCL           9
#elif defined(SDRAM_SPEED_800)
#define n_tCL           11              // 11T -- Case latency (set to mrs)

#elif defined(SDRAM_SPEED_933)
#define n_tCL           13
#define DPCU_CL_M2                      // MR0 CL_M2 = 1

#elif defined(SDRAM_SPEED_1066)
#define n_tCL           14              // 14T -- Case latency (set to mrs)
#define DPCU_CL_M2                      // MR0 CL_M2 = 1
#else
#error  Please define DRAM tCL...
#endif


#ifdef SDRAM_FPGA

#ifdef MPEG_DRAM_TYPE_DDR2
#define MRS_CWL_CNT (n_tCL - 1)
#else // DDR3
#define MRS_CWL_CNT 5
#endif // ddr2 /3

#define WL_CNT (MRS_CWL_CNT+1)

#else // ASIC_CSIM
#ifdef MPEG_DRAM_TYPE_DDR2

#define MRS_CWL_CNT (n_tCL - 1)

#else // DDR3

#ifdef SDRAM_SPEED_400_to_667
// DDR3-800 to DDR3-1333
// #ifdef tCK_LESS_533MHz
// // #define MRS_CWL_CNT     5   // 5T  --  Cas Write Latency
// #define MRS_CWL_CNT     8   // 6T  --  Cas Write Latency
// #else
// #define MRS_CWL_CNT     7   // 7T  --  Cas Write Latency
// #endif
#ifdef PLATFORM_PENTAGRAM
#define MRS_CWL_CNT     6   // Cas Write Latency
#else
#define MRS_CWL_CNT     7   // Cas Write Latency
#endif
#elif defined SDRAM_SPEED_667
#define MRS_CWL_CNT     7
// #define MRS_CWL_CNT     8   // 8T  --  Cas Write Latency

#elif defined SDRAM_SPEED_800
// DDR3-1600
#define MRS_CWL_CNT     8   // 8T  --  Cas Write Latency

#elif defined SDRAM_SPEED_933
// DDR3-1866
#define MRS_CWL_CNT     9

#else
#error  Please define DRAM tCWL...
#endif

#endif // MPEG_DRAM_TYPE_DDR2

#define WL_CNT (MRS_CWL_CNT+1)

#endif // SDRAM_FPGA


// --- Other AC parameters ---
#define DRAM_PARAMETER_OFFSET 0
#ifdef SDRAM_FPGA
#define n_tRP   ( 10  + DRAM_PARAMETER_OFFSET ) // 5T  -- Prechage to Active
#define n_tRCD  ( 10  + DRAM_PARAMETER_OFFSET ) // 5T  -- Active-CMD to read/write
#define n_tRC   ( 34 + DRAM_PARAMETER_OFFSET )  // 23T -- Active-CMD to Active-CMD (Same bank)
#define n_tRAS  ( 24 + DRAM_PARAMETER_OFFSET )  // 18T -- Active-CMD to precharge-CMD (Same bank)
#define n_tCCD  ( 4  + DRAM_PARAMETER_OFFSET )  // 2T  -- CAS to CAS (read to read / Write to write)
#define nRTP    ( 6  + DRAM_PARAMETER_OFFSET )  // 3T(tRTP)  -- Read to precharge :
//   nRTP = ( AL (0) + BL/2 + max(tRTP,2) -2 ) = 0 + 4 + 3 - 2 = 5
#define n_tRTW  ( 9  + DRAM_PARAMETER_OFFSET )  // 6T  -- BL=8 Read to write trun around cycle number : 6T

#define n_tWTR  ( 5  + DRAM_PARAMETER_OFFSET )  // 3T  -- DRAM internal dealy Write command to read command delay
#define n_tMRD  ( 4  + DRAM_PARAMETER_OFFSET )  // 2T  -- DRAM's MRS-CMD to MRS/EMRS-CMD
#define n_tMOD  ( 12 + DRAM_PARAMETER_OFFSET )  // 12T  -- DRAM's MRS-CMD to non-MRS/EMRS-CMD
#define n_tRRD  ( 6  + DRAM_PARAMETER_OFFSET )  // 4T  -- Active-CMD to Active-CMD (Diff. bank)
#define n_tFAW  ( 30 + DRAM_PARAMETER_OFFSET )  // 18T --
#define n_tWR   ( 10  + DRAM_PARAMETER_OFFSET ) // 6T  -- Write recovery time
#else // else SDRAM_FPGA
#ifdef MPEG_DRAM_TYPE_DDR2
// DDR2 settings
// #define n_tCL     7                             // 5T  -- Case latency
#define n_tRP   ( 10  + DRAM_PARAMETER_OFFSET ) // 5T  -- Prechage to Active
#define n_tRCD  ( 10  + DRAM_PARAMETER_OFFSET ) // 5T  -- Active-CMD to read/write
#define n_tRC   ( 34 + DRAM_PARAMETER_OFFSET )  // 23T -- Active-CMD to Active-CMD (Same bank)
#define n_tRAS  ( 24 + DRAM_PARAMETER_OFFSET )  // 18T -- Active-CMD to precharge-CMD (Same bank)
#define n_tCCD  ( 4  + DRAM_PARAMETER_OFFSET )  // 2T  -- CAS to CAS (read to read / Write to write)
#define nRTP    ( 6  + DRAM_PARAMETER_OFFSET )  // 3T(tRTP)  -- Read to precharge :
//   nRTP = ( AL (0) + BL/2 + max(tRTP,2) -2 ) = 0 + 4 + 3 - 2 = 5
#define n_tRTW  ( 9  + DRAM_PARAMETER_OFFSET )  // 6T  -- BL=8 Read to write trun around cycle number : 6T

#define n_tWTR  ( 5  + DRAM_PARAMETER_OFFSET )  // 3T  -- DRAM internal dealy Write command to read command delay
#define n_tMRD  ( 4  + DRAM_PARAMETER_OFFSET )  // 2T  -- DRAM's MRS-CMD to MRS/EMRS-CMD
#define n_tMOD  ( 12 + DRAM_PARAMETER_OFFSET )  // 12T  -- DRAM's MRS-CMD to non-MRS/EMRS-CMD
#define n_tRRD  ( 6  + DRAM_PARAMETER_OFFSET )  // 4T  -- Active-CMD to Active-CMD (Diff. bank)
#define n_tFAW  ( 30 + DRAM_PARAMETER_OFFSET )  // 18T --
#define n_tWR   ( 10  + DRAM_PARAMETER_OFFSET ) // 6T  -- Write recovery time
#else // endif - MPEG_DRAM_TYPE_DDR2
// DDR3 setting
#ifdef PLATFORM_PENTAGRAM
#ifdef SDRAM_SPEED_400_to_667
#define n_tRP   ( 8 + DRAM_PARAMETER_OFFSET)   // Prechage to Active
#define n_tRCD  ( 8 + DRAM_PARAMETER_OFFSET)   // Active-CMD to read/write
#define n_tRC   ( 28 + DRAM_PARAMETER_OFFSET)   // Active-CMD to Active-CMD (Same bank)
#define n_tRAS  ( 20 + DRAM_PARAMETER_OFFSET)   // Active-CMD to precharge-CMD (Same bank)
#define n_tCCD  ( 4  + DRAM_PARAMETER_OFFSET)   // 4T  -- CAS to CAS (read to read / Write to write)
#define nRTP    ( 4  + DRAM_PARAMETER_OFFSET)   // Read to precharge :
//    -- ddr3 is read to prechrage, nRTP = tRTP
#define n_tRTW  ( 9  + DRAM_PARAMETER_OFFSET)   // 9T  -- BL=8 Read to write trun around cycle number
//    -- 10+4+2-7 (CL+CCD+2-CWL)
#define n_tWTR  ( 4  + DRAM_PARAMETER_OFFSET)   // DRAM internal dealy Write command to read command delay
#define n_tMRD  ( 4  + DRAM_PARAMETER_OFFSET)   // 4T  -- DRAM's MRS-CMD to MRS/EMRS-CMD
#define n_tMOD  ( 12 + DRAM_PARAMETER_OFFSET)   // 12T -- DRAM's MRS-CMD to non-MRS/EMRS-CMD
#define n_tRRD  ( 6  + DRAM_PARAMETER_OFFSET)   //  Active-CMD to Active-CMD (Diff. bank) ; P.S, depend on DR3-1066
#define n_tFAW  ( 27 + DRAM_PARAMETER_OFFSET)
#define n_tWR   ( 8 + DRAM_PARAMETER_OFFSET)   // Write recovery time
#define MRS_WR_GEN2                             // TWR => for MRS-0 issue, there are 3-ways to encode WR
//    => define GEN3 : n_tWR == 16
//    => define GEN2 : n_tWR == 10/12/14
//    => else case   : n_tWR == 5/6/7/8
#endif
#elif defined(PLATFORM_I143)
#ifdef SDRAM_SPEED_400_to_667
#define n_tRP   ( 10 + DRAM_PARAMETER_OFFSET)
#define n_tRCD  ( 10 + DRAM_PARAMETER_OFFSET)
#define n_tRC   ( 34 + DRAM_PARAMETER_OFFSET)
#define n_tRAS  ( 24 + DRAM_PARAMETER_OFFSET)
#define n_tCCD  ( 4  + DRAM_PARAMETER_OFFSET)
#define nRTP    ( 5  + DRAM_PARAMETER_OFFSET)
#define n_tRTW  ( 9  + DRAM_PARAMETER_OFFSET)
#define n_tWTR  ( 5  + DRAM_PARAMETER_OFFSET)
#define n_tMRD  ( 4  + DRAM_PARAMETER_OFFSET)
#define n_tMOD  ( 12 + DRAM_PARAMETER_OFFSET)
#define n_tRRD  ( 6  + DRAM_PARAMETER_OFFSET)
#define n_tFAW  ( 30 + DRAM_PARAMETER_OFFSET)
#define n_tWR   ( 8 + DRAM_PARAMETER_OFFSET)
#define MRS_WR_GEN2
#elif defined SDRAM_SPEED_667
#define n_tRP   ( 9  + DRAM_PARAMETER_OFFSET)
#define n_tRCD  ( 9  + DRAM_PARAMETER_OFFSET)
#define n_tRC   ( 33 + DRAM_PARAMETER_OFFSET)
#define n_tRAS  ( 24 + DRAM_PARAMETER_OFFSET)
#define n_tCCD  ( 4  + DRAM_PARAMETER_OFFSET)
#define nRTP    ( 5  + DRAM_PARAMETER_OFFSET)
#define n_tRTW  ( 9  + DRAM_PARAMETER_OFFSET)
#define n_tWTR  ( 5  + DRAM_PARAMETER_OFFSET)
#define n_tMRD  ( 4  + DRAM_PARAMETER_OFFSET)
#define n_tMOD  ( 12 + DRAM_PARAMETER_OFFSET)
#define n_tRRD  ( 5  + DRAM_PARAMETER_OFFSET)
#define n_tFAW  ( 30 + DRAM_PARAMETER_OFFSET)
#define n_tWR   ( 10 + DRAM_PARAMETER_OFFSET)
#define MRS_WR_GEN2
#elif defined SDRAM_SPEED_800
#define n_tRP   ( 11 + DRAM_PARAMETER_OFFSET)
#define n_tRCD  ( 11 + DRAM_PARAMETER_OFFSET)   // 11T -- Active-CMD to read/write
#define n_tRC   ( 39 + DRAM_PARAMETER_OFFSET)   // 39T -- Active-CMD to Active-CMD (Same bank)
#define n_tRAS  ( 28 + DRAM_PARAMETER_OFFSET)   // 28T -- Active-CMD to precharge-CMD (Same bank)
#define n_tCCD  ( 4  + DRAM_PARAMETER_OFFSET)   // 4T  -- CAS to CAS (read to read / Write to write)
#define nRTP    ( 6  + DRAM_PARAMETER_OFFSET)   // 6T  -- Read to precharge :
//    -- ddr3 is read to prechrage, nRTP = tRTP
#define n_tRTW  ( 9  + DRAM_PARAMETER_OFFSET)   // 9T  -- BL=8 Read to write trun around cycle number
//    -- 11+4+2-8 (CL+CCD+2-CWL)
#define n_tWTR  ( 6  + DRAM_PARAMETER_OFFSET)   // 6T  -- DRAM internal dealy Write command to read command delay
#define n_tMRD  ( 4  + DRAM_PARAMETER_OFFSET)   // 4T  -- DRAM's MRS-CMD to MRS/EMRS-CMD
#define n_tMOD  ( 12 + DRAM_PARAMETER_OFFSET)   // 12T -- DRAM's MRS-CMD to non-MRS/EMRS-CMD
#define n_tRRD  ( 6  + DRAM_PARAMETER_OFFSET)   // 6T  -- Active-CMD to Active-CMD (Diff. bank)
#define n_tFAW  ( 32 + DRAM_PARAMETER_OFFSET)   // 32T --
#define n_tWR   ( 12 + DRAM_PARAMETER_OFFSET)   // 12T -- Write recovery time
#define MRS_WR_GEN2                             // TWR => for MRS-0 issue, there are 3-ways to encode WR
//    => define GEN3 : n_tWR == 16
//    => define GEN2 : n_tWR == 10/12/14
#elif defined SDRAM_SPEED_933
#define n_tRP   ( 13 + DRAM_PARAMETER_OFFSET)
#define n_tRCD  ( 13 + DRAM_PARAMETER_OFFSET)
#define n_tRC   ( 45 + DRAM_PARAMETER_OFFSET)
#define n_tRAS  ( 32 + DRAM_PARAMETER_OFFSET)
#define n_tCCD  ( 4  + DRAM_PARAMETER_OFFSET)
#define nRTP    ( 6  + DRAM_PARAMETER_OFFSET)
#define n_tRTW  ( 9 + DRAM_PARAMETER_OFFSET)
#define n_tWTR  ( 6  + DRAM_PARAMETER_OFFSET)
#define n_tMRD  ( 4  + DRAM_PARAMETER_OFFSET)
#define n_tMOD  ( 13 + DRAM_PARAMETER_OFFSET)
#define n_tRRD  ( 5  + DRAM_PARAMETER_OFFSET)
#define n_tFAW  ( 32 + DRAM_PARAMETER_OFFSET)
#define n_tWR   ( 13 + DRAM_PARAMETER_OFFSET)
#define MRS_WR_GEN2  //    => else case   : n_tWR == 5/6/7/8
#endif // endif - DDR3 speed
#endif // endif - PLATFORM_I143
#endif // endif - MPEG_DRAM_TYPE_DDR3
#endif // endif - SDRAM_FPGA


// We don't use the self-refresh function in our system.
// NOTE : Don't program the Self-refresh parameter
// - nAREF_INTVAL
// #1. SDR.SDCTRL : nAREF_INTVAL = [(tREFI)/(sdr_clk_period)-31]/32
// #2. HDR.SDCTRL : nAREF_INTVAL = [(tREFI)/(hdr_clk_period)-31]/32
// ex, for 534MHz
// - SDR : nAREF_INTVAL = [(7800/1.87)-31]/32 = 129.3
// - HDR : nAREF_INTVAL = [(7800/3.74)-31]/32 = 64.2
// - tRFC follow SDRAM size, but doesn't related to X16/X8



// DDR3_2_AREF_REG excel table
#ifdef SDRAM_FPGA // 8-24M
#define nAREF_ACC      0
#define nAREF_INTVAL   2
// auto refresh period ~6.96us (8MHz)
#define n_tRFC         6
// auto refresh BW ~17.94%
#elif defined SDRAM_SPEED_333_to_400
#define nAREF_ACC      0
#define nAREF_INTVAL   64
// auto refresh period ~7.78us (333MHz)

// tRFC follow SPEED 400 => period = 2.5ns
#ifdef MPEG_DRAM_TYPE_DDR2
// auto refresh BW ~2.90%
#define n_tRFC  (86 + DRAM_PARAMETER_OFFSET)    // 86T -- Refresh-CMD to Active/Refresh-CMD DDR2 -1Gb 127.5ns

#else   // DDR3
// auto refresh BW ~2.64%
#ifdef SDRAM_SIZE_1Gb
#define n_tRFC  (44  + DRAM_PARAMETER_OFFSET)   // 44T -- Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#elif defined SDRAM_SIZE_2Gb
#define n_tRFC  (64  + DRAM_PARAMETER_OFFSET)   // 64T -- Refresh-CMD to Active/Refresh-CMD DDR3 -2Gb 160ns
#elif defined SDRAM_SIZE_4Gb
#define n_tRFC  (104  + DRAM_PARAMETER_OFFSET)  // 104T -- Refresh-CMD to Active/Refresh-CMD DDR3-4Gb 260ns
#elif defined SDRAM_SIZE_8Gb
#define n_tRFC  (140 + DRAM_PARAMETER_OFFSET)   // 140T -- Refresh-CMD to Active/Refresh-CMD DDR3-8Gb 350ns
#else
#error  Please defined DDR3 n_tRFC ...
#endif

#endif

#elif defined SDRAM_SPEED_400_to_667
#define nAREF_ACC      0
#ifdef PLATFORM_PENTAGRAM
#define nAREF_INTVAL   39 // [str] decrase normal auto refresh interval
#elif defined(PLATFORM_I143)
#define nAREF_INTVAL   47 // [str] decrase normal auto refresh interval
#endif
#ifdef MPEG_DRAM_TYPE_DDR2
#define n_tRFC  (86 + DRAM_PARAMETER_OFFSET)    // 86T -- Refresh-CMD to Active/Refresh-CMD DDR2 -1Gb 127.5ns
#else   // DDR3
#ifdef PLATFORM_PENTAGRAM
#ifdef SDRAM_SIZE_512Mb
#define n_tRFC  (48  + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#elif defined SDRAM_SIZE_1Gb
#define n_tRFC  (59  + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#elif defined SDRAM_SIZE_2Gb
#define n_tRFC  (86  + DRAM_PARAMETER_OFFSET)  // Refresh-CMD to Active/Refresh-CMD DDR3 -2Gb 160ns
#elif defined SDRAM_SIZE_4Gb
#define n_tRFC  (139 + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -4Gb 260ns
#elif defined SDRAM_SIZE_8Gb
#define n_tRFC  (187 + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -8Gb 350ns
#else
#error  Please defined DDR3 n_tRFC ...
#endif
#elif defined PLATFORM_I143
#ifdef SDRAM_SIZE_512Mb
#define n_tRFC  (48  + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_1Gb
#define n_tRFC  (74  + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_2Gb
#define n_tRFC  (107 + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_4Gb
#define n_tRFC  (107 + DRAM_PARAMETER_OFFSET) //Define for I143
//#define n_tRFC  (174 + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_8Gb
#define n_tRFC  (174 + DRAM_PARAMETER_OFFSET) //Define for I143
//#define n_tRFC  (234 + DRAM_PARAMETER_OFFSET)
#else
#error  Please defined DDR3 n_tRFC ...
#endif
#endif
#ifdef CONFIG_DRAM_SIZE_USE_OTP
#define n_tRFC_512Mb  (48  + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#define n_tRFC_1Gb    (59  + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#define n_tRFC_4Gb    (139 + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -4Gb 260ns
#endif
#endif
#elif defined SDRAM_SPEED_667
#define nAREF_ACC      0
#define nAREF_INTVAL   81
#ifdef MPEG_DRAM_TYPE_DDR2
#define n_tRFC  (50 + DRAM_PARAMETER_OFFSET)
#else
#ifdef SDRAM_SIZE_512Mb
#define n_tRFC  (48  + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_1Gb
#define n_tRFC  (74  + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_2Gb
#define n_tRFC  (107 + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_4Gb
#define n_tRFC  (107 + DRAM_PARAMETER_OFFSET) //Define for I143
//#define n_tRFC  (174 + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_8Gb
#define n_tRFC  (174 + DRAM_PARAMETER_OFFSET) //Define for I143
#ifdef CONFIG_DRAM_SIZE_USE_OTP
#define n_tRFC_512Mb  (48  + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#define n_tRFC_1Gb    (59  + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#define n_tRFC_4Gb    (139 + DRAM_PARAMETER_OFFSET)   // Refresh-CMD to Active/Refresh-CMD DDR3 -4Gb 260ns
#endif
#else
#error  Please defined DDR3 n_tRFC ...
#endif
#endif

#elif defined SDRAM_SPEED_800
#define nAREF_ACC      0
#define nAREF_INTVAL   47
// auto refresh period ~7.79us (800MHz)
// auto refresh BW ~2.64%
#ifdef SDRAM_SIZE_1Gb
#define n_tRFC  (85  + DRAM_PARAMETER_OFFSET)   // 88T  -- Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#elif defined SDRAM_SIZE_2Gb
#define n_tRFC  (128 + DRAM_PARAMETER_OFFSET)   // 128T -- Refresh-CMD to Active/Refresh-CMD DDR3 -2Gb 160ns
#elif defined SDRAM_SIZE_4Gb
#define n_tRFC  (207 + DRAM_PARAMETER_OFFSET)   // 208T -- Refresh-CMD to Active/Refresh-CMD DDR3 -4Gb 260ns
#elif defined SDRAM_SIZE_8Gb
#define n_tRFC  (279 + DRAM_PARAMETER_OFFSET)   // 280T -- Refresh-CMD to Active/Refresh-CMD DDR3 -8Gb 350ns
#else
// #error    Please defined DDR3 n_tRFC ...
#define n_tRFC  (88  + DRAM_PARAMETER_OFFSET)   // 88T  -- Refresh-CMD to Active/Refresh-CMD DDR3 -1Gb 110ns
#endif

#elif defined SDRAM_SPEED_933
#define nAREF_ACC      0
#define nAREF_INTVAL   113
#ifdef SDRAM_SIZE_1Gb
#define n_tRFC  (0   + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_2Gb
#define n_tRFC  (149 + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_4Gb
#define n_tRFC  (243   + DRAM_PARAMETER_OFFSET)
#elif defined SDRAM_SIZE_8Gb
#define n_tRFC  (0   + DRAM_PARAMETER_OFFSET)
#else
#define n_tRFC  (0   + DRAM_PARAMETER_OFFSET)
#endif

#else
#error  Please define DRAM tRFC...
#endif

// PUB internal DRAM timing parameters
#define     PUB_DRAM_PARAMETER_OFFSET   1       // PUB DRAM AC parameters offset
#define     n_tWLMRD                    40      // 40T -- Minimum delay from when write leveling mode is programmed to the first DQS/DQS_B rising edge
#define     n_tWLO                      9       //  9T -- Write leveling output delay, speed < DDR3-1333 must set 9T

// PUB.DTPR0
#define     PUB_tRTP        ( nRTP   + PUB_DRAM_PARAMETER_OFFSET )      //  5T -- internal Rd to precharge delay
#define     PUB_tWTR        ( n_tWTR + PUB_DRAM_PARAMETER_OFFSET )      //  5T -- internal WR to RD command delay
#define     PUB_tRP         ( n_tRP  + PUB_DRAM_PARAMETER_OFFSET )      // 10T -- Prechage to Active
#define     PUB_tRCD        ( n_tRCD + PUB_DRAM_PARAMETER_OFFSET )      // 10T -- Active-CMD to RD or WR delay
#define     PUB_tRAS        ( n_tRAS + PUB_DRAM_PARAMETER_OFFSET )      // 25T -- Active-CMD to precharge-CMD (Same bank)
#define     PUB_tRRD        ( n_tRRD + PUB_DRAM_PARAMETER_OFFSET )      //  5T -- Active-CMD to Active-CMD (Diff. bank)
#define     PUB_tRC         ( n_tRC  + PUB_DRAM_PARAMETER_OFFSET )      // 34T -- Active-CMD to Active-CMD (Same bank)
#define     PUB_DTPR0_SET   ( (PUB_tRC<<26)  | (PUB_tRRD<<22) | (PUB_tRAS<<16) | \
			      (PUB_tRCD<<12) | (PUB_tRP<<8)   | (PUB_tWTR<<4)  | (PUB_tRTP<<0) )

// PUB.DTPR1
#define     PUB_tMRD        ( n_tMRD + PUB_DRAM_PARAMETER_OFFSET )      //  4T -- DRAM's MRS-CMD to MRS/EMRS-CMD
#define     PUB_tMOD        ((n_tMOD + PUB_DRAM_PARAMETER_OFFSET)-12 )  // 12T -- DRAM's MRS-CMD to non-MRS/EMRS-CMD
#define     PUB_tFAW        ( n_tFAW + PUB_DRAM_PARAMETER_OFFSET )      // 30T -- 4-bank activate period
#define     PUB_tRFC        ( n_tRFC + PUB_DRAM_PARAMETER_OFFSET )      // 59T -- Refresh-to=Refresh
#define     PUB_tWLMRD      ( n_tWLMRD  )                               // 40T --
#define     PUB_tWLO        ( n_tWLO    )
#define     PUB_DTPR1_SET   ( (PUB_tWLO<<26) | (PUB_tWLMRD<<20) | (PUB_tRFC<<11) | \
			      (PUB_tFAW<<5)  | (PUB_tMOD<<2)    | (PUB_tMRD<<0)    )

// EMR2
#define DPCU_MRS_MODE2_VAL_SET 0
// EMR3
#define DPCU_MRS_MODE3_VAL_SET 0

// =============================================================================
// SDRAM_CFG_STEP_4: Reg Location & Cascade
// - DRAM Spec
// #1. Dram type
// #2. Row address / Column address / Bank number
// =============================================================================

// SDCTRL_CFG
// [15:12] : RA_ADDR_number :   => [12:0]
// [11: 8] : CA_ADDR_number : 8 => [ 8:0]
// [ 3: 2] : Bank number    : 1 => 4 Banks
// [ 1: 0] : SDRAM Type     : 2 => DDR2
#define  DRAM_ROW_WIDTH(n)     ((n) << 12)
#define  DRAM_COLUMN_WIDTH(n)  ((n) <<  8)
#define  DRAM_BANK_WIDTH(n)    ((n) <<  2) // 2'b00: 2-banks, 2'b01: 4-banks, 2'b10: 8-bank
#define  DRAM_DDR_TYPE(n)      ((n) <<  0) // 2: DDR2, 3: DDR3

#define SDRAM0_SIZE_TYPE_VAL     DRAM_ROW_WIDTH(nROW0_WIDTH)    \
	| DRAM_COLUMN_WIDTH(nCOL0_WIDTH) \
	| DRAM_BANK_WIDTH(nBANK0_WIDTH)  \
	| DRAM_DDR_TYPE(nDDR_TYPE)

#define SDRAM1_SIZE_TYPE_VAL     DRAM_ROW_WIDTH(nROW1_WIDTH)    \
	| DRAM_COLUMN_WIDTH(nCOL1_WIDTH) \
	| DRAM_BANK_WIDTH(nBANK1_WIDTH)  \
	| DRAM_DDR_TYPE(nDDR_TYPE)

// ------------------------------------------------------
// SDRAM Access Interval
// CCD CNT[2:0]  bit[14:12]  RW (tCCD) CAS to CAS latency for DDR2. (unit: tMemCLK)
// RTP CNT[3:0]  bit[11: 8]  RW READ to PRECHG spacing (= AL+BL/2+max(tRTP,2)-2) (unit: tMemCLK)
// RTW CNT[3:0]  bit[ 7: 4]  RW READ to WRITE turn around for DDR2 SDRAM
//                           3  (4 cycles): When BL=4
//                           5  (6 cycles): When BL=8
//                           Other values are invalid. (unit: tMemCLK)
// TWTR LEN[3:0] bit[ 3: 0]  RW WRITE to READ spacing for DDR2 SDRAM
//                           (=CL-1+BL/2+tWTR) (unit: tMemCLK)
// ------------------------------------------------------
#define CCD_CNT(n)  ((n) << 12)
#define RTP_CNT(n)  ((n) <<  8)
#define RTW_CNT(n)  ((n) <<  4)
#define WTR_CNT(n)  ((n) <<  0)


// ------------------------------------------------------
// SDRAM Timing Parameters
// Reg-13: bit[ 7: 4] : tRP 4 => 5T
//
// Reg-14: bit[12: 8] : TRC  (23T)
//         bit[ 5: 0] : TRFC_LEN (tRFC)AREF to ACTIVE/AREF period (= tMem- CLK*(TRFC LEN+1))
//
// Reg-15: bit[14:12] : TRRD =>  ( 4T)
//         bit[11: 8] : TRCD =>  ( 5T)
//         bit[ 4: 0] : TRAS =>  (18T)
//
// Reg-16: bit[12: 8] : TFAW = 18T
//         bit[ 2: 0] : tWR (Write recovery ) =6T
// ------------------------------------------------------
// chihhao modified
// #define TMOD(n)                 (n << 16)
// #define TXSR_LSH(n)             (n << 13)
// #define TXSR(n)                 (n <<  8)

#define TXSDLL_LEN(n)           (n<<12)
#define TMOD(n)		        (n<<8)

#define TRP(n)                  (n <<  4)
#define TMRD(n)                 (n <<  0)
#define TRC(n)                  (n <<  8)
#define TRFC(n)                 (n <<  0)
#define TRRD(n)                 (n << 12)
#define TRCD(n)                 (n <<  8)
#define TRAS(n)                 (n <<  0)
#define TWR(n)                  (n <<  0)
#define TFAW(n)                 (n <<  8)

#define SD_PAR_INTERVAL_VAL     CCD_CNT(n_tCCD) | RTP_CNT(nRTP) | RTW_CNT(n_tRTW) | WTR_CNT(n_tWTR)
#define TXSR_tRFC               3
// #define SD_PAR_TIMING0_VAL      TMOD(n_tMOD) | TXSR_LSH(TXSR_tRFC) | TXSR(25) | TRP(n_tRP) | TMRD(n_tMRD)
#define n_tXSDLL  512
#define   SD_PAR_TIMING0_VAL    TXSDLL_LEN(n_tXSDLL) | TMOD(n_tMOD) | TRP(n_tRP) | TMRD(n_tMRD)

#define SD_PAR_TIMING1_VAL      TRC(n_tRC)   | TRFC(n_tRFC)
#define SD_PAR_TIMING2_VAL      TRRD(n_tRRD) | TRCD(n_tRCD) | TRAS(n_tRAS)
#define SD_PAR_TIMING3_VAL      TFAW(n_tFAW) | TWR(n_tWR)

// -------------------------------------------------------
// Auto Refresh Setting
// MRS_TRIG_EN = 1 (bit-15) ; AREF_EN = 0 (bit-0)
// SREF_en     = 0 (bit-14)
// harvey, for DDR2 AREF CMD in 8-30MHz
// -------------------------------------------------------
#define PREA_CMD_TRIG           (1  << 17)
#define AREF_CMD_TRIG           (2  << 17)
#define MRS_CMD_TRIG            (4  << 17)
#define ZQCL_CMD_TRIG           (8  << 17)
#define SG_CMD_INIT_SEQ_EN      (1  << 16)
#define MRS_TRIG_EN             (1  << 15)
#define AREF_FILL_CMD_FIFO_EN   (1  <<  7)
#define AREF_ACC(n)             ((n)<<  2)
#define AREF_SEL                (1  <<  1)
#define AREF_EN                 (1  <<  0)
#define AREF_DIS                (0  <<  0)
#define AREF_INTVAL(n)          ((n)<<  0)

#define MRS_TRG_DIS             AREF_FILL_CMD_FIFO_EN \
	| AREF_ACC(nAREF_ACC)   \
	| AREF_SEL              \
	| AREF_DIS

#define AREF_REG_DIS_VAL        SG_CMD_INIT_SEQ_EN    \
	| MRS_TRIG_EN           \
	| AREF_FILL_CMD_FIFO_EN \
	| AREF_ACC(nAREF_ACC)   \
	| AREF_SEL              \
	| AREF_DIS

#define AREF_REG_EN_VAL         MRS_TRG_DIS | AREF_EN
#define PREA_CMD_TRIG_VAL       PREA_CMD_TRIG | AREF_REG_DIS_VAL
#define AREF_CMD_TRIG_VAL       AREF_CMD_TRIG | AREF_REG_DIS_VAL
#define MRS_CMD_TRIG_VAL        MRS_CMD_TRIG  | AREF_REG_DIS_VAL
#define ZQCL_CMD_TRIG_VAL       ZQCL_CMD_TRIG | AREF_REG_DIS_VAL


// DRAM ZQ Setting
#define ZQCS_X_AREF_INTVAL(n)   ((n) << 16)  // issue zqcs once every X aref times
#define TZQCS_LEN               ( 63 <<  8)
#define ZQCS_CMD_EN             (  1 <<  0)
#define ZQCS_CMD_DIS            (  0 <<  0)

#define ZQCL_CFG_VAL            511

#ifdef DRAM_ZQ_CFG
#define  ZQCS_CFG_VAL         ZQCS_X_AREF_INTVAL(nZQCS_X_AREF_INTVAL) | TZQCS_LEN | ZQCS_CMD_EN
#else
#define  ZQCS_CFG_VAL         ZQCS_X_AREF_INTVAL(nZQCS_X_AREF_INTVAL) | TZQCS_LEN | ZQCS_CMD_DIS
#endif

// SDCTRL/MBUS System Configure
// [12:0]  L2C_RW_BST_CNT_MAX_MCLK
// [13]    L2C_RW_CNT_FULL_EN
#define L2C_RW_CNT_MODE_1           (  1 << 14)
#define L2C_RW_CNT_FULL_EN          (  1 << 13)
#define L2C_RW_CNT_FULL_DIS         (  0 << 13)
#define L2C_RW_BST_CNT_MAX_MCLK(n)  ((n) <<  0)

#ifdef ENABLE_LIM_BURST_LENGTH
#define  DATA_NO_LIM_CFG_VAL    L2C_RW_CNT_MODE_1 | L2C_RW_CNT_FULL_EN  | L2C_RW_BST_CNT_MAX_MCLK(nDATA_LIM_WORD)
#else
#define  DATA_NO_LIM_CFG_VAL    L2C_RW_CNT_MODE_1 | L2C_RW_CNT_FULL_DIS | L2C_RW_BST_CNT_MAX_MCLK(nDATA_LIM_WORD)
#endif

// REG6
// bit[28:24] = PRERAS_H_PTY_WAIT_CNT[4:0]  // 0: wait 1 time
// bit[23:20] = SD_BA2_FIXED_LOC
// bit[19:16] = SD_BA_FIXED_LOC
// bit[   15] = SD_IS_2CS
// bit[   13] = RW_CYC_PRECHG_EN
// bit[   12] = WR_CYC_PRECHG_EN
// bit[   11] = R_BURST_PRECHG_EN
// bit[   10] = SD_PRERAS_DIS
// bit[    8] = DQS_EN_ADV
// bit[    7] = PRERA_HIGH_PTY  // 1: pa/ra always high priority, 0: wait PRERAS_H_PTY_WAIT_CNT+1
// bit[    6] = SD_CKE_EN
// bit[    5] = SD_BA_FIXED
// bit[    4] = SD_BA_CHG
// bit[    3] = SD_SIO_EN
// bit[    2] = BURST8_EN
// bit[ 1: 0] = SD_DATA_BUS_WIDTH[1:0]
// Enable fixed CD-bank, Pre-charge and Active is high priority
// Set Fix-location of DRAM-BA1 for DRAM CMD CD Change
// VPP-Access to A16C16_R format:
#define  RD_VALID_FROM(n)           ((n)<< 29)
#define  PRERAS_H_PTY_WAIT_CNT      ( 0 << 24)  // 0: wait 1 time
#define  SD_BA2_FIXED_LOC           (13 << 20)
#define  SD_BA_FIXED_LOC            (11 << 16)
#define  SD_IS_2CS                  ( 0 << 15)
#define  RW_CYC_PRECHG_EN           ( 0 << 13)
#define  WR_CYC_PRECHG_EN           ( 0 << 12)
#define  R_BURST_PRECHG_EN          ( 1 << 11)
#define  SD_PRERAS_DIS              ( 0 << 10)
#define  DQS_EN_ADV                 ( 0 <<  8)
#define  PRERA_HIGH_PTY             ( 0 <<  7)  // 1: pa/ra always high priority, 0: wait PRERAS_H_PTY_WAIT_CNT+1
#define  SD_CKE_EN                  ( 0 <<  6)
#define  SD_BA_FIXED                ( 1 <<  5)
#define  SD_BA_CHG                  ( 1 <<  4)
#define  SD_SIO_EN                  ( 0 <<  3)
#define  BURST8_EN                  ( 0 <<  2)
#define  SD_DATA_BUS_WIDTH(n)       ((n)<<  0)

#define  SD_SYS_MISC                RD_VALID_FROM(nSD_RD_VALID_FROM) \
	| PRERAS_H_PTY_WAIT_CNT \
	| SD_BA2_FIXED_LOC      \
	| SD_BA_FIXED_LOC       \
	| SD_IS_2CS             \
	| RW_CYC_PRECHG_EN      \
	| WR_CYC_PRECHG_EN      \
	| R_BURST_PRECHG_EN     \
	| SD_PRERAS_DIS         \
	| DQS_EN_ADV            \
	| PRERA_HIGH_PTY        \
	| SD_CKE_EN             \
	| SD_BA_FIXED           \
	| SD_BA_CHG             \
	| SD_SIO_EN             \
	| BURST8_EN             \
	| SD_DATA_BUS_WIDTH(nSD_DATA_BUS_WIDTH)


// -------------------------------------------------------
// SCPP Reorder Configuration
// REORDER_TIMEOUT_VALUE_MCLK_P = R_RGST_BUS[15: 8];
// SB_WCMD_REORDER_EN_MCLK_P    = R_RGST_BUS[    2];
// REORDER_TIMEOUT_EN_MCLK_P    = R_RGST_BUS[    1];
// SB_CMD_REORDER_EN_MCLK_P     = R_RGST_BUS[    0]
// -------------------------------------------------------
#define REORDER_TIMEOUT_VALUE(n)    ((n)<< 8)
#define SB_WCMD_REORDER_EN          ( 1 << 2)
#define REORDER_TIMEOUT_EN          ( 1 << 1)
#define SB_CMD_REORDER_EN           ( 1 << 0)

#define SCPP_CFG_VAL                REORDER_TIMEOUT_VALUE(nSCPP_TIMEOUT_CNT) | \
	SB_WCMD_REORDER_EN | \
	REORDER_TIMEOUT_EN | \
	SB_CMD_REORDER_EN

#define MCPP_LIFE_MODE1_ID3(n)      ((n)<<26)
#define MCPP_LIFE_MODE1_ID2(n)      ((n)<<21)
#define MCPP_LIFE_MODE1_ID1(n)      ((n)<<16)
#define MCPP_LIFE_MODE1_ID0(n)      ((n)<<11)
#define MCPP_LIFE_LEN1(n)           ((n)<< 0)
#define MCPP_LIFE_VAL               MCPP_LIFE_MODE1_ID3(nMCPP_LIFE_MODE1_ID3) \
	| MCPP_LIFE_MODE1_ID2(nMCPP_LIFE_MODE1_ID2) \
	| MCPP_LIFE_MODE1_ID1(nMCPP_LIFE_MODE1_ID1) \
	| MCPP_LIFE_MODE1_ID0(nMCPP_LIFE_MODE1_ID0) \
	| MCPP_LIFE_LEN1(nMCPP_LIFE_LEN1)

#define MCPP_BKLEN_OV0_MODE(n)      ((n)<<20)

#define MCPP_LBL_PREFER_EN(n)       ((n)<< 9)

#ifdef MCPP_BYPASS
#define MCPP_BKLEN_CFG_VAL      0x0038039E // MCPP_EN: bit0 (1 -> 0)
#else
#define MCPP_BKLEN_CFG_VAL     (0xFF28FDEF) \
	| MCPP_BKLEN_OV0_MODE(nMCPP_BKLEN_OV0_MODE) \
	| MCPP_LBL_PREFER_EN(nMCPP_LBL_PREFER_EN)
#endif


// -------------------------------------------------------
// ODT signal timing config
// ODTH8_LEN_P     = R_RGST_BUS[19:16];
// TAOND_CNT_P     = R_RGST_BUS[14:12];
// TAOFD_CNT_P     = R_RGST_BUS[10: 8];
// SDC_ODT_MODE_P  = R_RGST_BUS[ 3: 2];
// DRAM_ODT_MODE_P = R_RGST_BUS[ 1: 0];
// -------------------------------------------------------
#define ODTH8_LEN(n)            ((n)<<16)
#define TAOND_CNT(n)            ((n)<<12)
#define TAOFD_CNT(n)            ((n)<< 8)
#define SDC_ODT_MODE(n)         ((n)<< 2)
#define DRAM_ODT_MODE(n)        ((n)<< 0)

#define ODT_SIGNAL_TIMING_VAL   ODTH8_LEN(nODTH8_LEN)         \
	| TAOND_CNT(nTAOND_CNT)         \
	| TAOFD_CNT(nTAOFD_CNT)         \
	| SDC_ODT_MODE(nSDC_ODT_MODE)   \
	| DRAM_ODT_MODE(nDRAM_ODT_MODE)

#define DDR3_RST_CKE_DISABLE    0x00
#define DDR3_RST_CKE_EN         (3<<0)
#define DDR3_RST_EN             (1<<1)



// -------------------------------------------------------
// MRS Setting
// -------------------------------------------------------

#ifdef MPEG_DRAM_TYPE_DDR2
// -------------------------------------------
// DDR2 SDRAM MRS Register Setting
// -------------------------------------------
#define SDCTRL_MRS_MODE(n)          (n << 13)

// Mode 0
#define MRS_WRITE_RECOVERY_6T       (5 <<  9)
#define MRS_WRITE_RECOVERY_8T	    (7 <<  9)
#define MRS_DLL_RESET               (1 <<  8)
#define MRS_DLL_NORMAL              (0 <<  8)
#define MRS_CAS_LATENCY(n)          (n <<  4)
#define BURST_LEN_4                 (2 <<  0)
#define BURST_LEN_8                 (3 <<  0)

#define MRS_MODE0_VAL_SET_1         SDCTRL_MRS_MODE(0)      |   \
	MRS_WRITE_RECOVERY_6T   |   \
	MRS_DLL_NORMAL          |   \
	MRS_CAS_LATENCY(n_tCL)  |   \
	BURST_LEN_8

#define MRS_MODE0_VAL_SET           MRS_MODE0_VAL_SET_1     |   \
	MRS_DLL_RESET

// -------------------------------------------
// DPCU - DDR2 SDRAM MRS Register Setting
// -------------------------------------------
// Mode
#define DPCU_MRS_MODE0_VAL_SET_1    MRS_WRITE_RECOVERY_8T   |   \
	MRS_DLL_NORMAL          |   \
	MRS_CAS_LATENCY         |   \
	BURST_LEN_8
// EMR
#define DPCU_MRS_MODE1_VAL_SET      SDCTRL_MRS_DQS_B_EN     |   \
	SDCTRL_MRS_OCD_EXIT     |   \
	SDCTRL_MRS_RTT_1        |   \
	SDCTRL_MRS_AL(0)        |   \
	SDCTRL_MRS_RTT_0        |   \
	SDCTRL_MRS_STRENGTH     |   \
	SDCTRL_MRS_DLL_MODE

// EMR2
#define DPCU_MRS_MODE2_VAL_SET      0

// EMR3
#define DPCU_MRS_MODE3_VAL_SET      0

// -------------------------------------------
// DDR2 SDRAM EMRS(1) Register Setting
// -------------------------------------------
// DDR2 EMRS-- RTT:2'b01 =>  75 Ohm
// DDR2 EMRS-- RTT:2'b10 => 150 Ohm
// DDR2 EMRS-- RTT:2'b11 =>  50 Ohm

#ifdef DDR2_RTT_75            // RTT=2'b01
#define SDCTRL_MRS_RTT_1  (0 << 6)
#define SDCTRL_MRS_RTT_0  (1 << 2)
// #define SDCTRL_PAD_RTT_75   // RTT=2
#elif defined DDR2_RTT_150    // RTT=2'b10
#define SDCTRL_MRS_RTT_1  (1 << 6)
#define SDCTRL_MRS_RTT_0  (0 << 2)
// #define SDCTRL_PAD_RTT_150  // RTT=1
#elif defined DDR2_RTT_50     // RTT=2'b11
#define SDCTRL_MRS_RTT_1  (1 << 6)
#define SDCTRL_MRS_RTT_0  (1 << 2)
// #define SDCTRL_PAD_RTT_50   // RTT=3
#else
#define SDCTRL_MRS_RTT_1  (0 << 6)
#define SDCTRL_MRS_RTT_0  (0 << 2)
// #define SDCTRL_PAD_RTT_0    // RTT=0
#endif
// Rtt => 0 = 0 Ohm , => 1 = 75 Ohm , => 2 = 150 Ohm , => 3 = 50 Ohm

#ifdef SDRAM_FPGA
#define SDCTRL_MRS_DLL_MODE     ( 1 <<  0)
#else
#define SDCTRL_MRS_DLL_MODE     ( 0 <<  0)
#endif

#ifdef SDRAM_COMPONENT_FULL_STRENGTH
#define SDCTRL_MRS_STRENGTH     ( 0 <<  1)
#else
#define SDCTRL_MRS_STRENGTH     ( 1 <<  1)
#endif

#define SDCTRL_MRS_AL(n)          ( n <<  3)
#define SDCTRL_MRS_OCD_EXIT       ( 0 <<  7)
#define SDCTRL_MRS_OCD_DEFAULT    ( 7 <<  7)
#define SDCTRL_MRS_DQS_B_EN       ( 0 << 10)


#define MRS_MODE1_VAL_SET   SDCTRL_MRS_MODE(1)    \
	| SDCTRL_MRS_DQS_B_EN   \
	| SDCTRL_MRS_OCD_EXIT   \
	| SDCTRL_MRS_RTT_1      \
	| SDCTRL_MRS_AL(0)      \
	| SDCTRL_MRS_RTT_0      \
	| SDCTRL_MRS_STRENGTH   \
	| SDCTRL_MRS_DLL_MODE

#define MRS_MODE1_VAL_SET_1   MRS_MODE1_VAL_SET       \
	| SDCTRL_MRS_OCD_DEFAULT

// -------------------------------------------
// DDR2 SDRAM EMRS(2) Register Setting
// -------------------------------------------
#define MRS_MODE2_VAL_SET SDCTRL_MRS_MODE(2)

// -------------------------------------------
// DDR2 SDRAM EMRS(3) Register Setting
// -------------------------------------------
#define MRS_MODE3_VAL_SET SDCTRL_MRS_MODE(3)

#else // ----- DDR3 MRS Setting -----

// -------------------------------------------
// DDR3 SDRAM MRS Register Setting
// -------------------------------------------
#define SDCTRL_MRS_MODE(n)    ( n << 13)

// Mode 0
#if defined(SDRAM_SPEED_933)
#define MRS_WRITE_RECOVERY    ((n_tWR - 6) <<  9)

#elif defined(SDRAM_SPEED_800)
#define MRS_WRITE_RECOVERY    ((n_tWR - 6) <<  9)

#elif defined(SDRAM_SPEED_400_to_667)
#define MRS_WRITE_RECOVERY    ((n_tWR - 4) <<  9)

#else
#define MRS_WRITE_RECOVERY    ((n_tWR - 4) <<  9)

#endif

#define MRS_DLL_RESET         (         1  <<  8)
#define MRS_DLL_ON            (         1  << 12)  //
#ifdef DPCU_CL_M2
#define MRS_CAS_LATENCY       ((n_tCL -12) <<  4)
#define MRS_CL_M2             (         1  <<  2)
#else
#define MRS_CAS_LATENCY       ((n_tCL - 4) <<  4)
#define MRS_CL_M2             (         0  <<  2)
#endif
#define DDR3_BURST_LEN        (         1  <<  0)  // bc4/bl8 otf

#define MRS_MODE0_VAL_SET   MRS_DLL_ON          \
	| SDCTRL_MRS_MODE(0)  \
	| MRS_WRITE_RECOVERY  \
	| MRS_DLL_RESET       \
	| MRS_CAS_LATENCY     \
	| MRS_CL_M2           \
	| DDR3_BURST_LEN

// -------------------------------------------
// DDR3 SDRAM EMRS(1) Register Setting
// -------------------------------------------
// DDR3 EMRS-- RTT_NOM:3'b001 =>  60 Ohm
// DDR3 EMRS-- RTT_NOM:3'b010 => 120 Ohm
// DDR3 EMRS-- RTT_NOM:3'b011 =>  40 Ohm

#define SDCTRL_MRS_RTT_2  (0 << 9)

#ifdef DDR3_RTT_60            // RTT=3'b001
#define SDCTRL_MRS_RTT_1    (0 <<  6)
#define SDCTRL_MRS_RTT_0    (1 <<  2)
// #define SDCTRL_PAD_RTT_75   // RTT=2
#elif defined DDR3_RTT_120    // RTT=3'b010
#define SDCTRL_MRS_RTT_1    (1 <<  6)
#define SDCTRL_MRS_RTT_0    (0 <<  2)
// #define SDCTRL_PAD_RTT_150  // RTT=1
#elif defined DDR3_RTT_40     // RTT=3'b011
#define SDCTRL_MRS_RTT_1    (1 <<  6)
#define SDCTRL_MRS_RTT_0    (1 <<  2)
// #define SDCTRL_PAD_RTT_50   // RTT=3
#else
#define SDCTRL_MRS_RTT_1    (0 <<  6)
#define SDCTRL_MRS_RTT_0    (0 <<  2)
// #define SDCTRL_PAD_RTT_0    // RTT=0
#endif
// Rtt => 0 = 0 Ohm , => 1 = 60 Ohm , => 2 = 120 Ohm , => 3 = 40 Ohm

#ifdef SDRAM_FPGA
#define SDCTRL_MRS_DLL_MODE (1 <<  0)
#else
#define SDCTRL_MRS_DLL_MODE (0 <<  0)
#endif

#ifdef DDR3_ZO_40
#define SDCTRL_MRS_ODS_1    (0 <<  5)
#define SDCTRL_MRS_ODS_0    (0 <<  1)
#else // ZO 34
#define SDCTRL_MRS_ODS_1    (0 <<  5)
#define SDCTRL_MRS_ODS_0    (1 <<  1)
#endif

#define SDCTRL_MRS_WL         (0 <<  7)
#define SDCTRL_MRS_AL(n)      (n <<  3)
#define SDCTRL_MRS_TDQS       (0 << 11)
#define SDCTRL_MRS_QOFF       (0 << 12)


#define MRS_MODE1_VAL_SET   SDCTRL_MRS_MODE(1)  \
	| SDCTRL_MRS_QOFF     \
	| SDCTRL_MRS_TDQS     \
	| SDCTRL_MRS_RTT_2    \
	| SDCTRL_MRS_WL       \
	| SDCTRL_MRS_RTT_1    \
	| SDCTRL_MRS_ODS_1    \
	| SDCTRL_MRS_AL(0)    \
	| SDCTRL_MRS_RTT_0    \
	| SDCTRL_MRS_ODS_0    \
	| SDCTRL_MRS_DLL_MODE

// -------------------------------------------
// DDR3 SDRAM EMRS(2) Register Setting
// -------------------------------------------
#define SDCTRL_MRS_RTT_WR   (2 << 9) // 0 : Disable DDR3 Dynamic ODT , 1 : Dynamic ODT , Rtt(WR)= 60Ohm , 2 : Dynamic ODT , Rtt(WR)= 120Ohm ,
#define SDCTRL_MRS_SRT      (0 << 7)
#define SDCTRL_MRS_SRT_EXT  (1 << 7)
#define SDCTRL_MRS_ASR      (0 << 6)
#define SDCTRL_MRS_CWL      ((MRS_CWL_CNT-5) << 3)

#ifdef DRAM_SRT_EXTENDED
#define MRS_MODE2_VAL_SET   SDCTRL_MRS_MODE(2)  \
	| SDCTRL_MRS_RTT_WR   \
	| SDCTRL_MRS_SRT_EXT  \
	| SDCTRL_MRS_ASR      \
	| SDCTRL_MRS_CWL
#else
#define MRS_MODE2_VAL_SET   SDCTRL_MRS_MODE(2)  \
	| SDCTRL_MRS_RTT_WR   \
	| SDCTRL_MRS_SRT      \
	| SDCTRL_MRS_ASR      \
	| SDCTRL_MRS_CWL
#endif
// -------------------------------------------
// DDR3 SDRAM EMRS(3) Register Setting
// -------------------------------------------
#define MRS_MODE3_VAL_SET SDCTRL_MRS_MODE(3)

#endif // end if - MRS setting


// -------------------------------------------------------
// Legacy DDR_PHY IP setting
// -------------------------------------------------------
// prepare pub used mrs setting
#define  PUB_MRS_DDR2_SET               0x040A
#define  PUB_MRS_MODE0_VAL_SET          MRS_MODE0_VAL_SET & 0x1FFF
#define  PUB_MRS_MODE1_VAL_SET          MRS_MODE1_VAL_SET & 0x1FFF
#define  PUB_MRS_MODE2_VAL_SET          MRS_MODE2_VAL_SET & 0x1FFF
#define  PUB_MRS_MODE3_VAL_SET          MRS_MODE3_VAL_SET & 0x1FFF

// CK BDD setting
#define AC0_CK0BD                       0x3C
#define AC0_CK1BD                       0x3C
#define AC0_CK2BD                       0x3C
#define AC0_ADDR_CMD_BD                 0x3C
#define AC0_BIT_DL_SETTING              (AC0_ADDR_CMD_BD<<18) | (AC0_CK2BD << 12) | (AC0_CK1BD << 6) | (AC0_CK0BD << 0)

// set SDRAM Init Done in DDR PHY
#define  PUB_DFI_SEL                    (1 <<  5)
#define  CTL_DFI_SEL                    (0 <<  5)
#define  DDR_PHY_CFG_A7_A4_PART(n)      (n <<  0)
#define  DDR_PHY_CFG_A3_A0(n)           (n <<  0)
#define  DDR_PHY_INIT                   (1 <<  0)
#define  DDR_PHY_ZCAL                   (1 <<  1)
#define  DDR_PHY_PLLINIT                (1 <<  4)
#define  DDR_PHY_DCAL                   (1 <<  5)
#define  DDR_PHY_PHYRST                 (1 <<  6)
#define  DDR_PHY_DRAMRST                (1 <<  7)
#define  DDR_PHY_DRAMINIT               (1 <<  8)

#ifdef MPEG_DRAM_TYPE_DDR3
#define  DDR_PHY_WL                 (1 <<  9)
#define  DDR_PHY_WLADJ              (1 << 11)
#else
#define  DDR_PHY_WL                 (0 <<  9)
#define  DDR_PHY_WLADJ              (0 << 11)
#endif

#define  DDR_PHY_QSGATE                 (1 << 10)
#define  DDR_PHY_RDDSKW                 (1 << 12)
#define  DDR_PHY_WRDSKW                 (1 << 13)
#define  DDR_PHY_RDEYE                  (1 << 14)
#define  DDR_PHY_WREYE                  (1 << 15)
#define  DDR_PHY_ICPC                   (1 << 16)
#define  DDR_PHY_PLL_BYP                (1 << 17)
#define  DDR_PHY_CTLDINIT               (1 << 18)

// DATA_TRAINING SETTING
#define  DATA_TRAIN_WL_SETTING          (0x0300C403)
#define  DDR3_MPR_SETTING               (0x010015C7)
#define  DATA_TRAIN_DISABLE             (0x7C000E80)

#ifdef SDRAM_FPGA
#define DDR_PHY_DATA_TRAINING         BYPASS_SET          | \
	DDR_PHY_CTLDINIT    | \
	DDR_PHY_ICPC        | \
	DDR_PHY_WL          | \
	DDR_PHY_QSGATE      | \
	DDR_PHY_WLADJ       | \
	DDR_PHY_INIT
#else
#ifdef MPEG_DRAM_TYPE_DDR3
#define DDR_PHY_DATA_TRAINING   BYPASS_SET          | \
	DDR_PHY_CTLDINIT    | \
	DDR_PHY_ICPC        | \
	DDR_PHY_WL          | \
	DDR_PHY_QSGATE      | \
	DDR_PHY_WLADJ       | \
	DDR_PHY_RDDSKW      | \
	DDR_PHY_WRDSKW      | \
	DDR_PHY_RDEYE       | \
	DDR_PHY_WREYE       | \
	DDR_PHY_INIT
#else // MPEG_DRAM_TYPE_DDR2
#define DDR_PHY_DATA_TRAINING   BYPASS_SET          | \
	DDR_PHY_CTLDINIT    | \
	DDR_PHY_ICPC        | \
	DDR_PHY_QSGATE      | \
	DDR_PHY_WLADJ       | \
	DDR_PHY_RDDSKW      | \
	DDR_PHY_WRDSKW      | \
	DDR_PHY_RDEYE       | \
	DDR_PHY_WREYE       | \
	DDR_PHY_INIT
#endif // MPEG_DRAM_TYPE_DDR2
#endif

#define DDR_PHY_QS2_TRAINING            BYPASS_SET          | \
	DDR_PHY_CTLDINIT    | \
	DDR_PHY_ICPC        | \
	DDR_PHY_QSGATE      | \
	DDR_PHY_INIT


#ifdef BYPASS_MODE
#define LCDL_SET_1              0xBFBFBFBF
#define GDQS_LCDL_SET           0xFF
#else // BYPASS_MODE
#ifdef SDRAM_SPEED_534_to_667
#define LCDL_SET_1              0x2E2E2E2E
#define GDQS_LCDL_SET           0x24
#else
#define LCDL_SET_1              0x2E2E2E2E
#define GDQS_LCDL_SET           0xA0
#endif
#endif // BYPASS_MODE

// =============================================================================
// SDRAM_CFG_STEP_5 - DDR_APHY Setting
// =============================================================================

// -------------------------------------------------------
// DPCU Global setting - ADR[0]
// -------------------------------------------------------
#define n_DFI_PATH_DPCU         1
#define n_DFI_PATH_SDCTRL       0
#define DPCU_DFI_PATH_SEL(n)    ((n)<<0)

#define VT_CALIB_DIS            (0<<4)

#define DPCU_DDR3_MODE          (1<<5)

#ifdef PLATFORM_PENTAGRAM
#if 1
#define DPCU_GLB_DEF            0x0431AA00
#else	/* for external DRAM */
#define DPCU_GLB_DEF            0x0031AA00
#endif
#elif defined(PLATFORM_I143)
#if 0
#define DPCU_GLB_DEF            0x0430AA00
#else	/* for external DRAM */
#define DPCU_GLB_DEF            0x0030AA00
#endif
#endif


#define DPCU_GLB_CFG0           DPCU_GLB_DEF                        | \
	DPCU_DDR3_MODE                      | \
	VT_CALIB_DIS                        | \
	DPCU_DFI_PATH_SEL(n_DFI_PATH_SDCTRL)| \
	DPCU_RI_PZQ_RES_SEL_SRC(n_DPCU_RI_PZQ_RES_SEL_SRC_EN)


#define DPCU_INIT_TIMMER        0x000A1220   // default is 20'd330000, now is 20'd660000 to prevent PZQ init error (trying)


// -------------------------------------------------------
// DPCU APHY initial setting
// -------------------------------------------------------
#define n_AI_SW_CTRL_DIS        0
#define n_AI_SW_CTRL_EN         1
#define AI_SW_CTRL(n)           ((n)<<0)

#define n_AI_SW_RST_ASSERT      0
#define n_AI_SW_RST_RELEASE     1
#define AI_SW_RST_B(n)          ((n)<<1)

#define n_AI_INIT_START_DEF     0
#define n_AI_INIT_START_EN      1
#define AI_INIT_START(n)        ((n)<<2)

#define AI_RST_SYNC_EN          (  1<<4)

// initial enable
#define n_PZQ_INIT_EN           1
#define n_DDL_INIT_EN           1
#define n_MPLL_INIT_EN          1
#define n_CTCAL_INIT_EN         1

#define n_PZQ_INIT_DIS          0
#define n_DDL_INIT_DIS          0
#define n_MPLL_INIT_DIS         0
#define n_CTCAL_INIT_DIS        0

#define AI_CTCAL_INIT_EN(n)     ((n)<<8)
#define AI_MPLL_INIT_EN(n)      ((n)<<9)
#define AI_DDL_INIT_EN(n)       ((n)<<10)
#define AI_PZQ_INIT_EN(n)       ((n)<<11)

// initial bypass
#define n_PZQ_INIT_BYPASS       1
#define n_DDL_INIT_BYPASS       1
#define n_MPLL_INIT_BYPASS      1
#define n_CTCAL_INIT_BYPASS     1

#define n_PZQ_INIT_NO_BYPASS    0
#define n_DDL_INIT_NO_BYPASS    0
#define n_MPLL_INIT_NO_BYPASS   0
#define n_CTCAL_INIT_NO_BYPASS  0

#define AI_CTCAL_INIT_BYPASS(n) ((n)<<12)
#define AI_MPLL_INIT_BYPASS(n)  ((n)<<13)
#define AI_DDL_INIT_BYPASS(n)   ((n)<<14)
#define AI_PZQ_INIT_BYPASS(n)   ((n)<<15)

#ifdef  NO_QUICK_SIM_AND_NO_BYPASS
#define DPCU_AI_CFG0            AI_PZQ_INIT_BYPASS  (n_PZQ_INIT_NO_BYPASS)  | \
	AI_DDL_INIT_BYPASS  (n_DDL_INIT_NO_BYPASS)  | \
	AI_MPLL_INIT_BYPASS (n_MPLL_INIT_NO_BYPASS) | \
	AI_CTCAL_INIT_BYPASS(n_CTCAL_INIT_NO_BYPASS)| \
	AI_PZQ_INIT_EN      (n_PZQ_INIT_EN)         | \
	AI_DDL_INIT_EN      (n_DDL_INIT_EN )        | \
	AI_MPLL_INIT_EN     (n_MPLL_INIT_EN  )      | \
	AI_CTCAL_INIT_EN    (n_CTCAL_INIT_EN  )     | \
	AI_RST_SYNC_EN                              | \
	AI_INIT_START       (n_AI_INIT_START_DEF)   | \
	AI_SW_RST_B         (n_AI_SW_RST_RELEASE)   | \
	AI_SW_CTRL          (n_AI_SW_CTRL_DIS)
#else
// bypass DDL initial during QUICK_SIM_MODE
#define DPCU_AI_CFG0            AI_PZQ_INIT_BYPASS  (n_PZQ_INIT_NO_BYPASS)  | \
	AI_DDL_INIT_BYPASS  (n_DDL_INIT_BYPASS)     | \
	AI_MPLL_INIT_BYPASS (n_MPLL_INIT_NO_BYPASS) | \
	AI_CTCAL_INIT_BYPASS(n_CTCAL_INIT_NO_BYPASS)| \
	AI_PZQ_INIT_EN      (n_PZQ_INIT_EN)         | \
	AI_DDL_INIT_EN      (n_DDL_INIT_DIS)        | \
	AI_MPLL_INIT_EN     (n_MPLL_INIT_EN)        | \
	AI_CTCAL_INIT_EN    (n_CTCAL_INIT_EN)       | \
	AI_RST_SYNC_EN                              | \
	AI_INIT_START       (n_AI_INIT_START_DEF)   | \
	AI_SW_RST_B         (n_AI_SW_RST_RELEASE)   | \
	AI_SW_CTRL          (n_AI_SW_CTRL_DIS)

#endif

#define DPCU_AI_CFG0_SELECT1    AI_PZQ_INIT_BYPASS  (n_PZQ_INIT_NO_BYPASS)     | \
	AI_DDL_INIT_BYPASS  (n_DDL_INIT_NO_BYPASS)  | \
	AI_MPLL_INIT_BYPASS (n_MPLL_INIT_NO_BYPASS) | \
	AI_CTCAL_INIT_BYPASS(n_CTCAL_INIT_NO_BYPASS)| \
	AI_PZQ_INIT_EN      (n_PZQ_INIT_EN)         | \
	AI_DDL_INIT_EN      (n_DDL_INIT_EN )        | \
	AI_MPLL_INIT_EN     (n_MPLL_INIT_EN  )      | \
	AI_CTCAL_INIT_EN    (n_CTCAL_INIT_EN  )     | \
	AI_RST_SYNC_EN                              | \
	AI_INIT_START       (n_AI_INIT_START_DEF)   | \
	AI_SW_RST_B         (n_AI_SW_RST_RELEASE)   | \
	AI_SW_CTRL          (n_AI_SW_CTRL_DIS)


#define DPCU_AI_CFG0_SELECT2    AI_PZQ_INIT_BYPASS  (n_PZQ_INIT_NO_BYPASS)     | \
	AI_DDL_INIT_BYPASS  (n_DDL_INIT_BYPASS)  | \
	AI_MPLL_INIT_BYPASS (n_MPLL_INIT_BYPASS) | \
	AI_CTCAL_INIT_BYPASS(n_CTCAL_INIT_BYPASS)| \
	AI_PZQ_INIT_EN      (n_PZQ_INIT_EN)         | \
	AI_DDL_INIT_EN      (n_DDL_INIT_DIS )        | \
	AI_MPLL_INIT_EN     (n_MPLL_INIT_DIS  )      | \
	AI_CTCAL_INIT_EN    (n_CTCAL_INIT_DIS  )     | \
	AI_RST_SYNC_EN                              | \
	AI_INIT_START       (n_AI_INIT_START_DEF)   | \
	AI_SW_RST_B         (n_AI_SW_RST_RELEASE)   | \
	AI_SW_CTRL          (n_AI_SW_CTRL_DIS)


// -------------------------------------------------------
// SSCPLL setting
// -------------------------------------------------------
#ifdef          MPEG_DRAM_DDR_2133
#define n_MPLL_DIV  0x4E
#elif defined   MPEG_DRAM_DDR_1866
#define n_MPLL_DIV  0x45
#elif defined   MPEG_DRAM_DDR_1600
#define n_MPLL_DIV  0x3B
#elif defined   MPEG_DRAM_DDR_1333
#define n_MPLL_DIV  0x31
#elif defined   MPEG_DRAM_DDR_1066
#define n_MPLL_DIV  0x27		// 0x27 * 27 = 1053
// #define n_MPLL_DIV  0x26		// 0x26 * 27 = 1026
// #define n_MPLL_DIV  0x25		// 0x25 * 27 = 999
// #define n_MPLL_DIV  0x24		// 0x24 * 27 = 972
// #define n_MPLL_DIV  0x23		// 0x24 * 27 = 945
// #define n_MPLL_DIV  0x22		// 0x22 * 27 = 918
// #define n_MPLL_DIV  0x21		// 0x22 * 27 = 891
#else
// DDR-800
#define n_MPLL_DIV  0x1D
#endif

#ifdef PLATFORM_PENTAGRAM
#define MPLL_CFG1_DEF   0x00495600
#elif defined(PLATFORM_I143)
//#define MPLL_CFG1_DEF   0x00415600
#define MPLL_CFG1_DEF   0x00455600
#endif

#define MPLL_DIV(n)     ((n)<<0)

#define DPCU_MPLL_CFG1  MPLL_CFG1_DEF       | \
	MPLL_DIV(n_MPLL_DIV)


// -------------------------------------------------------
// PZQ setting - G36.18
// -------------------------------------------------------
#define         n_PZQ_ZQ_INTR_EN            0x01
#define         n_PZQ_ZQ_INTR_DIS           0x00

#define         PZQ_REGI_ZQ_INTR(n)         ((n) << 3)

#define         n_PZQ_CK_SEL_EN            0x01
#define         n_PZQ_CK_SEL_DIS           0x00

#define         PZQ_CK_SEL(n)               ((n) << 10)

// this value is for internal PZQ resitor selection (collect value need wait APHY owner get from APHY macro test mode)
#define         n_PZQ_ZQ_RES_SEL            0x9a
#define         PZQ_ZQ_RES_SEL(n)           ((n) << 17)
#define         DPCU_RI_PZQ_RES_SEL_SRC(n)  ((n) << 16)
#ifdef TRIM_INTERNAL_PZQ_FROM_DPCU
#define         n_DPCU_RI_PZQ_RES_SEL_SRC_EN   1
#else
#define         n_DPCU_RI_PZQ_RES_SEL_SRC_EN   0
#endif

#define PZQ_CFG0_DEF    0x00000005

#define DPCU_PZQ_CFG0  PZQ_CFG0_DEF       | \
	PZQ_ZQ_RES_SEL(n_PZQ_ZQ_RES_SEL) | \
	PZQ_CK_SEL(n_PZQ_CK_SEL_EN) | \
	PZQ_REGI_ZQ_INTR(n_PZQ_ZQ_INTR_DIS)



// SSTL impedance setting
// ZPROG[7:4] ODT :  1 => 120 ohm  ;  5 => 60 ohm ; 8 => 40 ohm ;
// ZPROG[3:0] Ro  : 11 => 40 ohm   ; 13 => 34 ohm


#ifdef        SSTL15_RTT120_RO40
#define     DPCU_PZQ_ZPROG               0x1B    // Controller's IO Rtt: 120 Ohm  , Ro=40 Ohm

#elif defined SSTL15_RTT60_RO40
#define     DPCU_PZQ_ZPROG               0x5B    // Controller's IO Rtt:  60 Ohm  , Ro=40 Ohm

#elif defined SSTL15_RTT40_RO40
#define     DPCU_PZQ_ZPROG               0x8B    // Controller's IO Rtt:  40 Ohm  , Ro=40 Ohm

#elif defined SSTL15_RTT120_RO34
#define     DPCU_PZQ_ZPROG               0x1D    // Controller's IO Rtt: 120 Ohm  , Ro=34 Ohm

#elif defined SSTL15_RTT60_RO34
#define     DPCU_PZQ_ZPROG               0x5D    // Controller's IO Rtt:  60 Ohm  , Ro=34 Ohm

#elif defined SSTL15_RTT40_RO34
#define     DPCU_PZQ_ZPROG               0x8D    // Controller's IO Rtt:  40 Ohm  , Ro=34 Ohm

#else
#error  Please define SSTL15 Impedance...

#endif



#define  n_DPCU_PZQ_OVRD_EN      0x01
#define  n_DPCU_PZQ_OVRD_DIS     0x00
#define  DPCU_PZQ_OVRD_ST(n)     ((n) <<  8)

#define  n_DPCU_PZQ_OVRD_DATA     0x00018F39
// #define  n_DPCU_PZQ_OVRD_DATA     ((0x01 << 15) | (0x01 << 10) | (0x0F << 5) | (0x0F << 0))
#define  DPCU_PZQ_OVRD_DATA(n)   ((n) << 9)

#ifdef PLATFORM_I143
#define  n_DPCU_SEL_EXTERNAL_VREF     0x01
#define  n_DPCU_SEL_INTERNAL_VREF     0x00
#define  DPCU_RI_VREF_MUX(n)     ((n) <<  29)

#define DPCU_PZQ_CFG1  DPCU_RI_VREF_MUX(n_DPCU_SEL_EXTERNAL_VREF)    | \
	DPCU_PZQ_OVRD_DATA(n_DPCU_PZQ_OVRD_DATA)    | \
	DPCU_PZQ_OVRD_ST(n_DPCU_PZQ_OVRD_DIS)       | \
	DPCU_PZQ_ZPROG
#else
#define DPCU_PZQ_CFG1  DPCU_PZQ_OVRD_DATA(n_DPCU_PZQ_OVRD_DATA)    | \
	DPCU_PZQ_OVRD_ST(n_DPCU_PZQ_OVRD_DIS)       | \
	DPCU_PZQ_ZPROG
#endif

// -------------------------------------------------------
// DDRIO setting - G50.21
// -------------------------------------------------------
#define n_ALL_DX_DQ_ODT		   0x01
#define ALL_DX_DQ_ODT(n)	   ((n) << 1)

#define n_ALL_DX_DQS_ODT	   0x01
#define ALL_DX_DQS_ODT(n)	   ((n) << 2)

#define n_ALL_DX_DQSR_PD       0x00
#define ALL_DX_DQSR_PD(n)      ((n) << 5)

#define n_DPCU_DX_IO_ODT_SEL   0x01
#define DPCU_DX_IO_ODT_SEL(n)  ((n) << 6)

#define n_DX_DQS_RES           0x01
#define DX_DQS_RES(n)          ((n) << 8)

#define n_DX_DQSN_RES          0x09
#define DX_DQSN_RES(n)         ((n) << 12)

#define n_DPCU_RI_DX_ODT_SEL   0x01
#define DPCU_RI_DX_ODT_SEL(n)  ((n) << 16)

#define n_DPCU_RI_DX_RTT_SEL   0x00
#define DPCU_RI_DX_RTT_SEL(n)  ((n) << 17)

#define DPCU_DDRIO_CFG3  ALL_DX_DQ_ODT(n_ALL_DX_DQ_ODT) | \
	ALL_DX_DQS_ODT(n_ALL_DX_DQS_ODT) | \
	ALL_DX_DQSR_PD(n_ALL_DX_DQSR_PD) |\
	DPCU_DX_IO_ODT_SEL(n_DPCU_DX_IO_ODT_SEL) |\
	DX_DQS_RES(n_DX_DQS_RES)   | \
	DX_DQSN_RES(n_DX_DQSN_RES) | \
	DPCU_RI_DX_ODT_SEL(n_DPCU_RI_DX_ODT_SEL) | \
	DPCU_RI_DX_RTT_SEL(n_DPCU_RI_DX_RTT_SEL)


// -------------------------------------------------------
// CMD_ISSUE setting
// -------------------------------------------------------
#define CI_MRS_0_VAL_SET    MRS_MODE0_VAL_SET
#define CI_MRS_1_VAL_SET    MRS_MODE1_VAL_SET
#define CI_MRS_2_VAL_SET    MRS_MODE2_VAL_SET
#define CI_MRS_3_VAL_SET    MRS_MODE3_VAL_SET

// -------------------------------------------------------
// DPCU Data Training
// -------------------------------------------------------
// APHY AC,ACK,CK BDD Setting
// CK BDD setting
// add for socket and direct PCB can using the same RGST setting in SDCTRL in 2014.0719
#define DPCU_CK0BD                       0x01
#define DPCU_AC0BD                       0x00
#define DPCU_ACK0BD                      0x00
#define DPCU_CMD_BDD_SETTING            (DPCU_ACK0BD<<16) | (DPCU_AC0BD << 8) | (DPCU_CK0BD << 0)


// DT_CFG0 - DPCU Data Training global config - 0
#define n_DT_START          1
#define n_DT_DIS            0
#define DPCU_TRAIN_START(n) ((n)<<0)

#define n_WL_EN             1
#define n_WL_DIS            0
#define DT_WL(n)            ((n)<<1)

#define n_RG_EN             1
#define n_RG_DIS            0
#define DT_RG(n)            ((n)<<2)

#define n_W_DESKEW_EN       1
#define n_W_DESKEW_DIS      0
#define DT_WR_DESKEW(n)     ((n)<<3)

#define n_R_DESKEW_EN       1
#define n_R_DESKEW_DIS      0
#define DT_RD_DESKEW(n)     ((n)<<4)

#define n_W_EYE_EN          1
#define n_W_EYE_DIS         0
#define DT_WR_EYE(n)        ((n)<<5)

#define n_R_EYE_EN          1
#define n_R_EYE_DIS         0
#define DT_RD_EYE(n)        ((n)<<6)

#define n_CWL_EN            1
#define n_CWL_DIS           0
#define DT_CWL(n)           ((n)<<7)

#define n_DM_EN             1
#define n_DM_DIS            0
#define DT_DM(n)            ((n)<<26)


// #define DT_AREF_EN          (1<<7)
#ifdef SDRAM0_SIZE_4Gb
#ifdef PLATFORM_PENTAGRAM
#define DT_AREF_PRD         (1400<<8)  // take TRFC value // [str] decrease training auto refresh interval
#elif defined PLATFORM_I143
#define DT_AREF_PRD         (1560<<8)
#endif
#else
#define DT_AREF_PRD         (1560<<8)  // take TRFC value // [str] decrease training auto refresh interval
#endif
#ifdef CONFIG_DRAM_SIZE_USE_OTP
#define DT_AREF_PRD_4Gb     (1400<<8)  // take TRFC value // [str] decrease training auto refresh interval
#define DT_AREF_PRD_1Gb     (1560<<8)  // take TRFC value // [str] decrease training auto refresh interval
#define DT_AREF_PRD_512Mb   (1560<<8)  // take TRFC value // [str] decrease training auto refresh interval
#endif
#ifdef PLATFORM_PENTAGRAM
#define DPCU_DT_CFG0        DT_AREF_PRD                     | \
	DT_RD_EYE(n_R_EYE_DIS)          | \
	DT_WR_EYE(n_W_EYE_DIS)          | \
	DT_RD_DESKEW(n_R_DESKEW_DIS)    | \
	DT_WR_DESKEW(n_W_DESKEW_DIS)    | \
	DT_RG(n_RG_DIS)                 | \
	DT_WL(n_WL_DIS)                 | \
	DT_CWL(n_WL_EN)                 | \
	DPCU_TRAIN_START(n_DT_DIS)

#define DPCU_DT_GO          DT_AREF_PRD                     | \
	DT_RD_EYE(n_R_EYE_EN)           | \
	DT_WR_EYE(n_W_EYE_EN)           | \
	DT_RD_DESKEW(n_R_DESKEW_EN)     | \
	DT_WR_DESKEW(n_W_DESKEW_EN)     | \
	DT_RG(n_RG_EN)                  | \
	DT_WL(n_WL_EN)                  | \
	DT_CWL(n_WL_EN)                 | \
	DPCU_TRAIN_START(n_DT_START)
#elif defined PLATFORM_I143
#define DPCU_DT_CFG0        DT_DM(n_DM_DIS)                | \
  DT_AREF_PRD                     | \
	DT_RD_EYE(n_R_EYE_DIS)          | \
	DT_WR_EYE(n_W_EYE_DIS)          | \
	DT_RD_DESKEW(n_R_DESKEW_DIS)    | \
	DT_WR_DESKEW(n_W_DESKEW_DIS)    | \
	DT_RG(n_RG_DIS)                 | \
	DT_WL(n_WL_DIS)                 | \
	DT_CWL(n_WL_EN)                 | \
	DPCU_TRAIN_START(n_DT_DIS)

#define DPCU_DT_GO          DT_DM(n_DM_EN)                 | \
  DT_AREF_PRD                     | \
	DT_RD_EYE(n_R_EYE_EN)           | \
	DT_WR_EYE(n_W_EYE_EN)           | \
	DT_RD_DESKEW(n_R_DESKEW_EN)     | \
	DT_WR_DESKEW(n_W_DESKEW_EN)     | \
	DT_RG(n_RG_EN)                  | \
	DT_WL(n_WL_EN)                  | \
	DT_CWL(n_WL_EN)                 | \
	DPCU_TRAIN_START(n_DT_START)
#endif
// DT_CFG4 - DPCU Data Training global config - 4
#define n_DT_RD_MPR_EN      1
#define n_DT_RD_MPR_DIS     0
#define DT_RD_MPR(n)        ((n) << 16)

#define n_DT_RG_LINEAR_EN      1
#define n_DT_RG_LINEAR_DIS     0
#define DT_RG_LINEAR(n)        ((n) << 3)


// Data training manual patterns

#define n_BANK_SEL_0    0x00
#define n_BANK_SEL_1    0x01
#define n_BANK_SEL_2    0x02
#define n_BANK_SEL_3    0x03
#define DT_PAT_BANK_SLE(n) ((n)<<24)

#define n_DEFAULT_TYPE  0x00
#define n_RDQSG_TYPE    0x01
#define n_DESKEW_TYPE   0x02
#define n_EYE_TYPE      0x03
#define DT_PAT_TYPE_SEL(n)   ((n) << 27)

#define DT_PAT0_DESKEW_3_0 0x11112222
#define DT_PAT0_DESKEW_7_4 0xAAAABBBB
#define DT_PAT1_DESKEW_3_0 0x33334444
#define DT_PAT1_DESKEW_7_4 0xCCCCDDDD
#define DT_PAT2_DESKEW_3_0 0x55556666
#define DT_PAT2_DESKEW_7_4 0xEEEEFFFF
#define DT_PAT3_DESKEW_3_0 0x77778888
#define DT_PAT3_DESKEW_7_4 0x9999AAAA

#define DT_PAT0_EYE_3_0 0x01234567
#define DT_PAT0_EYE_7_4 0x89ABCDEF
#define DT_PAT1_EYE_3_0 0xCDCDABAB
#define DT_PAT1_EYE_7_4 0xEFEF1212
#define DT_PAT2_EYE_3_0 0x01012323
#define DT_PAT2_EYE_7_4 0x45456767
#define DT_PAT3_EYE_3_0 0x8989ABAB
#define DT_PAT3_EYE_7_4 0xCDCDEFEF
// DRAM0 0x00000000
#define DT_BANK_DRAM0     0x00
#define DT_ROW_DRAM0      0x00
#define DT_COL_DRAM0      0x00
#define DT_DRAM0_ADDRESS  (DT_COL_DRAM0<<19) | (DT_ROW_DRAM0<<3) | (DT_BANK_DRAM0<<0)
// DRAM1 0x20000000
#define DT_BANL_DRAM1     0x00
#define DT_ROW_DRAM1      0x00
#define DT_COL_DRAM1      0x00
#define DT_DRAM1_ADDRESS  (DT_COL_DRAM1<<19) | (DT_ROW_DRAM1<<3) | (DT_BANK_DRAM1<<0)
// deskew pattern
// 1. Pattern type select
// 2. Pattern bank select
// 3. Pattern address select
// 4. Pattern select

#define DESKEW_TYPE_BANK_SEL = DT_PAT_TYPE_SEL(n_DESKEW_TYPE)

#ifdef PLATFORM_PENTAGRAM
// Add for New UMCTL2
        #define UMCTL2_0_1      0
        #define UMCTL2_0_2      4
        #define UMCTL2_0_3      SDCTRL_MRS_DLL_MODE
        #ifdef  MPEG_DRAM_TYPE_DDR3
                #define UMCTL2_0_4      1
        #else
                #define UMCTL2_0_4      0
        #endif
        #define UMCTL2_0        ((UMCTL2_0_1)   << 22 |\
                                 (UMCTL2_0_2)   << 16 |\
                                 (UMCTL2_0_3)   << 15 |\
                                 (UMCTL2_0_4)   <<  0 )

        #define UMCTL2_10       0x0000f010

        #define UMCTL2_14       0x0000c067

        #define UMCTL2_30_1     1
        #define UMCTL2_30_2     0
        #define UMCTL2_30(n)    ((n) << 0)

        #define UMCTL2_34_1     64
        #define UMCTL2_34_2     4
        #define UMCTL2_34       ((UMCTL2_34_1)  << 16 |\
                                 (UMCTL2_34_2)  <<  0 )

        #define UMCTL2_38_1             99
        #define UMCTL2_38_2             0
        #define UMCTL2_38_3             0
        #define UMCTL2_38               ((UMCTL2_38_1)  << 16 |\
                                         (UMCTL2_38_2)  <<  1 |\
                                         (UMCTL2_38_3)  <<  0 )

        #define UMCTL2_50_1     2
        #define UMCTL2_50_2     32
        #define UMCTL2_50_3     0
        #define UMCTL2_50_4     0
        #define UMCTL2_50       ((UMCTL2_50_1)  << 20 |\
                                 (UMCTL2_50_2)  << 12 |\
                                 (UMCTL2_50_3)  <<  4 |\
                                 (UMCTL2_50_4)  <<  2 )

        #define UMCTL2_60_1     0
        #define UMCTL2_60_2     0
        #define UMCTL2_60_3     0
        #define UMCTL2_60_4     0
        #define UMCTL2_60       ((UMCTL2_60_1)  << 16 |\
                                 (UMCTL2_60_2)  <<  4 |\
                                 (UMCTL2_60_3)  <<  1 |\
                                 (UMCTL2_60_4)  <<  0 )

        #define UMCTL2_64_1     0
        #define UMCTL2_64_2     nAREF_INTVAL
        #define UMCTL2_64_3     0
        #define UMCTL2_64_4     ((n_tRFC + 1) >> 1)
        #define UMCTL2_64       ((UMCTL2_64_1)  << 31 |\
                                 (UMCTL2_64_2)  << 16 |\
                                 (UMCTL2_64_3)  << 15 |\
                                 (UMCTL2_64_4)  <<  0 )
#ifdef CONFIG_DRAM_SIZE_USE_OTP
        #define UMCTL2_64_512Mb  ((UMCTL2_64_1)  << 31 |\
                                 (UMCTL2_64_2)  << 16 |\
                                 (UMCTL2_64_3)  << 15 |\
                                 ((n_tRFC_512Mb+ 1) >> 1)  <<  0 )
        #define UMCTL2_64_1Gb    ((UMCTL2_64_1)  << 31 |\
                                 (UMCTL2_64_2)  << 16 |\
                                 (UMCTL2_64_3)  << 15 |\
                                 ((n_tRFC_1Gb + 1) >> 1)  <<  0 )
        #define UMCTL2_64_4Gb    ((UMCTL2_64_1)  << 31 |\
                                 (UMCTL2_64_2)  << 16 |\
                                 (UMCTL2_64_3)  << 15 |\
                                 ((n_tRFC_4Gb + 1) >> 1)  <<  0 )
#endif
        #define UMCTL2_C0_1     0
        #define UMCTL2_C0_2     0
        #define UMCTL2_C0_3     0
        #define UMCTL2_C0_4     0
        #define UMCTL2_C0_5     0
        #define UMCTL2_C0_6     0
        #define UMCTL2_C0_7     0
        #define UMCTL2_C0       ((UMCTL2_C0_1)  << 16 |\
                                 (UMCTL2_C0_2)  << 15 |\
                                 (UMCTL2_C0_3)  <<  8 |\
                                 (UMCTL2_C0_4)  <<  4 |\
                                 (UMCTL2_C0_5)  <<  2 |\
                                 (UMCTL2_C0_6)  <<  1 |\
                                 (UMCTL2_C0_7)  <<  0 )

        #define UMCTL2_D0_1     0
        #define UMCTL2_D0_2     2
        #define UMCTL2_D0_3     2
        #define UMCTL2_D0       ((UMCTL2_D0_1)  << 30 |\
                                 (UMCTL2_D0_2)  << 16 |\
                                 (UMCTL2_D0_3)  <<  0 )

        #define UMCTL2_D4_1     1
        #define UMCTL2_D4_2     2
        #define UMCTL2_D4       ((UMCTL2_D4_1)  << 16 |\
                                 (UMCTL2_D4_2)  <<  0 )

        #define UMCTL2_DC_1     MRS_DLL_ON
        #define UMCTL2_DC_2     MRS_WRITE_RECOVERY
        #define UMCTL2_DC_3     MRS_DLL_RESET
        #define UMCTL2_DC_4     0
        #define UMCTL2_DC_5     MRS_CAS_LATENCY
        #define UMCTL2_DC_6     MRS_CL_M2
        #define UMCTL2_DC_7     0
        #define UMCTL2_DC_8     DDR3_BURST_LEN
        #define UMCTL2_DC_9     SDCTRL_MRS_QOFF
        #define UMCTL2_DC_10    SDCTRL_MRS_TDQS
        #define UMCTL2_DC_11    SDCTRL_MRS_RTT_2
        #define UMCTL2_DC_12    SDCTRL_MRS_RTT_1
        #define UMCTL2_DC_13    SDCTRL_MRS_RTT_0
        #define UMCTL2_DC_14    SDCTRL_MRS_WL
        #define UMCTL2_DC_15    SDCTRL_MRS_ODS_1
        #define UMCTL2_DC_16    SDCTRL_MRS_ODS_0
        #define UMCTL2_DC_17    SDCTRL_MRS_AL(0)
        #define UMCTL2_DC_18    SDCTRL_MRS_DLL_MODE
        #define UMCTL2_DC       ((UMCTL2_DC_1)  << 16 |\
                                 (UMCTL2_DC_2)  << 16 |\
                                 (UMCTL2_DC_3)  << 16 |\
                                 (UMCTL2_DC_4)  << 23 |\
                                 (UMCTL2_DC_5)  << 16 |\
                                 (UMCTL2_DC_7)  << 19 |\
                                 (UMCTL2_DC_6)  << 16 |\
                                 (UMCTL2_DC_8)  << 16 |\
                                 (UMCTL2_DC_9)  <<  0 |\
                                 (UMCTL2_DC_10) <<  0 |\
                                 (UMCTL2_DC_11) <<  0 |\
                                 (UMCTL2_DC_14) <<  0 |\
                                 (UMCTL2_DC_12) <<  0 |\
                                 (UMCTL2_DC_15) <<  0 |\
                                 (UMCTL2_DC_17) <<  0 |\
                                 (UMCTL2_DC_13) <<  0 |\
                                 (UMCTL2_DC_16) <<  0 |\
                                 (UMCTL2_DC_18) <<  0 )

        #define UMCTL2_E0_1     SDCTRL_MRS_RTT_WR
        #define UMCTL2_E0_2     SDCTRL_MRS_SRT
        #define UMCTL2_E0_3     SDCTRL_MRS_ASR
        #define UMCTL2_E0_4     SDCTRL_MRS_CWL
        #define UMCTL2_E0_5     0
        #define UMCTL2_E0_6     0
        #define UMCTL2_E0_7     0
        #define UMCTL2_E0       ((UMCTL2_E0_1)  << 16 |\
                                 (UMCTL2_E0_2)  << 16 |\
                                 (UMCTL2_E0_3)  << 16 |\
                                 (UMCTL2_E0_4)  << 16 |\
                                 (UMCTL2_E0_5)  << 16 |\
                                 (UMCTL2_E0_6)  <<  0 |\
                                 (UMCTL2_E0_7)  <<  0 )

        #define UMCTL2_E4_1     9
        #define UMCTL2_E4_2     0
        #define UMCTL2_E4       ((UMCTL2_E4_1)  << 16 |\
                                 (UMCTL2_E4_2)  <<  0 )

        #define UMCTL2_F0_1     0
        #define UMCTL2_F0_2     0
        #define UMCTL2_F0_3     0
        #define UMCTL2_F0_4     0
        #define UMCTL2_F0_5     0
        #define UMCTL2_F0_6     0
        #define UMCTL2_F0_7     0
        #define UMCTL2_F0_8     0
        #define UMCTL2_F0_9     0
        #define UMCTL2_F0_10    0
        #define UMCTL2_F0_11    0
        #define UMCTL2_F0       ((UMCTL2_F0_1)  << 14 |\
                                 (UMCTL2_F0_2)  << 13 |\
                                 (UMCTL2_F0_3)  << 12 |\
                                 (UMCTL2_F0_4)  <<  8 |\
                                 (UMCTL2_F0_5)  <<  6 |\
                                 (UMCTL2_F0_6)  <<  5 |\
                                 (UMCTL2_F0_7)  <<  4 |\
                                 (UMCTL2_F0_8)  <<  3 |\
                                 (UMCTL2_F0_9)  <<  2 |\
                                 (UMCTL2_F0_10) <<  1 |\
                                 (UMCTL2_F0_11) <<  0 )

        #define UMCTL2_100_1    ((n_tCL + 4 + n_tWR + 1) >> 1)
        #define UMCTL2_100_2    ((n_tFAW + 1) >> 1)
        #define UMCTL2_100_3    37440
        #define UMCTL2_100_4    (((UMCTL2_100_3 >> 10) -1) >> 1)
        #define UMCTL2_100_5    ((n_tRAS + 1) >> 1)
        #define UMCTL2_100      ((UMCTL2_100_1) << 24 |\
                                 (UMCTL2_100_2) << 16 |\
                                 (UMCTL2_100_4) <<  8 |\
                                 (UMCTL2_100_5) <<  0 )

        #define UMCTL2_104_1    2
        #define UMCTL2_104_2    ((nRTP + 1) >> 1)
        #define UMCTL2_104_3    ((n_tRC + 1) >> 1)
        #define UMCTL2_104      ((UMCTL2_104_1) << 16 |\
                                 (UMCTL2_104_2) <<  8 |\
                                 (UMCTL2_104_3) <<  0 )

        #define UMCTL2_108_1    2
        #define UMCTL2_108_2    3
        #define UMCTL2_108_3    (((n_tCL + 4 + 2 - MRS_CWL_CNT) + 1) >> 1)
        #define UMCTL2_108_4    (((MRS_CWL_CNT + 4 + n_tWTR) +1) >> 1)
        #define UMCTL2_108      ((UMCTL2_108_1) << 24 |\
                                 (UMCTL2_108_2) << 16 |\
                                 (UMCTL2_108_3) <<  8 |\
                                 (UMCTL2_108_4) <<  0 )

        #define UMCTL2_10C_1    0
        #define UMCTL2_10C_2    ((n_tMRD + 1) >> 1)
        #define UMCTL2_10C_3    ((n_tMOD + 1) >> 1)
        #define UMCTL2_10C      ((UMCTL2_10C_1) << 20 |\
                                 (UMCTL2_10C_2) << 12 |\
                                 (UMCTL2_10C_3) <<  0 )

        #define UMCTL2_110_1    ((n_tRCD + 1) >> 1)
        #define UMCTL2_110_2    ((n_tCCD + 1) >> 1)
        #define UMCTL2_110_3    ((n_tRRD + 1) >> 1)
        #define UMCTL2_110_4    (((n_tRP + 1) >> 1) + 1)
        #define UMCTL2_110      ((UMCTL2_110_1) << 24 |\
                                 (UMCTL2_110_2) << 16 |\
                                 (UMCTL2_110_3) <<  8 |\
                                 (UMCTL2_110_4) <<  0 )

        #define UMCTL2_114_1    3
        #define UMCTL2_114_2    3
        #define UMCTL2_114_3    2
        #define UMCTL2_114_4    2
        #define UMCTL2_114      ((UMCTL2_114_1) << 24 |\
                                 (UMCTL2_114_2) << 16 |\
                                 (UMCTL2_114_3) <<  8 |\
                                 (UMCTL2_114_4) <<  0 )

        #define UMCTL2_120_1    0
        #define UMCTL2_120_2    0
        #define UMCTL2_120_3    10
        #define UMCTL2_120_4    3
        #define UMCTL2_120      ((UMCTL2_120_1) << 24 |\
                                 (UMCTL2_120_2) << 16 |\
                                 (UMCTL2_120_3) <<  8 |\
                                 (UMCTL2_120_4) <<  0 )

        #define UMCTL2_13C_1    0
        #define UMCTL2_13C_2    0
        #define UMCTL2_13C_3    0
        #define UMCTL2_13C      ((UMCTL2_13C_1) << 31 |\
                                 (UMCTL2_13C_2) << 24 |\
                                 (UMCTL2_13C_3) <<  0 )

        #define UMCTL2_180_1    0
        #define UMCTL2_180_2    0
        #define UMCTL2_180_3    0
        #define UMCTL2_180_4    0
        #define UMCTL2_180_5    128
        #define UMCTL2_180_6    32
        #define UMCTL2_180      ((UMCTL2_180_1) << 31 |\
                                 (UMCTL2_180_2) << 30 |\
                                 (UMCTL2_180_3) << 29 |\
                                 (UMCTL2_180_4) << 28 |\
                                 (UMCTL2_180_5) << 16 |\
                                 (UMCTL2_180_6) <<  0 )

        #define UMCTL2_184_1    0
        #define UMCTL2_184_2    857089
        #define UMCTL2_184      ((UMCTL2_184_1) << 20 |\
                                 (UMCTL2_184_2) <<  0 )

        #define UMCTL2_190_1    3
        #define UMCTL2_190_2    1
#if 1
#define UMCTL2_190_3    10
#else
        #ifdef NO_QUICK_SIM
                #ifdef  POST_SIM
                        #define UMCTL2_190_3    10
                #else
                        #define UMCTL2_190_3    9
                #endif
        #else
                #define UMCTL2_190_3    9
        #endif
#endif

        #define UMCTL2_190_4    1
        #define UMCTL2_190_5    1
        #define UMCTL2_190_6    5
        #define UMCTL2_190      ((UMCTL2_190_1) << 24 |\
                                 (UMCTL2_190_2) << 23 |\
                                 (UMCTL2_190_3) << 16 |\
                                 (UMCTL2_190_4) << 15 |\
                                 (UMCTL2_190_5) <<  8 |\
                                 (UMCTL2_190_6) <<  0 )

        #define UMCTL2_194_1    0
        #define UMCTL2_194_2    0
        #define UMCTL2_194_3    6
        #define UMCTL2_194_4    1
        #define UMCTL2_194_5    1
        #define UMCTL2_194      ((UMCTL2_194_1) << 28 |\
                                 (UMCTL2_194_2) << 24 |\
                                 (UMCTL2_194_3) << 16 |\
                                 (UMCTL2_194_4) <<  8 |\
                                 (UMCTL2_194_5) <<  0 )

        #define UMCTL2_198_1    7
        #define UMCTL2_198_2    0
        #define UMCTL2_198_3    0
        #define UMCTL2_198_4    3
        #define UMCTL2_198_5    0
        #define UMCTL2_198_6    1
        #define UMCTL2_198_7    1
        #define UMCTL2_198      ((UMCTL2_198_1) << 24 |\
                                 (UMCTL2_198_2) << 20 |\
                                 (UMCTL2_198_3) << 16 |\
                                 (UMCTL2_198_4) << 12 |\
                                 (UMCTL2_198_5) <<  8 |\
                                 (UMCTL2_198_6) <<  4 |\
                                 (UMCTL2_198_7) <<  0 )

        #define UMCTL2_1A0_1    1
        #define UMCTL2_1A0_2    1
        #define UMCTL2_1A0_3    1
        #define UMCTL2_1A0_4    64
        #define UMCTL2_1A0_5    5
        #define UMCTL2_1A0      ((UMCTL2_1A0_1) << 31 |\
                                 (UMCTL2_1A0_2) << 30 |\
                                 (UMCTL2_1A0_3) << 29 |\
                                 (UMCTL2_1A0_4) << 16 |\
                                 (UMCTL2_1A0_5) <<  0 )

        #define UMCTL2_1A4_1    1
        #define UMCTL2_1A4_2    2
        #define UMCTL2_1A4      ((UMCTL2_1A4_1) << 16 |\
                                 (UMCTL2_1A4_2) <<  0 )

        #define UMCTL2_1A8_1    0
        #define UMCTL2_1A8      ((UMCTL2_1A8_1) << 31 )

        #define UMCTL2_1B0_4    0
        #define UMCTL2_1B0_5    0
        #define UMCTL2_1B0_6    0
        #define UMCTL2_1B0_7    0
        #define UMCTL2_1B0_8    1
        #define UMCTL2_1B0_9    0
        #define UMCTL2_1B0_10   0
        #define UMCTL2_1B0_11   0
        #define UMCTL2_1B0_12   0
        #define UMCTL2_1B0_13   1
        #define UMCTL2_1B0_14   1
        #define UMCTL2_1B0_15   0
        #define UMCTL2_1B0_1    ((UMCTL2_1B0_8)         << 4 |\
                                 (UMCTL2_1B0_10)        << 3 |\
                                 (UMCTL2_1B0_11)        << 2 |\
                                 (UMCTL2_1B0_12)        << 1 |\
                                 (UMCTL2_1B0_14)        << 0 )
        #define UMCTL2_1B0_2    ((UMCTL2_1B0_8)         << 4 |\
                                 (UMCTL2_1B0_10)        << 3 |\
                                 (UMCTL2_1B0_11)        << 2 |\
                                 (UMCTL2_1B0_12)        << 1 |\
                                 (UMCTL2_1B0_14)        << 0 )
        #define UMCTL2_1B0_3    ((UMCTL2_1B0_8)         << 4 |\
                                 (UMCTL2_1B0_10)        << 3 |\
                                 (UMCTL2_1B0_11)        << 2 |\
                                 (UMCTL2_1B0_12)        << 1 |\
                                 (UMCTL2_1B0_14)        << 0 )
        #define UMCTL2_1B0(n)   ((UMCTL2_1B0_4)         << 8 |\
                                 (UMCTL2_1B0_5)         << 7 |\
                                 (UMCTL2_1B0_6)         << 6 |\
                                 (UMCTL2_1B0_7)         << 5 |\
                                 (n)                    << 0 )

        #define UMCTL2_1B4_1    4
        #define UMCTL2_1B4_2    1
        #define UMCTL2_1B4      ((UMCTL2_1B4_1) << 8 |\
                                 (UMCTL2_1B4_2) << 0 )

        #define UMCTL2_1C4_1    0
        #define UMCTL2_1C4      ((UMCTL2_1C4_1) << 0 )

        #ifdef  UMCTL2_BA_NO_2
                #define UMCTL2_204_1    (nCOL_WIDTH - 1)
                #define UMCTL2_204_2    (nCOL_WIDTH - 1)
                #define UMCTL2_204_3    (nCOL_WIDTH - 1)
        #endif
        #ifdef  UMCTL2_BA_NO_1
                #define UMCTL2_204_1    63
                #define UMCTL2_204_2    (nCOL_WIDTH - 1)
                #define UMCTL2_204_3    (nCOL_WIDTH - 1)
        #endif
        #ifdef  UMCTL2_BA_NO_0
                #define UMCTL2_204_1    63
                #define UMCTL2_204_2    63
                #define UMCTL2_204_3    (nCOL_WIDTH - 1)
        #endif
        #define UMCTL2_204      ((UMCTL2_204_1) << 16 |\
                                 (UMCTL2_204_2) <<  8 |\
                                 (UMCTL2_204_3) <<  0 )

        #ifdef  UMCTL2_CA_NO_11
                #define UMCTL2_210_1    0
                #define UMCTL2_210_2    0
                #define UMCTL2_20C_1    0
                #define UMCTL2_20C_2    0
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_10
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    0
                #define UMCTL2_20C_1    0
                #define UMCTL2_20C_2    0
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_9
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    0
                #define UMCTL2_20C_2    0
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_8
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    0
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_7
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_6
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_5
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_4
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    15
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_3
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    15
                #define UMCTL2_208_2    15
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_2
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    15
                #define UMCTL2_208_2    15
                #define UMCTL2_208_3    15
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_1
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    15
                #define UMCTL2_208_2    15
                #define UMCTL2_208_3    15
                #define UMCTL2_208_4    15
        #endif
        #define UMCTL2_208      ((UMCTL2_208_1) << 24 |\
                                 (UMCTL2_208_2) << 16 |\
                                 (UMCTL2_208_3) <<  8 |\
                                 (UMCTL2_208_4) <<  0 )

        #define UMCTL2_20C      ((UMCTL2_20C_1) << 24 |\
                                 (UMCTL2_20C_2) << 16 |\
                                 (UMCTL2_20C_3) <<  8 |\
                                 (UMCTL2_20C_4) <<  0 )

        #define UMCTL2_210_3    0
        #define UMCTL2_210      ((UMCTL2_210_3) << 31 |\
                                 (UMCTL2_210_1) <<  8 |\
                                 (UMCTL2_210_2) <<  0 )

        #ifdef  UMCTL2_RA_NO_17
                #define UMCTL2_218_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_16
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_15
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_14
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_13
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_12
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_11
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_10
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_9
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_8
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_7
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_6
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_5
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_4
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_3
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    15
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_2
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    15
                #define UMCTL2_224_3    15
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_1
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    15
                #define UMCTL2_224_3    15
                #define UMCTL2_224_4    15
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_0
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    15
                #define UMCTL2_224_3    15
                #define UMCTL2_224_4    15
                #define UMCTL2_214_2    15
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #define UMCTL2_214_4    15
        #define UMCTL2_214      ((UMCTL2_214_1) << 24 |\
                                 (UMCTL2_214_4) << 16 |\
                                 (UMCTL2_214_2) <<  8 |\
                                 (UMCTL2_214_3) <<  0 )

        #define UMCTL2_218_7    0
        #define UMCTL2_218_8    0
        #define UMCTL2_218      ((UMCTL2_218_7) << 31 |\
                                 (UMCTL2_218_8) << 29 |\
                                 (UMCTL2_218_3) << 24 |\
                                 (UMCTL2_218_4) << 16 |\
                                 (UMCTL2_218_5) <<  8 |\
                                 (UMCTL2_218_6) <<  0 )
#ifdef CONFIG_DRAM_SIZE_USE_OTP
        #define UMCTL2_218_512Mb ((UMCTL2_218_7) << 31 |\
                                 (UMCTL2_218_8) << 29 |\
                                 (15) << 24 |\
                                 (15) << 16 |\
                                 (15) <<  8 |\
                                 (15) <<  0 )
        #define UMCTL2_218_1Gb   ((UMCTL2_218_7) << 31 |\
                                 (UMCTL2_218_8) << 29 |\
                                 (15) << 24 |\
                                 (15) << 16 |\
                                 (15) <<  8 |\
                                 (nBANK_WIDTH + nCOL_WIDTH - 4) <<  0 )

        #define UMCTL2_218_4Gb   ((UMCTL2_218_7) << 31 |\
                                 (UMCTL2_218_8) << 29 |\
                                 (15) << 24 |\
                                 (nBANK_WIDTH + nCOL_WIDTH - 4) << 16 |\
                                 (nBANK_WIDTH + nCOL_WIDTH - 4) <<  8 |\
                                 (nBANK_WIDTH + nCOL_WIDTH - 4) <<  0 )

#endif
        #define UMCTL2_224      ((UMCTL2_224_1) << 24 |\
                                 (UMCTL2_224_2) << 16 |\
                                 (UMCTL2_224_3) <<  8 |\
                                 (UMCTL2_224_4) <<  0 )

        #define UMCTL2_228      ((UMCTL2_228_1) << 24 |\
                                 (UMCTL2_228_2) << 16 |\
                                 (UMCTL2_228_3) <<  8 |\
                                 (UMCTL2_228_4) <<  0 )

        #define UMCTL2_22C_2    0
        #define UMCTL2_22C_3    0
        #define UMCTL2_22C      ((UMCTL2_22C_2) << 16 |\
                                 (UMCTL2_22C_3) <<  8 |\
                                 (UMCTL2_22C_1) <<  0 )

        #define UMCTL2_240_1    6
        #define UMCTL2_240_2    0
        #define UMCTL2_240_3    6
        #define UMCTL2_240_4    (n_tCL - MRS_CWL_CNT)
        #define UMCTL2_240      ((UMCTL2_240_1) << 24 |\
                                 (UMCTL2_240_2) << 16 |\
                                 (UMCTL2_240_3) <<  8 |\
                                 (UMCTL2_240_4) <<  2 )

        #define UMCTL2_244_1    0
        #define UMCTL2_244_2    0
        #define UMCTL2_244_3    0
        #define UMCTL2_244_4    0
        #define UMCTL2_244_5    0
        #define UMCTL2_244_6    0
        #define UMCTL2_244_7    0
        #define UMCTL2_244_8    1
        #define UMCTL2_244      ((UMCTL2_244_1) << 28 |\
                                 (UMCTL2_244_2) << 24 |\
                                 (UMCTL2_244_3) << 20 |\
                                 (UMCTL2_244_4) << 16 |\
                                 (UMCTL2_244_5) << 12 |\
                                 (UMCTL2_244_6) <<  8 |\
                                 (UMCTL2_244_7) <<  4 |\
                                 (UMCTL2_244_8) <<  0 )

        #define UMCTL2_250_1    0
        #define UMCTL2_250_2    55
        #define UMCTL2_250_3    63
        #define UMCTL2_250_4    0
        #define UMCTL2_250_5    0
        #define UMCTL2_250_6    1
        #define UMCTL2_250      ((UMCTL2_250_1) << 24 |\
                                 (UMCTL2_250_2) << 16 |\
                                 (UMCTL2_250_3) <<  8 |\
                                 (UMCTL2_250_4) <<  2 |\
                                 (UMCTL2_250_5) <<  1 |\
                                 (UMCTL2_250_6) <<  0 )

        #define UMCTL2_254_1    0
        #define UMCTL2_254      ((UMCTL2_254_1) << 0 )

        #define UMCTL2_25C_1    15
        #define UMCTL2_25C_2    1
        #define UMCTL2_25C      ((UMCTL2_25C_1) << 24 |\
                                 (UMCTL2_25C_2) <<  0 )

        #define UMCTL2_264_1    128
        #define UMCTL2_264_2    1
        #define UMCTL2_264      ((UMCTL2_264_1) << 24 |\
                                 (UMCTL2_264_2) <<  0 )

        #define UMCTL2_26C_1    8
        #define UMCTL2_26C_2    2048
        #define UMCTL2_26C      ((UMCTL2_26C_1) << 24 |\
                                 (UMCTL2_26C_2) <<  0 )

        #define UMCTL2_300_1    0
        #define UMCTL2_300_2    0
        #define UMCTL2_300_3    0
        #define UMCTL2_300_4    0
        #define UMCTL2_300      ((UMCTL2_300_1) << 4 |\
                                 (UMCTL2_300_2) << 2 |\
                                 (UMCTL2_300_3) << 1 |\
                                 (UMCTL2_300_4) << 0 )

        #define UMCTL2_304_3    0
        #define UMCTL2_304_1    1
        #define UMCTL2_304_2    0
        #define UMCTL2_304(n)   ((UMCTL2_304_3) << 1 |\
                                 (n)            << 0 )

        #define UMCTL2_30C_1    0
        #define UMCTL2_30C_2    0
        #define UMCTL2_30C_3    0
        #define UMCTL2_30C_4    0
        #define UMCTL2_30C_5    0
        #define UMCTL2_30C_6    0
        #define UMCTL2_30C_7    0
        #define UMCTL2_30C_8    0
        #define UMCTL2_30C_9    0
        #define UMCTL2_30C_10   0
        #define UMCTL2_30C_11   0
        #define UMCTL2_30C_12   0
        #define UMCTL2_30C_13   0
        #define UMCTL2_30C_14   0
        #define UMCTL2_30C_15   0
        #define UMCTL2_30C_16   0
        #define UMCTL2_30C_17   0
        #define UMCTL2_30C_18   0
        #define UMCTL2_30C      ((UMCTL2_30C_1)         << 19 |\
                                 (UMCTL2_30C_2)         << 18 |\
                                 (UMCTL2_30C_3)         << 17 |\
                                 (UMCTL2_30C_4)         << 16 |\
                                 (UMCTL2_30C_5)         << 15 |\
                                 (UMCTL2_30C_6)         << 14 |\
                                 (UMCTL2_30C_7)         << 13 |\
                                 (UMCTL2_30C_8)         << 12 |\
                                 (UMCTL2_30C_9)         << 11 |\
                                 (UMCTL2_30C_10)        << 10 |\
                                 (UMCTL2_30C_11)        <<  9 |\
                                 (UMCTL2_30C_12)        <<  8 |\
                                 (UMCTL2_30C_13)        <<  5 |\
                                 (UMCTL2_30C_14)        <<  4 |\
                                 (UMCTL2_30C_15)        <<  3 |\
                                 (UMCTL2_30C_16)        <<  2 |\
                                 (UMCTL2_30C_17)        <<  1 |\
                                 (UMCTL2_30C_18)        <<  0 )

        #define UMCTL2_320_1    1
        #define UMCTL2_320_2    0
        #define UMCTL2_320(n)   ((n)    << 0 )

        #define UMCTL2_36C_1    0
        #define UMCTL2_36C_2    0
        #define UMCTL2_36C_3    1
        #define UMCTL2_36C_4    0
        #define UMCTL2_36C_5    1
        #define UMCTL2_36C_6    0
        #define UMCTL2_36C      ((UMCTL2_36C_1) << 24 |\
                                 (UMCTL2_36C_2) << 20 |\
                                 (UMCTL2_36C_3) << 16 |\
                                 (UMCTL2_36C_4) <<  8 |\
                                 (UMCTL2_36C_5) <<  4 |\
                                 (UMCTL2_36C_6) <<  0 )

        #define UMCTL2_400_1    0
        #define UMCTL2_400_2    0
        #define UMCTL2_400_3    0
        #define UMCTL2_400      ((UMCTL2_400_1) << 8 |\
                                 (UMCTL2_400_2) << 4 |\
                                 (UMCTL2_400_3) << 0 )

        #define UMCTL2_404_4    0
        #define UMCTL2_404_5    1
        #define UMCTL2_404_6    0
        #define UMCTL2_404_7    0
        #define UMCTL2_404_8    1
        #define UMCTL2_404_9    0
        #define UMCTL2_404_10   0
        #define UMCTL2_404_11   15
        #define UMCTL2_404_1    ((UMCTL2_404_5)         <<  2 |\
                                 (UMCTL2_404_7)         <<  1 |\
                                 (UMCTL2_404_9)         <<  0 )
        #define UMCTL2_404_2    ((UMCTL2_404_5)         <<  2 |\
                                 (UMCTL2_404_7)         <<  1 |\
                                 (UMCTL2_404_8)         <<  0 )
        #define UMCTL2_404_3    ((UMCTL2_404_6)         <<  2 |\
                                 (UMCTL2_404_7)         <<  1 |\
                                 (UMCTL2_404_8)         <<  0 )
        #define UMCTL2_404(n)   ((UMCTL2_404_4)         << 16 |\
                                 (n)                    << 12 |\
                                 (UMCTL2_404_10)        << 11 |\
                                 (UMCTL2_404_11)        <<  0 )

        #define UMCTL2_408_5    1
        #define UMCTL2_408_6    0
        #define UMCTL2_408_7    0
        #define UMCTL2_408_8    1
        #define UMCTL2_408_9    0
        #define UMCTL2_408_10   15
        #define UMCTL2_408_1    ((UMCTL2_408_5)         <<  2 |\
                                 (UMCTL2_408_7)         <<  1 |\
                                 (UMCTL2_408_9)         <<  0 )
        #define UMCTL2_408_2    ((UMCTL2_408_5)         <<  2 |\
                                 (UMCTL2_408_7)         <<  1 |\
                                 (UMCTL2_408_8)         <<  0 )
        #define UMCTL2_408_3    ((UMCTL2_408_6)         <<  2 |\
                                 (UMCTL2_408_7)         <<  1 |\
                                 (UMCTL2_408_8)         <<  0 )
        #define UMCTL2_408_4    ((UMCTL2_408_6)         <<  2 |\
                                 (UMCTL2_408_7)         <<  1 |\
                                 (UMCTL2_408_9)         <<  0 )
        #define UMCTL2_408(n)   ((n)                    << 12 |\
                                 (UMCTL2_408_10)        <<  0 )

        #define UMCTL2_490_1    1
        #define UMCTL2_490      ((UMCTL2_490_1) << 0 )

        #define uMCTL2_register_offset  0x9c107000
#elif defined(PLATFORM_I143)
// Add for New UMCTL2
        #define UMCTL2_0_1      0
        #define UMCTL2_0_2      4
        #define UMCTL2_0_3      SDCTRL_MRS_DLL_MODE
        #ifdef  MPEG_DRAM_TYPE_DDR3
                #define UMCTL2_0_4      1
        #else
                #define UMCTL2_0_4      0
        #endif
        #define UMCTL2_0        ((UMCTL2_0_1)   << 22 |\
                                 (UMCTL2_0_2)   << 16 |\
                                 (UMCTL2_0_3)   << 15 |\
                                 (UMCTL2_0_4)   <<  0 )

        #define UMCTL2_10       0x00003010

        #define UMCTL2_14       0x0000f6a2

        #define UMCTL2_30_1     1
        #define UMCTL2_30_2     0
        #define UMCTL2_30(n)    ((n) << 0)

        #define UMCTL2_34_1     64
        #define UMCTL2_34_2     4
        #define UMCTL2_34       ((UMCTL2_34_1)  << 16 |\
                                 (UMCTL2_34_2)  <<  0 )

        #define UMCTL2_38_1             254
        #define UMCTL2_38_2             0
        #define UMCTL2_38_3             0
        #define UMCTL2_38               ((UMCTL2_38_1)  << 16 |\
                                         (UMCTL2_38_2)  <<  1 |\
                                         (UMCTL2_38_3)  <<  0 )

        #define UMCTL2_50_1     2
        #define UMCTL2_50_2     16
        #define UMCTL2_50_3     0
        #define UMCTL2_50_4     0
        #define UMCTL2_50       ((UMCTL2_50_1)  << 20 |\
                                 (UMCTL2_50_2)  << 12 |\
                                 (UMCTL2_50_3)  <<  4 |\
                                 (UMCTL2_50_4)  <<  2 )

        #define UMCTL2_60_1     0
        #define UMCTL2_60_2     0
        #define UMCTL2_60_3     0
        #define UMCTL2_60_4     0
        #define UMCTL2_60       ((UMCTL2_60_1)  << 16 |\
                                 (UMCTL2_60_2)  <<  4 |\
                                 (UMCTL2_60_3)  <<  1 |\
                                 (UMCTL2_60_4)  <<  0 )

        #define UMCTL2_64_1     0
        #define UMCTL2_64_2     nAREF_INTVAL
        #define UMCTL2_64_3     0
        #define UMCTL2_64_4     ((n_tRFC + 1) >> 1)
        #define UMCTL2_64       ((UMCTL2_64_1)  << 31 |\
                                 (UMCTL2_64_2)  << 16 |\
                                 (UMCTL2_64_3)  << 15 |\
                                 (UMCTL2_64_4)  <<  0 )
#ifdef CONFIG_DRAM_SIZE_USE_OTP
        #define UMCTL2_64_512Mb  ((UMCTL2_64_1)  << 31 |\
                                 (UMCTL2_64_2)  << 16 |\
                                 (UMCTL2_64_3)  << 15 |\
                                 ((n_tRFC_512Mb+ 1) >> 1)  <<  0 )
        #define UMCTL2_64_1Gb    ((UMCTL2_64_1)  << 31 |\
                                 (UMCTL2_64_2)  << 16 |\
                                 (UMCTL2_64_3)  << 15 |\
                                 ((n_tRFC_1Gb + 1) >> 1)  <<  0 )
        #define UMCTL2_64_4Gb    ((UMCTL2_64_1)  << 31 |\
                                 (UMCTL2_64_2)  << 16 |\
                                 (UMCTL2_64_3)  << 15 |\
                                 ((n_tRFC_4Gb + 1) >> 1)  <<  0 )
#endif
        #define UMCTL2_C0_1     0
        #define UMCTL2_C0_2     0
        #define UMCTL2_C0_3     0
        #define UMCTL2_C0_4     0
        #define UMCTL2_C0_5     0
        #define UMCTL2_C0_6     0
        #define UMCTL2_C0_7     0
        #define UMCTL2_C0       ((UMCTL2_C0_1)  << 16 |\
                                 (UMCTL2_C0_2)  << 15 |\
                                 (UMCTL2_C0_3)  <<  8 |\
                                 (UMCTL2_C0_4)  <<  4 |\
                                 (UMCTL2_C0_5)  <<  2 |\
                                 (UMCTL2_C0_6)  <<  1 |\
                                 (UMCTL2_C0_7)  <<  0 )

        #define UMCTL2_D0_1     0
        #define UMCTL2_D0_2     2
        #define UMCTL2_D0_3     2
        #define UMCTL2_D0       ((UMCTL2_D0_1)  << 30 |\
                                 (UMCTL2_D0_2)  << 16 |\
                                 (UMCTL2_D0_3)  <<  0 )

        #define UMCTL2_D4_1     1
        #define UMCTL2_D4_2     2
        #define UMCTL2_D4       ((UMCTL2_D4_1)  << 16 |\
                                 (UMCTL2_D4_2)  <<  0 )

        #define UMCTL2_DC_1     MRS_DLL_ON
        #define UMCTL2_DC_2     MRS_WRITE_RECOVERY
        #define UMCTL2_DC_3     MRS_DLL_RESET
        #define UMCTL2_DC_4     0
        #define UMCTL2_DC_5     MRS_CAS_LATENCY
        #define UMCTL2_DC_6     MRS_CL_M2
        #define UMCTL2_DC_7     0
        #define UMCTL2_DC_8     DDR3_BURST_LEN
        #define UMCTL2_DC_9     SDCTRL_MRS_QOFF
        #define UMCTL2_DC_10    SDCTRL_MRS_TDQS
        #define UMCTL2_DC_11    SDCTRL_MRS_RTT_2
        #define UMCTL2_DC_12    SDCTRL_MRS_RTT_1
        #define UMCTL2_DC_13    SDCTRL_MRS_RTT_0
        #define UMCTL2_DC_14    SDCTRL_MRS_WL
        #define UMCTL2_DC_15    SDCTRL_MRS_ODS_1
        #define UMCTL2_DC_16    SDCTRL_MRS_ODS_0
        #define UMCTL2_DC_17    SDCTRL_MRS_AL(0)
        #define UMCTL2_DC_18    SDCTRL_MRS_DLL_MODE
        #define UMCTL2_DC       ((UMCTL2_DC_1)  << 16 |\
                                 (UMCTL2_DC_2)  << 16 |\
                                 (UMCTL2_DC_3)  << 16 |\
                                 (UMCTL2_DC_4)  << 23 |\
                                 (UMCTL2_DC_5)  << 16 |\
                                 (UMCTL2_DC_7)  << 19 |\
                                 (UMCTL2_DC_6)  << 16 |\
                                 (UMCTL2_DC_8)  << 16 |\
                                 (UMCTL2_DC_9)  <<  0 |\
                                 (UMCTL2_DC_10) <<  0 |\
                                 (UMCTL2_DC_11) <<  0 |\
                                 (UMCTL2_DC_14) <<  0 |\
                                 (UMCTL2_DC_12) <<  0 |\
                                 (UMCTL2_DC_15) <<  0 |\
                                 (UMCTL2_DC_17) <<  0 |\
                                 (UMCTL2_DC_13) <<  0 |\
                                 (UMCTL2_DC_16) <<  0 |\
                                 (UMCTL2_DC_18) <<  0 )

        #define UMCTL2_E0_1     SDCTRL_MRS_RTT_WR
        #define UMCTL2_E0_2     SDCTRL_MRS_SRT
        #define UMCTL2_E0_3     SDCTRL_MRS_ASR
        #define UMCTL2_E0_4     SDCTRL_MRS_CWL
        #define UMCTL2_E0_5     0
        #define UMCTL2_E0_6     0
        #define UMCTL2_E0_7     0
        #define UMCTL2_E0       ((UMCTL2_E0_1)  << 16 |\
                                 (UMCTL2_E0_2)  << 16 |\
                                 (UMCTL2_E0_3)  << 16 |\
                                 (UMCTL2_E0_4)  << 16 |\
                                 (UMCTL2_E0_5)  << 16 |\
                                 (UMCTL2_E0_6)  <<  0 |\
                                 (UMCTL2_E0_7)  <<  0 )

        #define UMCTL2_E4_1     9
        #define UMCTL2_E4_2     0
        #define UMCTL2_E4       ((UMCTL2_E4_1)  << 16 |\
                                 (UMCTL2_E4_2)  <<  0 )

        #define UMCTL2_F0_1     0
        #define UMCTL2_F0_2     0
        #define UMCTL2_F0_3     0
        #define UMCTL2_F0_4     0
        #define UMCTL2_F0_5     0
        #define UMCTL2_F0_6     0
        #define UMCTL2_F0_7     0
        #define UMCTL2_F0_8     0
        #define UMCTL2_F0_9     0
        #define UMCTL2_F0_10    0
        #define UMCTL2_F0_11    0
        #define UMCTL2_F0       ((UMCTL2_F0_1)  << 14 |\
                                 (UMCTL2_F0_2)  << 13 |\
                                 (UMCTL2_F0_3)  << 12 |\
                                 (UMCTL2_F0_4)  <<  8 |\
                                 (UMCTL2_F0_5)  <<  6 |\
                                 (UMCTL2_F0_6)  <<  5 |\
                                 (UMCTL2_F0_7)  <<  4 |\
                                 (UMCTL2_F0_8)  <<  3 |\
                                 (UMCTL2_F0_9)  <<  2 |\
                                 (UMCTL2_F0_10) <<  1 |\
                                 (UMCTL2_F0_11) <<  0 )

        #define UMCTL2_100_1    ((n_tCL + 4 + n_tWR + 1) >> 1)
        #define UMCTL2_100_2    ((n_tFAW + 1) >> 1)
        #define UMCTL2_100_3    46800
        #define UMCTL2_100_4    (((UMCTL2_100_3 >> 10) -1) >> 1)
        #define UMCTL2_100_5    ((n_tRAS + 1) >> 1)
        #define UMCTL2_100      ((UMCTL2_100_1) << 24 |\
                                 (UMCTL2_100_2) << 16 |\
                                 (UMCTL2_100_4) <<  8 |\
                                 (UMCTL2_100_5) <<  0 )

        #define UMCTL2_104_1    8
        #define UMCTL2_104_2    ((nRTP + 1) >> 1)
        #define UMCTL2_104_3    ((n_tRC + 1) >> 1)
        #define UMCTL2_104      ((UMCTL2_104_1) << 16 |\
                                 (UMCTL2_104_2) <<  8 |\
                                 (UMCTL2_104_3) <<  0 )

        #define UMCTL2_108_1    0
        #define UMCTL2_108_2    0
        #define UMCTL2_108_3    (((n_tCL + 4 + 2 - MRS_CWL_CNT) + 1) >> 1)
        #define UMCTL2_108_4    (((MRS_CWL_CNT + 4 + n_tWTR) +1) >> 1)
        #define UMCTL2_108      ((UMCTL2_108_1) << 24 |\
                                 (UMCTL2_108_2) << 16 |\
                                 (UMCTL2_108_3) <<  8 |\
                                 (UMCTL2_108_4) <<  0 )

        #define UMCTL2_10C_1    0
        #define UMCTL2_10C_2    ((n_tMRD + 1) >> 1)
        #define UMCTL2_10C_3    ((n_tMOD + 1) >> 1)
        #define UMCTL2_10C      ((UMCTL2_10C_1) << 20 |\
                                 (UMCTL2_10C_2) << 12 |\
                                 (UMCTL2_10C_3) <<  0 )

        #define UMCTL2_110_1    ((n_tRCD + 1) >> 1)
        #define UMCTL2_110_2    ((n_tCCD + 1) >> 1)
        #define UMCTL2_110_3    ((n_tRRD + 1) >> 1)
        #define UMCTL2_110_4    (((n_tRP + 1) >> 1) + 1)
        #define UMCTL2_110      ((UMCTL2_110_1) << 24 |\
                                 (UMCTL2_110_2) << 16 |\
                                 (UMCTL2_110_3) <<  8 |\
                                 (UMCTL2_110_4) <<  0 )

        #define UMCTL2_114_1    4
        #define UMCTL2_114_2    4
        #define UMCTL2_114_3    3
        #define UMCTL2_114_4    2
        #define UMCTL2_114      ((UMCTL2_114_1) << 24 |\
                                 (UMCTL2_114_2) << 16 |\
                                 (UMCTL2_114_3) <<  8 |\
                                 (UMCTL2_114_4) <<  0 )

        #define UMCTL2_120_1    0
        #define UMCTL2_120_2    0
        #define UMCTL2_120_3    10
        #define UMCTL2_120_4    3
        #define UMCTL2_120      ((UMCTL2_120_1) << 24 |\
                                 (UMCTL2_120_2) << 16 |\
                                 (UMCTL2_120_3) <<  8 |\
                                 (UMCTL2_120_4) <<  0 )

        #define UMCTL2_13C_1    0
        #define UMCTL2_13C_2    0
        #define UMCTL2_13C_3    0
        #define UMCTL2_13C      ((UMCTL2_13C_1) << 31 |\
                                 (UMCTL2_13C_2) << 24 |\
                                 (UMCTL2_13C_3) <<  0 )

        #define UMCTL2_180_1    0
        #define UMCTL2_180_2    0
        #define UMCTL2_180_3    0
        #define UMCTL2_180_4    0
        #define UMCTL2_180_5    128
        #define UMCTL2_180_6    32
        #define UMCTL2_180      ((UMCTL2_180_1) << 31 |\
                                 (UMCTL2_180_2) << 30 |\
                                 (UMCTL2_180_3) << 29 |\
                                 (UMCTL2_180_4) << 28 |\
                                 (UMCTL2_180_5) << 16 |\
                                 (UMCTL2_180_6) <<  0 )

        #define UMCTL2_184_1    0
        #define UMCTL2_184_2    205756
        #define UMCTL2_184      ((UMCTL2_184_1) << 20 |\
                                 (UMCTL2_184_2) <<  0 )

        #define UMCTL2_190_1    3
        #define UMCTL2_190_2    1
#if 1
#define UMCTL2_190_3    9
#else
        #ifdef NO_QUICK_SIM
                #ifdef  POST_SIM
                        #define UMCTL2_190_3    10
                #else
                        #define UMCTL2_190_3    9
                #endif
        #else
                #define UMCTL2_190_3    9
        #endif
#endif

        #define UMCTL2_190_4    1
        #define UMCTL2_190_5    1
        #define UMCTL2_190_6    6
        #define UMCTL2_190      ((UMCTL2_190_1) << 24 |\
                                 (UMCTL2_190_2) << 23 |\
                                 (UMCTL2_190_3) << 16 |\
                                 (UMCTL2_190_4) << 15 |\
                                 (UMCTL2_190_5) <<  8 |\
                                 (UMCTL2_190_6) <<  0 )

        #define UMCTL2_194_1    0
        #define UMCTL2_194_2    0
        #define UMCTL2_194_3    6
        #define UMCTL2_194_4    1
        #define UMCTL2_194_5    1
        #define UMCTL2_194      ((UMCTL2_194_1) << 28 |\
                                 (UMCTL2_194_2) << 24 |\
                                 (UMCTL2_194_3) << 16 |\
                                 (UMCTL2_194_4) <<  8 |\
                                 (UMCTL2_194_5) <<  0 )

        #define UMCTL2_198_1    7
        #define UMCTL2_198_2    0
        #define UMCTL2_198_3    0
        #define UMCTL2_198_4    7
        #define UMCTL2_198_5    1
        #define UMCTL2_198_6    0
        #define UMCTL2_198_7    0
        #define UMCTL2_198      ((UMCTL2_198_1) << 24 |\
                                 (UMCTL2_198_2) << 20 |\
                                 (UMCTL2_198_3) << 16 |\
                                 (UMCTL2_198_4) << 12 |\
                                 (UMCTL2_198_5) <<  8 |\
                                 (UMCTL2_198_6) <<  4 |\
                                 (UMCTL2_198_7) <<  0 )

        #define UMCTL2_1A0_1    1
        #define UMCTL2_1A0_2    1
        #define UMCTL2_1A0_3    1
        #define UMCTL2_1A0_4    64
        #define UMCTL2_1A0_5    5
        #define UMCTL2_1A0      ((UMCTL2_1A0_1) << 31 |\
                                 (UMCTL2_1A0_2) << 30 |\
                                 (UMCTL2_1A0_3) << 29 |\
                                 (UMCTL2_1A0_4) << 16 |\
                                 (UMCTL2_1A0_5) <<  0 )

        #define UMCTL2_1A4_1    1
        #define UMCTL2_1A4_2    2
        #define UMCTL2_1A4      ((UMCTL2_1A4_1) << 16 |\
                                 (UMCTL2_1A4_2) <<  0 )

        #define UMCTL2_1A8_1    0
        #define UMCTL2_1A8      ((UMCTL2_1A8_1) << 31 )

        #define UMCTL2_1B0_4    0
        #define UMCTL2_1B0_5    0
        #define UMCTL2_1B0_6    0
        #define UMCTL2_1B0_7    0
        #define UMCTL2_1B0_8    1
        #define UMCTL2_1B0_9    0
        #define UMCTL2_1B0_10   0
        #define UMCTL2_1B0_11   0
        #define UMCTL2_1B0_12   0
        #define UMCTL2_1B0_13   1
        #define UMCTL2_1B0_14   1
        #define UMCTL2_1B0_15   0
        #define UMCTL2_1B0_1    ((UMCTL2_1B0_8)         << 4 |\
                                 (UMCTL2_1B0_10)        << 3 |\
                                 (UMCTL2_1B0_11)        << 2 |\
                                 (UMCTL2_1B0_12)        << 1 |\
                                 (UMCTL2_1B0_14)        << 0 )
        #define UMCTL2_1B0_2    ((UMCTL2_1B0_8)         << 4 |\
                                 (UMCTL2_1B0_10)        << 3 |\
                                 (UMCTL2_1B0_11)        << 2 |\
                                 (UMCTL2_1B0_12)        << 1 |\
                                 (UMCTL2_1B0_14)        << 0 )
        #define UMCTL2_1B0_3    ((UMCTL2_1B0_8)         << 4 |\
                                 (UMCTL2_1B0_10)        << 3 |\
                                 (UMCTL2_1B0_11)        << 2 |\
                                 (UMCTL2_1B0_12)        << 1 |\
                                 (UMCTL2_1B0_14)        << 0 )
        #define UMCTL2_1B0(n)   ((UMCTL2_1B0_4)         << 8 |\
                                 (UMCTL2_1B0_5)         << 7 |\
                                 (UMCTL2_1B0_6)         << 6 |\
                                 (UMCTL2_1B0_7)         << 5 |\
                                 (n)                    << 0 )

        #define UMCTL2_1B4_1    4
        #define UMCTL2_1B4_2    6
        #define UMCTL2_1B4      ((UMCTL2_1B4_1) << 8 |\
                                 (UMCTL2_1B4_2) << 0 )

        #define UMCTL2_1C4_1    0
        #define UMCTL2_1C4      ((UMCTL2_1C4_1) << 0 )

        #ifdef  UMCTL2_BA_NO_2
                #define UMCTL2_204_1    (nCOL_WIDTH - 1)
                #define UMCTL2_204_2    (nCOL_WIDTH - 1)
                #define UMCTL2_204_3    (nCOL_WIDTH - 1)
        #endif
        #ifdef  UMCTL2_BA_NO_1
                #define UMCTL2_204_1    63
                #define UMCTL2_204_2    (nCOL_WIDTH - 1)
                #define UMCTL2_204_3    (nCOL_WIDTH - 1)
        #endif
        #ifdef  UMCTL2_BA_NO_0
                #define UMCTL2_204_1    63
                #define UMCTL2_204_2    63
                #define UMCTL2_204_3    (nCOL_WIDTH - 1)
        #endif
        #define UMCTL2_204      ((UMCTL2_204_1) << 16 |\
                                 (UMCTL2_204_2) <<  8 |\
                                 (UMCTL2_204_3) <<  0 )

        #ifdef  UMCTL2_CA_NO_11
                #define UMCTL2_210_1    0
                #define UMCTL2_210_2    0
                #define UMCTL2_20C_1    0
                #define UMCTL2_20C_2    0
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_10
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    0
                #define UMCTL2_20C_1    0
                #define UMCTL2_20C_2    0
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_9
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    0
                #define UMCTL2_20C_2    0
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_8
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    0
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_7
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    0
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_6
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    0
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_5
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    0
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_4
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    15
                #define UMCTL2_208_2    0
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_3
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    15
                #define UMCTL2_208_2    15
                #define UMCTL2_208_3    0
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_2
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    15
                #define UMCTL2_208_2    15
                #define UMCTL2_208_3    15
                #define UMCTL2_208_4    0
        #endif
        #ifdef  UMCTL2_CA_NO_1
                #define UMCTL2_210_1    31
                #define UMCTL2_210_2    31
                #define UMCTL2_20C_1    31
                #define UMCTL2_20C_2    31
                #define UMCTL2_20C_3    31
                #define UMCTL2_20C_4    31
                #define UMCTL2_208_1    15
                #define UMCTL2_208_2    15
                #define UMCTL2_208_3    15
                #define UMCTL2_208_4    15
        #endif
        #define UMCTL2_208      ((UMCTL2_208_1) << 24 |\
                                 (UMCTL2_208_2) << 16 |\
                                 (UMCTL2_208_3) <<  8 |\
                                 (UMCTL2_208_4) <<  0 )

        #define UMCTL2_20C      ((UMCTL2_20C_1) << 24 |\
                                 (UMCTL2_20C_2) << 16 |\
                                 (UMCTL2_20C_3) <<  8 |\
                                 (UMCTL2_20C_4) <<  0 )

        #define UMCTL2_210_3    0
        #define UMCTL2_210      ((UMCTL2_210_3) << 31 |\
                                 (UMCTL2_210_1) <<  8 |\
                                 (UMCTL2_210_2) <<  0 )

        #ifdef  UMCTL2_RA_NO_17
                #define UMCTL2_218_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_16
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_15
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_14
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_13
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_12
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_11
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_10
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_9
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_8
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_7
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_6
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_5
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_4
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_3
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    15
                #define UMCTL2_224_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_2
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    15
                #define UMCTL2_224_3    15
                #define UMCTL2_224_4    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_1
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    15
                #define UMCTL2_224_3    15
                #define UMCTL2_224_4    15
                #define UMCTL2_214_2    (nBANK_WIDTH + nCOL_WIDTH - 4)
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #ifdef  UMCTL2_RA_NO_0
                #define UMCTL2_218_1    15
                #define UMCTL2_218_2    15
                #define UMCTL2_218_3    15
                #define UMCTL2_218_4    15
                #define UMCTL2_218_5    15
                #define UMCTL2_218_6    15
                #define UMCTL2_214_1    15
                #define UMCTL2_22C_1    15
                #define UMCTL2_228_1    15
                #define UMCTL2_228_2    15
                #define UMCTL2_228_3    15
                #define UMCTL2_228_4    15
                #define UMCTL2_224_1    15
                #define UMCTL2_224_2    15
                #define UMCTL2_224_3    15
                #define UMCTL2_224_4    15
                #define UMCTL2_214_2    15
                #define UMCTL2_214_3    (nBANK_WIDTH + nCOL_WIDTH - 4)
        #endif
        #define UMCTL2_214_4    15
        #define UMCTL2_214      ((UMCTL2_214_1) << 24 |\
                                 (UMCTL2_214_4) << 16 |\
                                 (UMCTL2_214_2) <<  8 |\
                                 (UMCTL2_214_3) <<  0 )

        #define UMCTL2_218_7    0
        #define UMCTL2_218_8    0
        #define UMCTL2_218      ((UMCTL2_218_7) << 31 |\
                                 (UMCTL2_218_8) << 29 |\
                                 (UMCTL2_218_3) << 24 |\
                                 (UMCTL2_218_4) << 16 |\
                                 (UMCTL2_218_5) <<  8 |\
                                 (UMCTL2_218_6) <<  0 )
#ifdef CONFIG_DRAM_SIZE_USE_OTP
        #define UMCTL2_218_512Mb ((UMCTL2_218_7) << 31 |\
                                 (UMCTL2_218_8) << 29 |\
                                 (15) << 24 |\
                                 (15) << 16 |\
                                 (15) <<  8 |\
                                 (15) <<  0 )
        #define UMCTL2_218_1Gb   ((UMCTL2_218_7) << 31 |\
                                 (UMCTL2_218_8) << 29 |\
                                 (15) << 24 |\
                                 (15) << 16 |\
                                 (15) <<  8 |\
                                 (nBANK_WIDTH + nCOL_WIDTH - 4) <<  0 )

        #define UMCTL2_218_4Gb   ((UMCTL2_218_7) << 31 |\
                                 (UMCTL2_218_8) << 29 |\
                                 (15) << 24 |\
                                 (nBANK_WIDTH + nCOL_WIDTH - 4) << 16 |\
                                 (nBANK_WIDTH + nCOL_WIDTH - 4) <<  8 |\
                                 (nBANK_WIDTH + nCOL_WIDTH - 4) <<  0 )

#endif
        #define UMCTL2_224      ((UMCTL2_224_1) << 24 |\
                                 (UMCTL2_224_2) << 16 |\
                                 (UMCTL2_224_3) <<  8 |\
                                 (UMCTL2_224_4) <<  0 )

        #define UMCTL2_228      ((UMCTL2_228_1) << 24 |\
                                 (UMCTL2_228_2) << 16 |\
                                 (UMCTL2_228_3) <<  8 |\
                                 (UMCTL2_228_4) <<  0 )

        #define UMCTL2_22C_2    0
        #define UMCTL2_22C_3    0
        #define UMCTL2_22C      ((UMCTL2_22C_2) << 16 |\
                                 (UMCTL2_22C_3) <<  8 |\
                                 (UMCTL2_22C_1) <<  0 )

        #define UMCTL2_240_1    6
        #define UMCTL2_240_2    0
        #define UMCTL2_240_3    6
        #define UMCTL2_240_4    (n_tCL - MRS_CWL_CNT)
        #define UMCTL2_240      ((UMCTL2_240_1) << 24 |\
                                 (UMCTL2_240_2) << 16 |\
                                 (UMCTL2_240_3) <<  8 |\
                                 (UMCTL2_240_4) <<  2 )

        #define UMCTL2_244_1    0
        #define UMCTL2_244_2    0
        #define UMCTL2_244_3    0
        #define UMCTL2_244_4    0
        #define UMCTL2_244_5    0
        #define UMCTL2_244_6    0
        #define UMCTL2_244_7    0
        #define UMCTL2_244_8    1
        #define UMCTL2_244      ((UMCTL2_244_1) << 28 |\
                                 (UMCTL2_244_2) << 24 |\
                                 (UMCTL2_244_3) << 20 |\
                                 (UMCTL2_244_4) << 16 |\
                                 (UMCTL2_244_5) << 12 |\
                                 (UMCTL2_244_6) <<  8 |\
                                 (UMCTL2_244_7) <<  4 |\
                                 (UMCTL2_244_8) <<  0 )

        #define UMCTL2_250_1    0
        #define UMCTL2_250_2    55
        #define UMCTL2_250_3    63
        #define UMCTL2_250_4    0
        #define UMCTL2_250_5    0
        #define UMCTL2_250_6    1
        #define UMCTL2_250      ((UMCTL2_250_1) << 24 |\
                                 (UMCTL2_250_2) << 16 |\
                                 (UMCTL2_250_3) <<  8 |\
                                 (UMCTL2_250_4) <<  2 |\
                                 (UMCTL2_250_5) <<  1 |\
                                 (UMCTL2_250_6) <<  0 )

        #define UMCTL2_254_1    0
        #define UMCTL2_254      ((UMCTL2_254_1) << 0 )

        #define UMCTL2_25C_1    15
        #define UMCTL2_25C_2    1
        #define UMCTL2_25C      ((UMCTL2_25C_1) << 24 |\
                                 (UMCTL2_25C_2) <<  0 )

        #define UMCTL2_264_1    128
        #define UMCTL2_264_2    1
        #define UMCTL2_264      ((UMCTL2_264_1) << 24 |\
                                 (UMCTL2_264_2) <<  0 )

        #define UMCTL2_26C_1    8
        #define UMCTL2_26C_2    2048
        #define UMCTL2_26C      ((UMCTL2_26C_1) << 24 |\
                                 (UMCTL2_26C_2) <<  0 )

        #define UMCTL2_300_1    0
        #define UMCTL2_300_2    0
        #define UMCTL2_300_3    0
        #define UMCTL2_300_4    0
        #define UMCTL2_300      ((UMCTL2_300_1) << 4 |\
                                 (UMCTL2_300_2) << 2 |\
                                 (UMCTL2_300_3) << 1 |\
                                 (UMCTL2_300_4) << 0 )

        #define UMCTL2_304_3    0
        #define UMCTL2_304_1    1
        #define UMCTL2_304_2    0
        #define UMCTL2_304(n)   ((UMCTL2_304_3) << 1 |\
                                 (n)            << 0 )

        #define UMCTL2_30C_1    0
        #define UMCTL2_30C_2    0
        #define UMCTL2_30C_3    0
        #define UMCTL2_30C_4    0
        #define UMCTL2_30C_5    0
        #define UMCTL2_30C_6    0
        #define UMCTL2_30C_7    0
        #define UMCTL2_30C_8    0
        #define UMCTL2_30C_9    0
        #define UMCTL2_30C_10   0
        #define UMCTL2_30C_11   0
        #define UMCTL2_30C_12   0
        #define UMCTL2_30C_13   0
        #define UMCTL2_30C_14   0
        #define UMCTL2_30C_15   0
        #define UMCTL2_30C_16   0
        #define UMCTL2_30C_17   0
        #define UMCTL2_30C_18   0
        #define UMCTL2_30C      ((UMCTL2_30C_1)         << 19 |\
                                 (UMCTL2_30C_2)         << 18 |\
                                 (UMCTL2_30C_3)         << 17 |\
                                 (UMCTL2_30C_4)         << 16 |\
                                 (UMCTL2_30C_5)         << 15 |\
                                 (UMCTL2_30C_6)         << 14 |\
                                 (UMCTL2_30C_7)         << 13 |\
                                 (UMCTL2_30C_8)         << 12 |\
                                 (UMCTL2_30C_9)         << 11 |\
                                 (UMCTL2_30C_10)        << 10 |\
                                 (UMCTL2_30C_11)        <<  9 |\
                                 (UMCTL2_30C_12)        <<  8 |\
                                 (UMCTL2_30C_13)        <<  5 |\
                                 (UMCTL2_30C_14)        <<  4 |\
                                 (UMCTL2_30C_15)        <<  3 |\
                                 (UMCTL2_30C_16)        <<  2 |\
                                 (UMCTL2_30C_17)        <<  1 |\
                                 (UMCTL2_30C_18)        <<  0 )

        #define UMCTL2_320_1    1
        #define UMCTL2_320_2    0
        #define UMCTL2_320(n)   ((n)    << 0 )

        #define UMCTL2_36C_1    0
        #define UMCTL2_36C_2    0
        #define UMCTL2_36C_3    1
        #define UMCTL2_36C_4    0
        #define UMCTL2_36C_5    1
        #define UMCTL2_36C_6    0
        #define UMCTL2_36C      ((UMCTL2_36C_1) << 24 |\
                                 (UMCTL2_36C_2) << 20 |\
                                 (UMCTL2_36C_3) << 16 |\
                                 (UMCTL2_36C_4) <<  8 |\
                                 (UMCTL2_36C_5) <<  4 |\
                                 (UMCTL2_36C_6) <<  0 )

        #define UMCTL2_400_1    0
        #define UMCTL2_400_2    0
        #define UMCTL2_400_3    0
        #define UMCTL2_400      ((UMCTL2_400_1) << 8 |\
                                 (UMCTL2_400_2) << 4 |\
                                 (UMCTL2_400_3) << 0 )

        #define UMCTL2_404_4    0
        #define UMCTL2_404_5    1
        #define UMCTL2_404_6    0
        #define UMCTL2_404_7    0
        #define UMCTL2_404_8    1
        #define UMCTL2_404_9    0
        #define UMCTL2_404_10   0
        #define UMCTL2_404_11   15
        #define UMCTL2_404_1    ((UMCTL2_404_5)         <<  2 |\
                                 (UMCTL2_404_7)         <<  1 |\
                                 (UMCTL2_404_9)         <<  0 )
        #define UMCTL2_404_2    ((UMCTL2_404_5)         <<  2 |\
                                 (UMCTL2_404_7)         <<  1 |\
                                 (UMCTL2_404_8)         <<  0 )
        #define UMCTL2_404_3    ((UMCTL2_404_6)         <<  2 |\
                                 (UMCTL2_404_7)         <<  1 |\
                                 (UMCTL2_404_8)         <<  0 )
        #define UMCTL2_404(n)   ((UMCTL2_404_4)         << 16 |\
                                 (n)                    << 12 |\
                                 (UMCTL2_404_10)        << 11 |\
                                 (UMCTL2_404_11)        <<  0 )

        #define UMCTL2_408_5    1
        #define UMCTL2_408_6    0
        #define UMCTL2_408_7    0
        #define UMCTL2_408_8    1
        #define UMCTL2_408_9    0
        #define UMCTL2_408_10   15
        #define UMCTL2_408_1    ((UMCTL2_408_5)         <<  2 |\
                                 (UMCTL2_408_7)         <<  1 |\
                                 (UMCTL2_408_9)         <<  0 )
        #define UMCTL2_408_2    ((UMCTL2_408_5)         <<  2 |\
                                 (UMCTL2_408_7)         <<  1 |\
                                 (UMCTL2_408_8)         <<  0 )
        #define UMCTL2_408_3    ((UMCTL2_408_6)         <<  2 |\
                                 (UMCTL2_408_7)         <<  1 |\
                                 (UMCTL2_408_8)         <<  0 )
        #define UMCTL2_408_4    ((UMCTL2_408_6)         <<  2 |\
                                 (UMCTL2_408_7)         <<  1 |\
                                 (UMCTL2_408_9)         <<  0 )
        #define UMCTL2_408(n)   ((n)                    << 12 |\
                                 (UMCTL2_408_10)        <<  0 )

        #define UMCTL2_490_1    1
        #define UMCTL2_490      ((UMCTL2_490_1) << 0 )

        #define uMCTL2_register_offset  0x9c107000

#endif
