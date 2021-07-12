/** \file 
 * \brief calculates messageBlock header based on user_input_basic and user_input_advanced.
 */

#include <stdlib.h>
#include "dwc_ddrphy_phyinit.h"

extern PMU_SMB_LPDDR4_1D_t    mb_LPDDR4_1D[4];
extern PMU_SMB_LPDDR4_2D_t    mb_LPDDR4_2D[4];

/**
 *  \addtogroup SrcFunc
 *  @{
 */

/** \brief reads PhyInit inputs structures and sets relevant message bloc
 * parameters.
 * 
 * This function sets Message Block parameters based on user_input_basic and 
 * user_input_advanced. Parameters are only set if not programed by
 * dwc_ddrphy_phyinit_userCustom_overrideUserInput() or
 * dwc_ddrphy_phyinit_setDefault(). user changes in these files takes precedence
 * over this function call.
 * 
 * MessageBlock fields set ::
 *
 *  - DramType
 *  - Pstate              
 *  - DRAMFreq            
 *  - PllBypassEn         
 *  - DfiFreqRatio        
 *  - PhyOdtImpedance     
 *  - PhyDrvImpedance     
 *  - BPZNResVal          
 *  - EnabledDQsChA    
 *  - CsPresentChA     
 *  - EnabledDQsChB    
 *  - CsPresentChB     
 * 
 * \return void
 */
void dwc_ddrphy_phyinit_calcMb (int Train2D) { 

    //char *printf_header;
    //printf_header = "//[dwc_ddrphy_phyinit_calcMb]";

    //dwc_ddrphy_phyinit_print ("%s Start of dwc_ddrphy_phyinit_calcMb()\n", printf_header);

    int nad0 = userInputBasic.NumActiveDbyteDfi0;
    int nad1 = 0;
    nad1 = userInputBasic.NumActiveDbyteDfi1;

    // a few checks to make sure valid programing.
    if ( nad0 <= 0 || 
         nad1 < 0 ||
         userInputBasic.NumDbyte <= 0) { 
      //dwc_ddrphy_phyinit_assert(0,"%s NumActiveDbyteDfi0, NumActiveDbyteDfi0, NumByte out of range.\n",printf_header);
      }

    if ( (nad0 + nad1) > userInputBasic.NumDbyte) { 
      //dwc_ddrphy_phyinit_assert(0,"%s NumActiveDbyteDfi0+NumActiveDbyteDfi1 is larger than NumDbyteDfi0\n",printf_header);
      }

    if (userInputBasic.Dfi1Exists==0 && nad1 != 0) {
      //dwc_ddrphy_phyinit_assert(0,"%s Dfi1Exists==0 but NumDbyteDfi0 != 0\n",printf_header);
      }
     
   
    uint8_t myps;

    // 1D message block defaults
    for (myps=0; myps<userInputBasic.NumPStates; myps++) {
   
      if (userInputBasic.DramDataWidth == 8 && mb_LPDDR4_1D[myps].X8Mode == 0x0) {
        //dwc_ddrphy_phyinit_assert(0,"%s LPDDR4 DramDataWidth == 8 but no X8 devices programmed in mb_LPDDR4_1D[%d].X8Mode!\n",printf_header,myps);
      }


      dwc_ddrphy_phyinit_softSetMb(myps,"Pstate",myps,0);
      dwc_ddrphy_phyinit_softSetMb(myps,"DRAMFreq",userInputBasic.Frequency[myps] * 2,0);
      dwc_ddrphy_phyinit_softSetMb(myps,"PllBypassEn",userInputBasic.PllBypass[myps],0);
      dwc_ddrphy_phyinit_softSetMb(myps,"DfiFreqRatio",(0==userInputBasic.DfiFreqRatio[myps]) ? 0x1 : ((1==userInputBasic.DfiFreqRatio[myps]) ? 0x2 : 0x4),0); 
    if (userInputBasic.HardMacroVer == 4 )  { //HardMacroE
      dwc_ddrphy_phyinit_softSetMb(myps,"PhyOdtImpedance",0,0);
      dwc_ddrphy_phyinit_softSetMb(myps,"PhyDrvImpedance",0,0);
    } else {
      dwc_ddrphy_phyinit_softSetMb(myps,"PhyOdtImpedance",0,0);
      dwc_ddrphy_phyinit_softSetMb(myps,"PhyDrvImpedance",0,0);
    }
      dwc_ddrphy_phyinit_softSetMb(myps,"BPZNResVal",0,0);
  
      dwc_ddrphy_phyinit_softSetMb(myps,"EnabledDQsChA",nad0*8,0);
      dwc_ddrphy_phyinit_softSetMb(myps,"CsPresentChA",(2==userInputBasic.NumRank_dfi0) ? 0x3 : userInputBasic.NumRank_dfi0,0);
      dwc_ddrphy_phyinit_softSetMb(myps,"EnabledDQsChB",nad1*8,0);
      dwc_ddrphy_phyinit_softSetMb(myps,"CsPresentChB",(2==userInputBasic.NumRank_dfi1) ? 0x3 : userInputBasic.NumRank_dfi1,0);
    } // myps

    // 2D message block defaults
    if (Train2D) {
    for (myps=0; myps<1; myps++) {
   
      if (userInputBasic.DramDataWidth == 8 && mb_LPDDR4_2D[myps].X8Mode == 0x0) {
        //dwc_ddrphy_phyinit_assert(0,"%s LPDDR4 DramDataWidth == 8 but no X8 devices programmed in mb_LPDDR4_2D[%d].X8Mode!\n",printf_header,myps);
      }


        dwc_ddrphy_phyinit_softSetMb(myps,"Pstate",myps,1);
        dwc_ddrphy_phyinit_softSetMb(myps,"DRAMFreq",userInputBasic.Frequency[myps] * 2,1);
        dwc_ddrphy_phyinit_softSetMb(myps,"PllBypassEn",userInputBasic.PllBypass[myps],1);
        dwc_ddrphy_phyinit_softSetMb(myps,"DfiFreqRatio",(0==userInputBasic.DfiFreqRatio[myps]) ? 0x1 : ((1==userInputBasic.DfiFreqRatio[myps]) ? 0x2 : 0x4),1); 
    if (userInputBasic.HardMacroVer == 4 )  { //HardMacroE
        dwc_ddrphy_phyinit_softSetMb(myps,"PhyOdtImpedance",0,1);
        dwc_ddrphy_phyinit_softSetMb(myps,"PhyDrvImpedance",0,1);
    } else {
        dwc_ddrphy_phyinit_softSetMb(myps,"PhyOdtImpedance",0,1);
        dwc_ddrphy_phyinit_softSetMb(myps,"PhyDrvImpedance",0,1);
    }
        dwc_ddrphy_phyinit_softSetMb(myps,"BPZNResVal",0,1);
  
        dwc_ddrphy_phyinit_softSetMb(myps,"EnabledDQsChA",nad0*8,1);
        dwc_ddrphy_phyinit_softSetMb(myps,"CsPresentChA",(2==userInputBasic.NumRank_dfi0) ? 0x3 : userInputBasic.NumRank_dfi0,1);
        dwc_ddrphy_phyinit_softSetMb(myps,"EnabledDQsChB",nad1*8,1);
        dwc_ddrphy_phyinit_softSetMb(myps,"CsPresentChB",(2==userInputBasic.NumRank_dfi1) ? 0x3 : userInputBasic.NumRank_dfi1,1);
      } // myps
    } // Train2D

    //dwc_ddrphy_phyinit_print ("%s End of dwc_ddrphy_phyinit_calcMb()\n", printf_header);
}
/** @} */
