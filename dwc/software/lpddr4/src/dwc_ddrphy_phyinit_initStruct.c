
/** \file 
 * \brief initializes PhyInit input data structures to sane values.
 */
//#include<string.h>
#include "dwc_ddrphy_phyinit.h"

/**  
 * 1D Message Block 
 */
extern PMU_SMB_LPDDR4_1D_t    mb_LPDDR4_1D[4];
/**  
 * 2D Message Block 
 */
extern PMU_SMB_LPDDR4_2D_t    mb_LPDDR4_2D[4];

/**
 *  \addtogroup SrcFunc
 *  @{
 */

/** @brief  This is used to initialize the PhyInit structures before user defaults and overrides are applied. 
 * 
 * @return Void
 */

typedef unsigned char		UINT8;
void *memset(UINT8 *s1, int c, int n)
{
	UINT8 *s1_end = s1 + n;

	while (s1 != s1_end)
		*s1++ = c;

	return s1;
}

void dwc_ddrphy_phyinit_initStruct (int Train2D /**< Train2D 1=1D & 2D training enabled, 0=only 1D training. */) {
    
    //char *printf_header;
    //printf_header = "// [dwc_ddrphy_phyinit_initStruct]";

    //dwc_ddrphy_phyinit_print ("%s Start of dwc_ddrphy_phyinit_initStruct()\n", printf_header);
#if 0
	memset((void *) &userInputBasic, 0, sizeof(userInputBasic)); // Zero out struct contents 
    memset((void *) &userInputAdvanced, 0, sizeof(userInputAdvanced)); // Zero out struct contents 
    memset((void *) &userInputSim, 0, sizeof(userInputSim)); // Zero out struct contents 
#else
	memset((UINT8 *) &userInputBasic, 0, sizeof(userInputBasic)); // Zero out struct contents 
    memset((UINT8 *) &userInputAdvanced, 0, sizeof(userInputAdvanced)); // Zero out struct contents 
    memset((UINT8 *) &userInputSim, 0, sizeof(userInputSim)); // Zero out struct contents 
#endif 
    // ##############################################################
    // userInputBasic - Basic Inputs the user must provide values
    // for detailed descriptions of each field see src/dwc_ddrphy_phyinit_struct.h
    // ##############################################################
    userInputBasic.DramType                 = LPDDR4;
    userInputBasic.DimmType                 = NODIMM;
    userInputBasic.HardMacroVer             = 3; //default: HardMacro family D 

    userInputBasic.Lp4xMode                 = 0x0000;
    userInputBasic.NumDbyte                 = 0x0004;
    userInputBasic.NumActiveDbyteDfi0       = 0x0002;
    userInputBasic.NumActiveDbyteDfi1       = 0x0002;
    userInputBasic.NumAnib                  = 0x0006;
    userInputBasic.NumRank_dfi0             = 0x0001; // 2 ranks each controlled by dfi0 and dfi1 correspondingly.
    userInputBasic.NumRank_dfi1             = 0x0001; // 
    userInputBasic.NumPStates               = 0x0001; // 1 Pstate
    userInputBasic.Frequency[3]             = 800; 
    userInputBasic.Frequency[2]             = 933; 
    userInputBasic.Frequency[1]             = 1067; 
    userInputBasic.Frequency[0]             = 1600; // 3200Mbps
    userInputBasic.PllBypass[0]             = 0x0000;
    userInputBasic.PllBypass[1]             = 0x0000;
    userInputBasic.PllBypass[2]             = 0x0000;
    userInputBasic.PllBypass[3]             = 0x0000;
    userInputBasic.DfiFreqRatio[0]          = 0x0001;
    userInputBasic.DfiFreqRatio[1]          = 0x0001;
    userInputBasic.DfiFreqRatio[2]          = 0x0001;
    userInputBasic.DfiFreqRatio[3]          = 0x0001;
    userInputBasic.Dfi1Exists               = 0x0001;
    userInputBasic.DramDataWidth            = 0x0010; //x16

    // ##############################################################
    // userInputAdvnaced (Optional)  
    // Default values will be used if no input provided
    // ##############################################################

    userInputAdvanced.DramByteSwap             = 0x0000; 
    userInputAdvanced.ExtCalResVal             = 0x0000;
    userInputAdvanced.TxSlewRiseDQ[0]          = 0x000f; 
    userInputAdvanced.TxSlewRiseDQ[1]          = 0x000f; 
    userInputAdvanced.TxSlewRiseDQ[2]          = 0x000f; 
    userInputAdvanced.TxSlewRiseDQ[3]          = 0x000f; 
    userInputAdvanced.TxSlewFallDQ[0]          = 0x000f; 
    userInputAdvanced.TxSlewFallDQ[1]          = 0x000f; 
    userInputAdvanced.TxSlewFallDQ[2]          = 0x000f; 
    userInputAdvanced.TxSlewFallDQ[3]          = 0x000f; 
    userInputAdvanced.TxSlewRiseAC             = 0x000f; 
    userInputAdvanced.TxSlewFallAC             = 0x000f; 
    userInputAdvanced.ODTImpedance[0]          = 60;      
    userInputAdvanced.ODTImpedance[1]          = 60; 
    userInputAdvanced.ODTImpedance[2]          = 60; 
    userInputAdvanced.ODTImpedance[3]          = 60; 
    userInputAdvanced.TxImpedance[0]           = 60; 
    userInputAdvanced.TxImpedance[1]           = 60; 
    userInputAdvanced.TxImpedance[2]           = 60; 
    userInputAdvanced.TxImpedance[3]           = 60;     
    userInputAdvanced.ATxImpedance             = 20;      //with HardMacro family E, ATxImpedance will default to 40 Ohm instead       
    userInputAdvanced.MemAlertEn               = 0x0000;
    userInputAdvanced.MemAlertPUImp            = 0x0005;
    userInputAdvanced.MemAlertVrefLevel        = 0x0029;
    userInputAdvanced.MemAlertSyncBypass       = 0x0000;
    userInputAdvanced.CalInterval              = 0x0009;
    userInputAdvanced.CalOnce                  = 0x0000;

    userInputAdvanced.DisDynAdrTri[0]          = 0x0001;
    userInputAdvanced.DisDynAdrTri[1]          = 0x0001;
    userInputAdvanced.DisDynAdrTri[2]          = 0x0001;
    userInputAdvanced.DisDynAdrTri[3]          = 0x0001;
    userInputAdvanced.WDQSExt                  = 0x0000;

    userInputAdvanced.RxEnBackOff              = 0x0001; 
    userInputAdvanced.PhyMstrTrainInterval[0]  = 0x000a;
    userInputAdvanced.PhyMstrTrainInterval[1]  = 0x000a;
    userInputAdvanced.PhyMstrTrainInterval[2]  = 0x000a;
    userInputAdvanced.PhyMstrTrainInterval[3]  = 0x000a;
    userInputAdvanced.PhyMstrMaxReqToAck[0]    = 0x0005;
    userInputAdvanced.PhyMstrMaxReqToAck[1]    = 0x0005;
    userInputAdvanced.PhyMstrMaxReqToAck[2]    = 0x0005;
    userInputAdvanced.PhyMstrMaxReqToAck[3]    = 0x0005;

    userInputAdvanced.Lp4RxPreambleMode[0]     = 0x0000;
    userInputAdvanced.Lp4RxPreambleMode[1]     = 0x0000;
    userInputAdvanced.Lp4RxPreambleMode[2]     = 0x0000;
    userInputAdvanced.Lp4RxPreambleMode[3]     = 0x0000;
    userInputAdvanced.Lp4PostambleExt[0]       = 0x0001;
    userInputAdvanced.Lp4PostambleExt[1]       = 0x0001;
    userInputAdvanced.Lp4PostambleExt[2]       = 0x0001;
    userInputAdvanced.Lp4PostambleExt[3]       = 0x0001;
    userInputAdvanced.Lp4DbiRd[0]              = 0x0000;
    userInputAdvanced.Lp4DbiRd[1]              = 0x0000;
    userInputAdvanced.Lp4DbiRd[2]              = 0x0000;
    userInputAdvanced.Lp4DbiRd[3]              = 0x0000;
    userInputAdvanced.Lp4DbiWr[0]              = 0x0000;
    userInputAdvanced.Lp4DbiWr[1]              = 0x0000;
    userInputAdvanced.Lp4DbiWr[2]              = 0x0000;
    userInputAdvanced.Lp4DbiWr[3]              = 0x0000;
    userInputAdvanced.Lp4WLS[0]                = 0x0000;
    userInputAdvanced.Lp4WLS[1]                = 0x0000;
    userInputAdvanced.Lp4WLS[2]                = 0x0000;
    userInputAdvanced.Lp4WLS[3]                = 0x0000;
    int pstate;
    for (pstate=0; pstate<userInputBasic.NumPStates;pstate++) {
        if ( userInputBasic.Frequency[pstate] <= 266 ) {
            userInputAdvanced.Lp4RL[pstate]         = 0x0000; 
            userInputAdvanced.Lp4WL[pstate]         = 0x0000; 
            userInputAdvanced.Lp4nWR[pstate]        = 0x0000; 
        }
        else if ( userInputBasic.Frequency[pstate] <= 533) {
            userInputAdvanced.Lp4RL[pstate]         = 0x0001; 
            userInputAdvanced.Lp4WL[pstate]         = 0x0001; 
            userInputAdvanced.Lp4nWR[pstate]        = 0x0001; 
        }
        else if ( userInputBasic.Frequency[pstate] <= 800) {
            userInputAdvanced.Lp4RL[pstate]         = 0x0002; 
            userInputAdvanced.Lp4WL[pstate]         = 0x0002; 
            userInputAdvanced.Lp4nWR[pstate]        = 0x0002; 
        }
        else if ( userInputBasic.Frequency[pstate] <= 1066) { 
            userInputAdvanced.Lp4RL[pstate]         = 0x0003;
            userInputAdvanced.Lp4WL[pstate]         = 0x0003;
            userInputAdvanced.Lp4nWR[pstate]        = 0x0003; 
        }
        else if ( userInputBasic.Frequency[pstate] <= 1333) {
            userInputAdvanced.Lp4RL[pstate]         = 0x0004;
            userInputAdvanced.Lp4WL[pstate]         = 0x0004;
            userInputAdvanced.Lp4nWR[pstate]        = 0x0004; 
        }
        else if ( userInputBasic.Frequency[pstate] <= 1600) {
            userInputAdvanced.Lp4RL[pstate]         = 0x0005;
            userInputAdvanced.Lp4WL[pstate]         = 0x0005;
            userInputAdvanced.Lp4nWR[pstate]        = 0x0005; 
        }
        else if ( userInputBasic.Frequency[pstate] <= 1866) {
            userInputAdvanced.Lp4RL[pstate]         = 0x0006;
            userInputAdvanced.Lp4WL[pstate]         = 0x0006;
            userInputAdvanced.Lp4nWR[pstate]        = 0x0006; 
        } 
        else {
            userInputAdvanced.Lp4RL[pstate]         = 0x0007;
            userInputAdvanced.Lp4WL[pstate]         = 0x0007;
            userInputAdvanced.Lp4nWR[pstate]        = 0x0007; 
        }
    }

    userInputAdvanced.EnableDfiCsPolarityFix   = 0;

    // ##############################################################
    // Basic Message Block Variables
    // ##############################################################
    
    uint8_t myps;

    // ##############################################################
    // These are typically invariant across Pstate
    // ##############################################################
    uint8_t MsgMisc              = 0x06; //For fast simulation
    uint8_t Reserved00           = 0x0;  // Set Reserved00[7]   = 1 (If using T28 attenuated receivers)
                                         // Set Reserved00[6:0] = 0 (Reserved; must be programmed to 0)
                                          
    uint8_t HdtCtrl              = 0xff;
    uint8_t PhyVref              = 0x14;
    uint8_t CATerminatingRankChA = 0x00; //Usually Rank0 is terminating rank
    uint8_t CATerminatingRankChB = 0x00; //Usually Rank0 is terminating rank
    uint8_t DFIMRLMargin         = 0x02; //This needs to be large enough for max tDQSCK variation
    uint8_t PuCal                = 0x01;
    uint8_t Share2DVrefResult    = 0x1;  // Bitmap that controls which vref generator the phy will use per pstate
                                         //     If Share2DVrefResult[x] = 1, pstate x will use the per-lane VrefDAC0/1 CSRs which can be trained by 2d training.    
                                         //                                  If 2D has not run yet, VrefDAC0/1 will default to pstate 0's 1D phyVref messageBlock setting.
                                         //     If Share2DVrefResult[x] = 0, pstate x will use the per-phy VrefInGlobal CSR, which are set to pstate x's 1D phyVref messageBlock setting. 
    uint8_t UseBroadcastMR       = 0x00;
    // ##############################################################
    // These typically change across Pstate
    // ##############################################################
    
    uint16_t SequenceCtrl[4];
  
    SequenceCtrl[0] = 0x131f; // Training steps to run in PState 0
    SequenceCtrl[1] = 0x121f; // Training steps to run in PState 1
    SequenceCtrl[2] = 0x121f; // Training steps to run in PState 2
    SequenceCtrl[3] = 0x121f; // Training steps to run in PState 3
    
    uint16_t PDDS[4] = {0x6,0x6,0x6,0x6};
    uint16_t DqOdt[4] = {0x4,0x4,0x4,0x4};
    uint16_t CaOdt[4] = {0x6,0x6,0x6,0x6};
    uint16_t VrefCa[4] = {0xd,0xd,0xd,0xd};
    uint16_t VrefCaRange[4] = {0x1,0x1,0x1,0x1};
    uint16_t VrefDq[4] = {0xf,0xf,0xf,0xf};
    uint16_t VrefDqRange[4] = {0x1,0x1,0x1,0x1};
    uint16_t SocOdt[4] = {0x4,0x4,0x4,0x4};
    uint16_t CkOdtEn[4] = {0x0,0x0,0x0,0x0};
    uint16_t CsOdtEn[4] = {0x0,0x0,0x0,0x0};
    uint16_t CaOdtDis[4] = {0x0,0x0,0x0,0x0};

    // itterate through all pstates to write the MR's
    uint8_t  mr1[4];     
    uint8_t  mr2[4];     
    uint8_t  mr3[4];     
    uint8_t  mr4[4];     
    uint8_t mr11[4];     
    uint8_t mr12[4];     
    uint8_t mr13[4];     
    uint8_t mr14[4];     
    uint8_t mr16[4];     
    uint8_t mr17[4];     
    uint8_t mr22[4];     
    uint8_t mr24[4];
    
    
    // 2D Training firmware Variables
    uint8_t  SequenceCtrl2D[4];
    SequenceCtrl2D[0] = 0x0061; // 2D Training Sequince. 2DTX, 2DRX, DevInit
    SequenceCtrl2D[1] = 0x0000; 
    SequenceCtrl2D[2] = 0x0000;
    SequenceCtrl2D[3] = 0x0000;

    // ##############################################################
    // These are per-pstate Control Words for RCD
    // Please enter the correct values for your configuration
    // ##############################################################
      



    // ##############################################################
    // 95% of users will not need to edit below
    // ##############################################################
    // MR bit packing for LPDDR4
    for ( myps = 0; myps < 4 ; myps++) {
        mr1[myps] =
            ( (0x0                                       << 0) & 0x03) |
            ( (0x1                                       << 2) & 0x04) |
            ( (userInputAdvanced.Lp4RxPreambleMode[myps] << 3) & 0x08) |
            ( (userInputAdvanced.Lp4nWR[myps]            << 4) & 0x70) |
            ( (0x0                                       << 7) & 0x80) ;
    
        mr2[myps] =
            ( (userInputAdvanced.Lp4RL[myps]            << 0) & 0x07) |
            ( (userInputAdvanced.Lp4WL[myps]            << 3) & 0x38) |
            ( (0x0                                      << 6) & 0x40) |
            ( ((SequenceCtrl[myps] & 0x2)               << 7) & 0x80) ;
    
        mr3[myps] =
            ( (PuCal                                    << 0) & 0x01) |
            ( (userInputAdvanced.Lp4PostambleExt[myps]  << 1) & 0x02) |
            ( (0x0                                      << 2) & 0x04) |
            ( (PDDS[myps]                               << 3) & 0x38) |
            ( (userInputAdvanced.Lp4DbiRd[myps]         << 6) & 0x40) |
            ( (userInputAdvanced.Lp4DbiWr[myps]         << 7) & 0x80) ;
    
        mr4[myps] = 0x0;
    
        mr11[myps] =
            ( (DqOdt[myps]            << 0) & 0x07) |
            ( (CaOdt[myps]            << 4) & 0x70) ;
    
        mr12[myps] =
            ( (VrefCa[myps]           << 0) & 0x3f) |
            ( (VrefCaRange[myps]      << 6) & 0x40) ;
    
        mr13[myps] =
            ( (0x0           << 0) & 0x01) | // CBT
            ( (0x0           << 1) & 0x02) | // RPT
            ( (0x0           << 2) & 0x04) | // VRO
            ( (0x1           << 3) & 0x08) | // VRCG
            ( (0x0           << 4) & 0x10) | // RRO
            ( (0x1           << 5) & 0x20) | // DMD
            ( (0x0           << 6) & 0x40) | // FSP-WR
            ( (0x0           << 7) & 0x80) ; // FSP-OP
    
        mr14[myps] =
            ( (VrefDq[myps]           << 0) & 0x3f) |
            ( (VrefDqRange[myps]      << 6) & 0x40) ;
    
        mr16[myps] = 0x0;
        mr17[myps] = 0x0;
    
        mr22[myps] =
            ( (SocOdt[myps]      << 0) & 0x07) |
            ( (CkOdtEn[myps]     << 3) & 0x08) |
            ( (CsOdtEn[myps]     << 4) & 0x10) |
            ( (CaOdtDis[myps]    << 5) & 0x20) |
            ( (0x0               << 6) & 0xc0) ;
        mr24[myps] = 0x0;
    } // myps 



    // 1D message block defaults
    for (myps=0; myps<4; myps++) {
      mb_LPDDR4_1D[myps].Pstate               = myps;
      mb_LPDDR4_1D[myps].SequenceCtrl         = SequenceCtrl[myps];
      mb_LPDDR4_1D[myps].PhyConfigOverride    = 0x0;
      mb_LPDDR4_1D[myps].HdtCtrl              = HdtCtrl;
      mb_LPDDR4_1D[myps].MsgMisc              = MsgMisc;
      mb_LPDDR4_1D[myps].Reserved00           = Reserved00;
      mb_LPDDR4_1D[myps].DFIMRLMargin         = DFIMRLMargin;
      mb_LPDDR4_1D[myps].PhyVref              = PhyVref;
  
      mb_LPDDR4_1D[myps].EnabledDQsChA        = userInputBasic.NumActiveDbyteDfi0 * 8;
      mb_LPDDR4_1D[myps].CsPresentChA         = (2==userInputBasic.NumRank_dfi0) ? 0x3 : userInputBasic.NumRank_dfi0;
      mb_LPDDR4_1D[myps].EnabledDQsChB        = userInputBasic.NumActiveDbyteDfi1 * 8;
      mb_LPDDR4_1D[myps].CsPresentChB         = (2==userInputBasic.NumRank_dfi1) ? 0x3 : userInputBasic.NumRank_dfi1;
      mb_LPDDR4_1D[myps].UseBroadcastMR       = UseBroadcastMR;

      mb_LPDDR4_1D[myps].Lp4Misc              = 0x0000;
      mb_LPDDR4_1D[myps].CATerminatingRankChA = CATerminatingRankChA;
      mb_LPDDR4_1D[myps].CATerminatingRankChB = CATerminatingRankChB;
      mb_LPDDR4_1D[myps].Lp4Quickboot         = 0x0000;
      mb_LPDDR4_1D[myps].CATrainOpt           = 0x0000;
      mb_LPDDR4_1D[myps].X8Mode               = 0x0000;

      mb_LPDDR4_1D[myps].MR1_A0               = mr1[myps]; 
      mb_LPDDR4_1D[myps].MR2_A0               = mr2[myps]; 
      mb_LPDDR4_1D[myps].MR3_A0               = mr3[myps]; 
      mb_LPDDR4_1D[myps].MR4_A0               = mr4[myps]; 
      mb_LPDDR4_1D[myps].MR11_A0              = mr11[myps];
      mb_LPDDR4_1D[myps].MR12_A0              = mr12[myps];
      mb_LPDDR4_1D[myps].MR13_A0              = mr13[myps];
      mb_LPDDR4_1D[myps].MR14_A0              = mr14[myps];
      mb_LPDDR4_1D[myps].MR16_A0              = mr16[myps];
      mb_LPDDR4_1D[myps].MR17_A0              = mr17[myps];
      mb_LPDDR4_1D[myps].MR22_A0              = mr22[myps];
      mb_LPDDR4_1D[myps].MR24_A0              = mr24[myps];
      mb_LPDDR4_1D[myps].MR1_A1               = mr1[myps]; 
      mb_LPDDR4_1D[myps].MR2_A1               = mr2[myps]; 
      mb_LPDDR4_1D[myps].MR3_A1               = mr3[myps]; 
      mb_LPDDR4_1D[myps].MR4_A1               = mr4[myps]; 
      mb_LPDDR4_1D[myps].MR11_A1              = mr11[myps];
      mb_LPDDR4_1D[myps].MR12_A1              = mr12[myps];
      mb_LPDDR4_1D[myps].MR13_A1              = mr13[myps];
      mb_LPDDR4_1D[myps].MR14_A1              = mr14[myps];
      mb_LPDDR4_1D[myps].MR16_A1              = mr16[myps];
      mb_LPDDR4_1D[myps].MR17_A1              = mr17[myps];
      mb_LPDDR4_1D[myps].MR22_A1              = mr22[myps];
      mb_LPDDR4_1D[myps].MR24_A1              = mr24[myps];
      
      mb_LPDDR4_1D[myps].MR1_B0               = mr1[myps]; 
      mb_LPDDR4_1D[myps].MR2_B0               = mr2[myps]; 
      mb_LPDDR4_1D[myps].MR3_B0               = mr3[myps]; 
      mb_LPDDR4_1D[myps].MR4_B0               = mr4[myps]; 
      mb_LPDDR4_1D[myps].MR11_B0              = mr11[myps];
      mb_LPDDR4_1D[myps].MR12_B0              = mr12[myps];
      mb_LPDDR4_1D[myps].MR13_B0              = mr13[myps];
      mb_LPDDR4_1D[myps].MR14_B0              = mr14[myps];
      mb_LPDDR4_1D[myps].MR16_B0              = mr16[myps];
      mb_LPDDR4_1D[myps].MR17_B0              = mr17[myps];
      mb_LPDDR4_1D[myps].MR22_B0              = mr22[myps];
      mb_LPDDR4_1D[myps].MR24_B0              = mr24[myps];
      mb_LPDDR4_1D[myps].MR1_B1               = mr1[myps]; 
      mb_LPDDR4_1D[myps].MR2_B1               = mr2[myps]; 
      mb_LPDDR4_1D[myps].MR3_B1               = mr3[myps]; 
      mb_LPDDR4_1D[myps].MR4_B1               = mr4[myps]; 
      mb_LPDDR4_1D[myps].MR11_B1              = mr11[myps];
      mb_LPDDR4_1D[myps].MR12_B1              = mr12[myps];
      mb_LPDDR4_1D[myps].MR13_B1              = mr13[myps];
      mb_LPDDR4_1D[myps].MR14_B1              = mr14[myps];
      mb_LPDDR4_1D[myps].MR16_B1              = mr16[myps];
      mb_LPDDR4_1D[myps].MR17_B1              = mr17[myps];
      mb_LPDDR4_1D[myps].MR22_B1              = mr22[myps];
      mb_LPDDR4_1D[myps].MR24_B1              = mr24[myps];
      
      mb_LPDDR4_1D[myps].Share2DVrefResult    = Share2DVrefResult;

#if 0
      memset((void *) &shdw_LPDDR4_1D[myps], 0, sizeof(PMU_SMB_LPDDR4_1D_t)); // Zero out struct contents 
#else
 memset((UINT8 *) &shdw_LPDDR4_1D[myps], 0, sizeof(PMU_SMB_LPDDR4_1D_t)); // Zero out struct contents 

#endif
      } // myps

    // 2D message block defaults
    if (Train2D) {
    for (myps=0; myps<1; myps++) {
        mb_LPDDR4_2D[myps].Pstate               = myps;
        mb_LPDDR4_2D[myps].SequenceCtrl         = SequenceCtrl2D[myps];
        mb_LPDDR4_2D[myps].PhyConfigOverride    = 0x0;
        mb_LPDDR4_2D[myps].HdtCtrl              = HdtCtrl;
        mb_LPDDR4_2D[myps].MsgMisc              = MsgMisc;
        mb_LPDDR4_2D[myps].Reserved00           = Reserved00;
        mb_LPDDR4_2D[myps].DFIMRLMargin         = DFIMRLMargin;
        mb_LPDDR4_2D[myps].PhyVref              = PhyVref;
  
        mb_LPDDR4_2D[myps].EnabledDQsChA        = userInputBasic.NumActiveDbyteDfi0 * 8;
        mb_LPDDR4_2D[myps].CsPresentChA         = (2==userInputBasic.NumRank_dfi0) ? 0x3 : userInputBasic.NumRank_dfi0;
        mb_LPDDR4_2D[myps].EnabledDQsChB        = userInputBasic.NumActiveDbyteDfi1 * 8;
        mb_LPDDR4_2D[myps].CsPresentChB         = (2==userInputBasic.NumRank_dfi1) ? 0x3 : userInputBasic.NumRank_dfi1;
        mb_LPDDR4_2D[myps].UseBroadcastMR       = UseBroadcastMR;

        mb_LPDDR4_2D[myps].Lp4Misc              = 0x0000;
        mb_LPDDR4_2D[myps].CATerminatingRankChA = CATerminatingRankChA;
        mb_LPDDR4_2D[myps].CATerminatingRankChB = CATerminatingRankChB;
        mb_LPDDR4_2D[myps].CATrainOpt           = 0x0000;
        mb_LPDDR4_2D[myps].X8Mode               = 0x0000;

        mb_LPDDR4_2D[myps].MR1_A0               = mr1[myps]; 
        mb_LPDDR4_2D[myps].MR2_A0               = mr2[myps]; 
        mb_LPDDR4_2D[myps].MR3_A0               = mr3[myps]; 
        mb_LPDDR4_2D[myps].MR4_A0               = mr4[myps]; 
        mb_LPDDR4_2D[myps].MR11_A0              = mr11[myps];
        mb_LPDDR4_2D[myps].MR12_A0              = mr12[myps];
        mb_LPDDR4_2D[myps].MR13_A0              = mr13[myps];
        mb_LPDDR4_2D[myps].MR14_A0              = mr14[myps];
        mb_LPDDR4_2D[myps].MR16_A0              = mr16[myps];
        mb_LPDDR4_2D[myps].MR17_A0              = mr17[myps];
        mb_LPDDR4_2D[myps].MR22_A0              = mr22[myps];
        mb_LPDDR4_2D[myps].MR24_A0              = mr24[myps];
        mb_LPDDR4_2D[myps].MR1_A1               = mr1[myps]; 
        mb_LPDDR4_2D[myps].MR2_A1               = mr2[myps]; 
        mb_LPDDR4_2D[myps].MR3_A1               = mr3[myps]; 
        mb_LPDDR4_2D[myps].MR4_A1               = mr4[myps]; 
        mb_LPDDR4_2D[myps].MR11_A1              = mr11[myps];
        mb_LPDDR4_2D[myps].MR12_A1              = mr12[myps];
        mb_LPDDR4_2D[myps].MR13_A1              = mr13[myps];
        mb_LPDDR4_2D[myps].MR14_A1              = mr14[myps];
        mb_LPDDR4_2D[myps].MR16_A1              = mr16[myps];
        mb_LPDDR4_2D[myps].MR17_A1              = mr17[myps];
        mb_LPDDR4_2D[myps].MR22_A1              = mr22[myps];
        mb_LPDDR4_2D[myps].MR24_A1              = mr24[myps];
      
        mb_LPDDR4_2D[myps].MR1_B0               = mr1[myps]; 
        mb_LPDDR4_2D[myps].MR2_B0               = mr2[myps]; 
        mb_LPDDR4_2D[myps].MR3_B0               = mr3[myps]; 
        mb_LPDDR4_2D[myps].MR4_B0               = mr4[myps]; 
        mb_LPDDR4_2D[myps].MR11_B0              = mr11[myps];
        mb_LPDDR4_2D[myps].MR12_B0              = mr12[myps];
        mb_LPDDR4_2D[myps].MR13_B0              = mr13[myps];
        mb_LPDDR4_2D[myps].MR14_B0              = mr14[myps];
        mb_LPDDR4_2D[myps].MR16_B0              = mr16[myps];
        mb_LPDDR4_2D[myps].MR17_B0              = mr17[myps];
        mb_LPDDR4_2D[myps].MR22_B0              = mr22[myps];
        mb_LPDDR4_2D[myps].MR24_B0              = mr24[myps];
        mb_LPDDR4_2D[myps].MR1_B1               = mr1[myps]; 
        mb_LPDDR4_2D[myps].MR2_B1               = mr2[myps]; 
        mb_LPDDR4_2D[myps].MR3_B1               = mr3[myps]; 
        mb_LPDDR4_2D[myps].MR4_B1               = mr4[myps]; 
        mb_LPDDR4_2D[myps].MR11_B1              = mr11[myps];
        mb_LPDDR4_2D[myps].MR12_B1              = mr12[myps];
        mb_LPDDR4_2D[myps].MR13_B1              = mr13[myps];
        mb_LPDDR4_2D[myps].MR14_B1              = mr14[myps];
        mb_LPDDR4_2D[myps].MR16_B1              = mr16[myps];
        mb_LPDDR4_2D[myps].MR17_B1              = mr17[myps];
        mb_LPDDR4_2D[myps].MR22_B1              = mr22[myps];
        mb_LPDDR4_2D[myps].MR24_B1              = mr24[myps];
      
        mb_LPDDR4_2D[myps].Share2DVrefResult    = Share2DVrefResult; 
        mb_LPDDR4_2D[myps].RX2D_TrainOpt        = 0x00; // DFE off, Voltage Step Size=1 DAC setting, LCDL Delay Step Size=1 LCDL delay between checked values.
        mb_LPDDR4_2D[myps].TX2D_TrainOpt        = 0x00; // FFE off, Voltage Step Size=1 DAC setting, LCDL Delay Step Size=1 LCDL delay between checked values.
        mb_LPDDR4_2D[myps].Delay_Weight2D       = 0x20; // Evenly weigh Delay vs Voltage
        mb_LPDDR4_2D[myps].Voltage_Weight2D     = 0x80; 

#if 0
        memset((void *) &shdw_LPDDR4_2D[myps], 0, sizeof(PMU_SMB_LPDDR4_2D_t)); // Zero out struct contents 
#else
memset((UINT8 *) &shdw_LPDDR4_2D[myps], 0, sizeof(PMU_SMB_LPDDR4_2D_t)); // Zero out struct contents 
#endif 
      } // myps
    } // Train2D

    // ##############################################################
    // userInputSim - Dram/Dimm Timing Parameters the user must p
    // provide value if applicable
    // ##############################################################
    userInputSim.tDQS2DQ                    = 0;  
    userInputSim.tDQSCK                     = 0;  
    userInputSim.tSTAOFF[0]                 = 0;
    userInputSim.tSTAOFF[1]                 = 0;
    userInputSim.tSTAOFF[2]                 = 0;
    userInputSim.tSTAOFF[3]                 = 0;
    userInputSim.tPDM[0]                    = 0;
    userInputSim.tPDM[1]                    = 0;
    userInputSim.tPDM[2]                    = 0;
    userInputSim.tPDM[3]                    = 0;

    
    //dwc_ddrphy_phyinit_print ("%s End of dwc_ddrphy_phyinit_initStruct()\n", printf_header);
}
/** @} */
