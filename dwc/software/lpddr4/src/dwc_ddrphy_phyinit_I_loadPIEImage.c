/** \file
 * \brief implements Step I of initialization sequence
 *
 * This file contains the implementation of dwc_ddrphy_phyinit_I_initPhyConfig
 * function. 
 *
 *  \addtogroup SrcFunc
 *  @{
 */
#include "dwc_ddrphy_phyinit.h"
/** \brief Loads registers after training
 *
 * This function programs the PHY Initialization Engine (PIE) instructions and
 * the associated registers.  
 * Training hardware registers are also programmed to for mission mode
 * retraining.
 * 
 * \return void
 * 
 * Detailed list of registers programmed by this function:
 */
void dwc_ddrphy_phyinit_I_loadPIEImage (int skip_training) {
    
    char *printf_header;
    printf_header = "// [phyinit_I_loadPIEImage]";
    
    dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_I_loadPIEImage()\n", printf_header);

    unsigned int pstate; 
    int p_addr;

    int PPTTrainSetup[4];
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("// (I) Load PHY Init Engine Image \n");
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("// Load the PHY Initialization Engine memory with the provided initialization sequence.\n");
    dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("// For LPDDR3/LPDDR4, this sequence will include the necessary retraining code.\n");
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    
    dwc_ddrphy_phyinit_cmnt ("// Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0. \n");
    dwc_ddrphy_phyinit_cmnt ("// This allows the memory controller unrestricted access to the configuration CSRs. \n");
    dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

    dwc_ddrphy_phyinit_cmnt ("%s Programming PIE Production Code\n", printf_header);

    dwc_ddrphy_phyinit_LoadPieProdCode();

    /**
     * - Registers: Seq0BDLY0, Seq0BDLY1, Seq0BDLY2, Seq0BDLY3
     *   - Program PIE instruction delays 
     *   - Dependencies:
     *     - user_input_basic.Frequency
     */
    // Need delays for 0.5us, 1us, 10us, and 25us.
    uint16_t psCount[4][4];
    float delayScale = 1.00;

    // Calculate the counts to obtain the correct delay for each frequency
    // Need to divide by 4 since the delay value are specified in units of
    // 4 clocks.
    double DfiFrq,dllLock;

    for (pstate=0;pstate<userInputBasic.NumPStates;pstate++) {
      p_addr = pstate << 20;
      DfiFrq = (0.5 * userInputBasic.Frequency[pstate]);
      psCount[pstate][0] = (int)(( 0.5 * 0.25 * DfiFrq * delayScale));
      psCount[pstate][1] = (int)(( 1.0 * 0.25 * DfiFrq * delayScale));
      psCount[pstate][2] = (int)((10.0 * 0.25 * DfiFrq * delayScale));

      if (DfiFrq>266.5) { dllLock=176; } 
      else if (DfiFrq<=266.5 && DfiFrq>200) { dllLock=132; } 
      else { dllLock=64; }

      psCount[pstate][3] = (int)(0.25 * dllLock);

      dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d,  Memclk=%dMHz, Programming Seq0BDLY0 to 0x%x\n", printf_header, pstate,  userInputBasic.Frequency[pstate], psCount[pstate][0]);
      dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | c0 | tMASTER | csr_Seq0BDLY0_ADDR), psCount[pstate][0]);
      
      dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d,  Memclk=%dMHz, Programming Seq0BDLY1 to 0x%x\n", printf_header, pstate,  userInputBasic.Frequency[pstate], psCount[pstate][1]);
      dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | c0 | tMASTER | csr_Seq0BDLY1_ADDR), psCount[pstate][1]);
      
      dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d,  Memclk=%dMHz, Programming Seq0BDLY2 to 0x%x\n", printf_header, pstate,  userInputBasic.Frequency[pstate], psCount[pstate][2]);
      dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | c0 | tMASTER | csr_Seq0BDLY2_ADDR), psCount[pstate][2]);
      
      dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d,  Memclk=%dMHz, Programming Seq0BDLY3 to 0x%x\n", printf_header, pstate,  userInputBasic.Frequency[pstate], psCount[pstate][3]);
      dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | c0 | tMASTER | csr_Seq0BDLY3_ADDR), psCount[pstate][3]);

    }
    /**
     * - Registers: Seq0BDisableFlag0 Seq0BDisableFlag1 Seq0BDisableFlag2 
     *   Seq0BDisableFlag3 Seq0BDisableFlag4 Seq0BDisableFlag5
     *   - Program PIE Instruction Disable Flags 
     *   - Dependencies:
     */
    dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag0_ADDR), 0x0000);
    dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag1_ADDR), 0x0173);
    dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag2_ADDR), 0x0060);
    dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag3_ADDR), 0x6110);
    dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag4_ADDR), 0x2152);
    dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag5_ADDR), 0xDFBD);
    if(skip_training==1 || userInputAdvanced.DisableRetraining || userInputBasic.Frequency[0] < 333) {
        dwc_ddrphy_phyinit_cmnt ("%s Disabling DRAM drift compensation.\n", printf_header);
        dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag6_ADDR), 0xffff);
    } else {
        dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag6_ADDR), 0x2060);
    }
    dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_Seq0BDisableFlag7_ADDR), 0x6152);


    /**
     * - Registers: PPTTrainSetup and PPTTrainSetup2
     *   - Related to DFI PHY Master Interface (PMI). See Register description in PUB. 
     *   - Enable DFI PMI if training firmware was run
     *   - Fields:
     *     - PhyMstrTrainInterval
     *     - PhyMstrMaxReqToAck
     *     - PhyMstrFreqOverride
     *   - Dependencies:
     *     - skip_training
     *     - user_input_basic.Frequency
     *     - user_input_advanced.PhyMstrTrainInterval
     *     - user_input_advanced.PhyMstrMaxReqToAck
     */
    if (skip_training == 0) {
        dwc_ddrphy_phyinit_cmnt ("%s Enabling Phy Master Interface for DRAM drift compensation\n", printf_header);
        for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
            p_addr = pstate << 20;
            if (userInputBasic.Frequency[pstate] >= 666 ) { 
                PPTTrainSetup[pstate] = (userInputAdvanced.PhyMstrTrainInterval[pstate] << csr_PhyMstrTrainInterval_LSB) |
                                        (userInputAdvanced.PhyMstrMaxReqToAck[pstate] << csr_PhyMstrMaxReqToAck_LSB);
             
                dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming PPTTrainSetup::PhyMstrTrainInterval to 0x%x\n",
                       printf_header, pstate, userInputBasic.Frequency[pstate],
                       userInputAdvanced.PhyMstrTrainInterval[pstate]);
                dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming PPTTrainSetup::PhyMstrMaxReqToAck to 0x%x\n",
                       printf_header, pstate, userInputBasic.Frequency[pstate],
                       userInputAdvanced.PhyMstrMaxReqToAck[pstate]);
                dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming PPTTrainSetup2::PhyMstrFreqOverride to 0x3\n",
                       printf_header, pstate, userInputBasic.Frequency[pstate]);
                dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_PPTTrainSetup_ADDR), PPTTrainSetup[pstate]);
                dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_PPTTrainSetup2_ADDR), 0x0003);
            }
        }
    }


    /**
     * - Registers AcsmPlayback*x* 
     *   - Program Address/Command Sequence Engine (ACSM) registers with
     *     required instructions for retraining algorithm.
     *   - Dependencies:
     *     - user_input_basic.NumPStates
     */

    int acsmplayback[2][4] = {
        {0,0,0,0},
        {0,0,0,0}
    };
    int NumVec=0;
    int vec=0;
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++)
      {
      p_addr = pstate << 20;
      NumVec=0;
      acsmplayback[0][0] = 0x3<<6 | 0x20;
      acsmplayback[1][0] = 0x0<<6 | 0x12;
      acsmplayback[0][1] = 0x3<<6 | 0x20;
      acsmplayback[1][1] = 0x0<<6 | 0x12;
      acsmplayback[0][2] = 0x3<<6 | 0x20; 
      acsmplayback[1][2] = 0x0<<6 | 0x12;
      NumVec = 3;

      for (vec=0; vec<NumVec; vec++)
        {
        dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming AcsmPlayback0x%d to 0x%x\n", printf_header, pstate, vec, acsmplayback[0][vec]);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tACSM | (csr_AcsmPlayback0x0_ADDR+vec*2)), acsmplayback[0][vec]);
        
        dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming AcsmPlayback1x%d to 0x%x\n", printf_header, pstate, vec, acsmplayback[1][vec]);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tACSM | (csr_AcsmPlayback1x0_ADDR+vec*2)), acsmplayback[1][vec]);
        }
      }
    

    /**
     * Program Training Hardware Registers for mission mode retraining
     * and DRAM drift compensation algorithm.
     *
     */
    uint16_t regData;
    dwc_ddrphy_phyinit_cmnt ("%s Programing Training Hardware Registers for mission mode retraining\n", printf_header);
   /// - Register: AcsmCtrl13
   ///   - Fields: AcsmCkeEnb
    regData=(0xf << csr_AcsmCkeEnb_LSB);
    dwc_ddrphy_phyinit_userCustom_io_write16((c0 | tACSM | csr_AcsmCtrl13_ADDR), regData);
   /// - Register: TsmByte1, TsmByte2
   ///   - Dependencies: userInputBasic.NumDbyte
    unsigned int c_addr,i_addr;
    unsigned int byte;
    for (byte=0; byte < userInputBasic.NumDbyte ; byte++) { // for each chiplet
        c_addr = byte*c1;
        dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_TsmByte1_ADDR), 0x1); // [15:8] gstep; [7:0]bstep;
        dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_TsmByte2_ADDR), 0x1); // [15:0] good_bar;
        
        regData=(csr_DtsmStaticCmpr_MASK | csr_DtsmStaticCmprVal_MASK);
        
   /// - Register: TsmByte3, TsmByte5
   ///   - Fields:
   ///     - DtsmStaticCmpr
   ///     - DtsmStaticCmprVal
        dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_TsmByte3_ADDR), regData);
        dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_TsmByte5_ADDR), 0x1); // [15:0] bad_bar;
        
   /// - Register: TrainingParam
   ///   - Fields:
   ///     - EnDynRateReduction
   ///     - RollIntoCoarse
   ///     - IncDecRate
   ///     - TrainEnRxEn
   ///   - Dependencies:
   ///     - user_input_advanced.DisableRetraining
        regData = (csr_EnDynRateReduction_MASK | csr_RollIntoCoarse_MASK| (0x3 << csr_IncDecRate_LSB));
        regData = userInputAdvanced.DisableRetraining ? regData : (regData | csr_TrainEnRxEn_MASK);
        dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_TrainingParam_ADDR), regData ); 
        
   /// - Register: Tsm0
   ///   - Fields:
   ///     - DtsmEnb
        regData=(0x1 << csr_DtsmEnb_LSB ); 
        dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | i0 | tDBYTE | csr_Tsm0_ADDR), regData);
        
   /// - Register: Tsm2
   ///   - Fields:
   ///     - DtsmDisErrChk
        regData=(0x1 << csr_DtsmDisErrChk_LSB);
        for (vec=1; vec<=i_max ; vec++) {
            i_addr = vec*i1;
            dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | i_addr | tDBYTE | csr_Tsm2_ADDR), regData);
        } // for vec


    } // for byte




   /// - Register: CalZap
   ///   - Prepare the calibration controller for mission mode.
   ///     Turn on calibration and hold idle until dfi_init_start is asserted sequence is triggered.
    dwc_ddrphy_phyinit_cmnt ("%s Turn on calibration and hold idle until dfi_init_start is asserted sequence is triggered.\n", printf_header);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_CalZap_ADDR), 0x1);

   /// - Register: CalRate
   ///   - Fields:
   ///     - CalRun
   ///     - CalOnce
   ///     - CalInterval
   ///   - Dependencies 
   ///     - user_input_advanced.CalInterval 
   ///     - user_input_advanced.CalRun
   ///     - user_input_advanced.CalOnce

    int CalRate;
    int CalInterval;
    int CalOnce;

    CalInterval = userInputAdvanced.CalInterval;
    CalOnce = userInputAdvanced.CalOnce;

    CalRate = (0x1 << csr_CalRun_LSB) | (CalOnce << csr_CalOnce_LSB) | (CalInterval << csr_CalInterval_LSB);

    dwc_ddrphy_phyinit_cmnt ("%s Programming CalRate::CalInterval to 0x%x\n", printf_header, CalInterval);
    dwc_ddrphy_phyinit_cmnt ("%s Programming CalRate::CalOnce to 0x%x\n", printf_header, CalOnce);
    dwc_ddrphy_phyinit_cmnt ("%s Programming CalRate::CalRun to 0x%x\n", printf_header, 0x1);

    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_CalRate_ADDR), CalRate);

   /**
    * At the end of this function, PHY Clk gating register UcclkHclkEnables is
    * set for mission mode.  Additionally APB access is Isolated by setting 
    * MicroContMuxSel.
    */
    dwc_ddrphy_phyinit_cmnt ("%s Disabling Ucclk (PMU)\n", printf_header);
    dwc_ddrphy_phyinit_userCustom_io_write16((tDRTUB | csr_UcclkHclkEnables_ADDR), 0x2);
    
    dwc_ddrphy_phyinit_cmnt ("%s Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. \n", printf_header);
    dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);

    dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_I_loadPIEImage()\n", printf_header);

}
/** @} */
