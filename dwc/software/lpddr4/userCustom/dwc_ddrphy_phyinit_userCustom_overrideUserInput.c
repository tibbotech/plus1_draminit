/** \file 
 *  \brief used for overriding setDefault assignments
 *  \addtogroup CustFunc
 *  @{
 */
#include "dwc_ddrphy_phyinit.h"
#include <stdlib.h>

extern int CurrentPhyInst;

/** @brief overrides any field in PhyInit data structure dynamically.
 *
 * This function can be used to dynamically change PhyInit Data structures set
 * by dwc_ddrphy_setDefault(). 
 * 
 * To override user_input_basic, user_input_advanced and user_input_sim the user
 * can:
 * -# call dwc_ddrphy_phyinit_setUserInput(char *field, int value) 
 * -# directly assigning structure instances userInputBasic or userInputAdvanced.
 *
 * To override settings in the message block, users must use
 * dwc_ddrphy_phyinit_setMb().
 *
 * \warning some message bloc fields are set by dwc_ddrphy_phyinit_calcMb(). If
 * these fields are set directly in this function, the override will not take
 * effect. dwc_ddrphy_phyinit_setMb() must be used to keep the override message
 * block values.
 * 
 * @return Void
 */ 

void dwc_ddrphy_phyinit_userCustom_overrideUserInput () {
    char *printf_header;
    printf_header = " [dwc_ddrphy_phyinit_userCustom_overrideUserInput]";

    //dwc_ddrphy_phyinit_print ("\n");
    //dwc_ddrphy_phyinit_print ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("// dwc_ddrphy_phyinit_userCustom_overrideUserInput is a user-editable function.\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    //dwc_ddrphy_phyinit_print ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_print ("\n");
   // dwc_ddrphy_phyinit_print ("dwc_ddrphy_phyinit_userCustom_overrideUserInput ();\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");

    // == Definitions for overriding a single PHY system 
    // Example Values for testing

    // === Example to override frequency for P-State 0,1,2,3
    // === using dwc_ddrphy_phyinit_setUserInput()
    // dwc_ddrphy_phyinit_setUserInput ("Frequency[0]", 1600); // 3200Mbps 
    // dwc_ddrphy_phyinit_setUserInput ("Frequency[1]", 1200); // 2400Mbps
    // dwc_ddrphy_phyinit_setUserInput ("Frequency[2]", 933);  // 1866Mbps
    // dwc_ddrphy_phyinit_setUserInput ("Frequency[3]", 50);   // 100Mbps
    
    
    // === Example to override MemAlert related inputs using dwc_ddrphy_phyinit_setUserInput()
    // dwc_ddrphy_phyinit_setUserInput ("MemAlertEn", 0x1);
    // dwc_ddrphy_phyinit_setUserInput ("MemAlertPUImp", 0x8);
    // dwc_ddrphy_phyinit_setUserInput ("MemAlertVrefLevel", 32);
    // dwc_ddrphy_phyinit_setUserInput ("MemAlertSyncBypass", 0x1);

    
    // === Example to override frequency by setting the data structure
    // === directly
    // userInputBasic.Frequency[3]  = 1600;  // 3200Mbps 
    // userInputBasic.Frequency[2]  = 1200;  // 2400Mbps
    // userInputBasic.Frequency[1]  = 933;   // 1866Mbps
    // userInputBasic.Frequency[0]  = 50;    // 100Mbps
    
    
    // === Example to override MemAlert related inputs by setting the data
    // === structure directly
    // userInputAdvanced.MemAlertEn               = 0x0000;
    // userInputAdvanced.MemAlertPUImp            = 0x0005;
    // userInputAdvanced.MemAlertVrefLevel        = 0x0029;
    // userInputAdvanced.MemAlertSyncBypass       = 0x0000;

    // === Example to set HdtCtrl to 0x8 for 1D for pstates 0,1,2,3
    // === using dwc_ddrphy_phyinit_setMb()
    //dwc_ddrphy_phyinit_setMb (0, "HdtCtrl", 0x8, 0);
    //dwc_ddrphy_phyinit_setMb (1, "HdtCtrl", 0x8, 0);
    //dwc_ddrphy_phyinit_setMb (2, "HdtCtrl", 0x8, 0);
    //dwc_ddrphy_phyinit_setMb (3, "HdtCtrl", 0x8, 0);


    userInputBasic.DramType                 = LPDDR4;
    userInputBasic.DimmType                 = UDIMM;
    userInputBasic.NumDbyte                 = 0x0004;
    userInputBasic.NumActiveDbyteDfi0       = 0x0002;
    userInputBasic.NumAnib                  = 0x000a;
    userInputBasic.NumRank_dfi0             = 0x0002;
    userInputBasic.NumActiveDbyteDfi1       = 0x0002;
    userInputBasic.NumRank_dfi1             = 0x0002;
    userInputBasic.DramDataWidth            = 16;
    userInputBasic.NumPStates               = 0x0001;
    userInputBasic.Dfi1Exists               = 0x0001;
    userInputBasic.DfiMode                  = 0x0000;
    userInputBasic.Lp4xMode                 = 0x0000;
    userInputBasic.Frequency[0]             = 1600;
    userInputBasic.PllBypass[0]             = 0x0000;
    userInputBasic.DfiFreqRatio[0]          = 0x0001;
    userInputAdvanced.Lp4RxPreambleMode[0]  = 0x0000;
    userInputAdvanced.Lp4PostambleExt[0]    = 0x0001;
    userInputAdvanced.Lp4RL[0]              = 0x0005;
    userInputAdvanced.Lp4WL[0]              = 0x0005;
    userInputAdvanced.Lp4nWR[0]             = 0x0005;
    userInputAdvanced.Lp4WLS[0]             = 0x0000;
    userInputAdvanced.Lp4DbiRd[0]           = 0x0000;
    userInputAdvanced.Lp4DbiWr[0]           = 0x0000;
    userInputSim.tDQS2DQ                    = 200;
    userInputSim.tDQSCK                     = 1500;
    
        dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_userCustom_overrideUserInput()\n", printf_header);
    userInputBasic.HardMacroVer = 4; //HardMacro family E
    userInputBasic.PllBypass[0]  = 1;
    userInputBasic.PllBypass[1]  = 1;
    userInputBasic.PllBypass[2]  = 1;
    userInputBasic.PllBypass[3]  = 1;
    
userInputAdvanced.ODTImpedance[3] = 0;
userInputAdvanced.ODTImpedance[2] = 0;
userInputAdvanced.ODTImpedance[1] = 0;
userInputAdvanced.ODTImpedance[0] = 0;
}
/** @} */
