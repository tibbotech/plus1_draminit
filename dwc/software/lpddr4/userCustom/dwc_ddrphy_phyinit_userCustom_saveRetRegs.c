/** \file
 *  \brief routine to save registers for IO retention restore
 *  \addtogroup CustFunc
 *  @{
 */
#include <stdlib.h>
//#include <math.h>
#include "dwc_ddrphy_phyinit.h"

/** \brief This function can be used to implement saving of PHY registers to be
 * restored on retention exit.
 * 
 * The requirement of this function is to issue register reads and store the
 * value to be recovered on retention exit.  The following is an example
 * implementation and the user may implement alternate methods that suit their
 * specific SoC system needs.
 * 
 * In this implementation PhyInit saves register values in an internal C array.
 * During retention exit it restores register values from the array.  The exact
 * list of registers to save and later restore can be seen in the output txt
 * file with an associated calls to dwc_ddrphy_phyinit_usercustom_io_read16().
 * 
 * PhyInit provides a register interface and a tracking mechanism to minimize
 * the number registers needing restore. Refer to source code for
 * dwc_ddrphy_phyinit_regInterface() for detailed implementation of tracking
 * mechanism. Tracking is disabled from step D to Step H as these involve
 * loading, executing and checking the state of training firmware execution
 * which are not required to implement the retention exit sequence. The registers
 * specified in firmware training App Note representing training results are
 * also saved in addition to registers written by PhyInit during PHY
 * initialization.
 *
 * \returns \c void
 */
void dwc_ddrphy_phyinit_userCustom_saveRetRegs()
{
  char *printf_header;
  printf_header = "// [dwc_ddrphy_phyinit_userCustom_saveRetRegs]";

  dwc_ddrphy_phyinit_cmnt ("%s start of dwc_ddrphy_phyinit_userCustom_saveRetRegs()\n", printf_header);
  dwc_ddrphy_phyinit_cmnt ("\n\n");
  dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
  dwc_ddrphy_phyinit_cmnt ("//\n");
  dwc_ddrphy_phyinit_cmnt ("// Customer Save Retention Registers \n");
  dwc_ddrphy_phyinit_cmnt ("//\n");
  dwc_ddrphy_phyinit_cmnt ("// This function can be used to implement saving of PHY registers to be\n");
  dwc_ddrphy_phyinit_cmnt ("// restored on retention exit. the following list of register reads can\n");
  dwc_ddrphy_phyinit_cmnt ("// be used to compile the exact list of registers.\n");
  dwc_ddrphy_phyinit_cmnt ("//\n");
  dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
  dwc_ddrphy_phyinit_cmnt ("\n\n");

  /// In short the implementation of this function performs tasks:

  // --------------------------------------------------------------------------
  /// 1. Enable tracking of training firmware result registers\n
  ///    See firmware training App Note section "IO Retention" for reference
  ///    table registers that need to be saved. 
  ///
  ///    \note  The tagged registers in this step are in
  ///    addition to what is automatically tagged during Steps C to I.
  ///
  // --------------------------------------------------------------------------

  // 95% of users should not require to change the code below.
  int pstate;
  int anib;
  int byte;
  int nibble;
  int lane;
  int p_addr;
  int c_addr;
  int u_addr;
  int b_addr;
  int r_addr;
  
  dwc_ddrphy_phyinit_trackReg(tMASTER | csr_PllCtrl3_ADDR);

  // Non-PState Dbyte Registers
  for (byte=0; byte<userInputBasic.NumDbyte; byte++) 
    {
      c_addr = byte << 12;
      for (lane=0; lane <= r_max ; lane++)
        {
          r_addr = lane << 8;
              dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | r_addr | csr_RxPBDlyTg0_ADDR);
              dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | r_addr | csr_RxPBDlyTg1_ADDR);
        } // r_addr
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_PptCtlStatic_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_TrainingIncDecDtsmEn_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_TsmByte0_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_Dq0LnSel_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_Dq1LnSel_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_Dq2LnSel_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_Dq3LnSel_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_Dq4LnSel_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_Dq5LnSel_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_Dq6LnSel_ADDR);
      dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | csr_Dq7LnSel_ADDR);
    } // c_addr

  // PState variable registers
  for (pstate=0; pstate<userInputBasic.NumPStates; pstate++)
    {
      p_addr = pstate << 20;
      dwc_ddrphy_phyinit_trackReg(p_addr | tMASTER | csr_VrefInGlobal_ADDR);
      // Anig Registers
      for (anib=0; anib<userInputBasic.NumAnib; anib++) 
        {
          c_addr = anib << 12;
          dwc_ddrphy_phyinit_trackReg(p_addr | tANIB | c_addr | csr_ATxDly_ADDR);
        }
      // Dbyte Registers
      for (byte=0; byte<userInputBasic.NumDbyte; byte++) 
        {
          c_addr = byte << 12;
          dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | csr_DFIMRL_ADDR);
          for (nibble=0; nibble <= b_max ; nibble++)
            {
              b_addr = nibble << 8;
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | b_addr | csr_DqDqsRcvCntrl_ADDR);
            }
          for (nibble=0; nibble < 2; nibble++) 
            {
              u_addr = nibble << 8;
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg0_ADDR);
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg1_ADDR);
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg0_ADDR);
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg1_ADDR);
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg0_ADDR);
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg1_ADDR);
            } // nibble
          for (lane=r_min; lane <= r_max; lane++) 
            {
              r_addr = lane << 8;
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg0_ADDR);
              dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg1_ADDR);
            } // r_addr
          dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | csr_PptDqsCntInvTrnTg0_ADDR);
          dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | csr_PptDqsCntInvTrnTg1_ADDR);
        } // c_addr
      // PIE Registers
      dwc_ddrphy_phyinit_trackReg(p_addr | tINITENG | csr_Seq0BGPR1_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tINITENG | csr_Seq0BGPR2_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tINITENG | csr_Seq0BGPR3_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tINITENG | csr_Seq0BGPR4_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tINITENG | csr_Seq0BGPR5_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tINITENG | csr_Seq0BGPR6_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tINITENG | csr_Seq0BGPR7_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tINITENG | csr_Seq0BGPR8_ADDR);
      // Master Registers 
      dwc_ddrphy_phyinit_trackReg(p_addr | tMASTER | csr_DllGainCtl_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tMASTER | csr_DllLockParam_ADDR);
      dwc_ddrphy_phyinit_trackReg(p_addr | tMASTER | csr_HwtMRL_ADDR);
    } // p_addr

  // Master Registers 
  dwc_ddrphy_phyinit_trackReg(tMASTER | csr_HwtCAMode_ADDR);
  dwc_ddrphy_phyinit_trackReg(tMASTER | csr_HwtLpCsEnA_ADDR);
  dwc_ddrphy_phyinit_trackReg(tMASTER | csr_HwtLpCsEnB_ADDR);

  // ACSM registers
  dwc_ddrphy_phyinit_trackReg(tACSM | csr_AcsmCtrl13_ADDR);
  dwc_ddrphy_phyinit_trackReg(tACSM | csr_AcsmCtrl23_ADDR);


  if (runtimeConfig.Train2D) 
    {
      for (byte=0; byte<userInputBasic.NumDbyte; byte++) 
        {
          c_addr = byte << 12;

          for (pstate=0; pstate<userInputBasic.NumPStates; pstate++)
            {
              p_addr = pstate << 20;
              for (nibble=0; nibble < 2; nibble++) 
                {
                  u_addr = nibble << 8;
                  dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg0_ADDR);
                  dwc_ddrphy_phyinit_trackReg(p_addr | tDBYTE | c_addr | u_addr | csr_RxClkDlyTg1_ADDR);
                } // nibble
            } // p_addr

        for (lane=0; lane <= r_max ; lane++)
          {
            r_addr = lane << 8;
                  dwc_ddrphy_phyinit_trackReg(tDBYTE | c_addr | r_addr | csr_VrefDAC0_ADDR);
          }

        } // byte
    }
  // --------------------------------------------------------------------------
  /// 2. Track any additional registers\n 
  ///    Register writes made using the any of the PhyInit functions are
  ///    automatically tracked using the call to dwc_ddrphy_phyinit_trackReg() in
  ///    dwc_ddrphy_phyinit_userCustom_io_write16(). Use this section to track
  ///     additional registers.
  // --------------------------------------------------------------------------

  /// Example:
  /// dwc_ddrphy_phyinit_trackReg(<addr>);

  // --------------------------------------------------------------------------
  /// 3. Prepare for register reads\n 
  ///    - Write the MicroContMuxSel CSR to 0x0 to allow access to the internal CSRs
  ///    - Write the UcclkHclkEnables CSR to 0x3 to enable all the clocks so the reads can complete
  // --------------------------------------------------------------------------

  dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);
  dwc_ddrphy_phyinit_userCustom_io_write16((tDRTUB | csr_UcclkHclkEnables_ADDR), 0x3);

  // --------------------------------------------------------------------------
  /// 4. Read and save all the registers
  ///    - The list of registers differ depending on protocol and 1D or 1D+2D training. 
  // --------------------------------------------------------------------------

  dwc_ddrphy_phyinit_regInterface(saveRegs,0,0);

  // --------------------------------------------------------------------------
  /// 5. Prepare for mission mode
  ///    - Write the UcclkHclkEnables CSR to disable the appropriate clocks after all reads done.
  ///    - Write the MicroContMuxSel CSR to 0x1 to isolate the internal CSRs during mission mode
  // --------------------------------------------------------------------------

    dwc_ddrphy_phyinit_cmnt ("// Disabling Ucclk (PMU)\n");
    dwc_ddrphy_phyinit_userCustom_io_write16((tDRTUB | csr_UcclkHclkEnables_ADDR), 0x2);

  dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);

  dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_userCustom_saveRetRegs()\n", printf_header);
  return;
}

/** @} */
