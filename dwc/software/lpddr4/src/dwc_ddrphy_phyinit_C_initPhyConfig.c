/** 
 * \file
 * \brief implements Step C of initialization sequence
 *
 * This file contains the implementation of dwc_ddrphy_phyinit_C_initPhyConfig
 * function.
 *
 * \addtogroup SrcFunc
 * @{
 */
#include <stdlib.h>
//#include <math.h>
#include "dwc_ddrphy_phyinit.h"


/** \brief implements Step C of initialization sequence
 *
 * This function programs majority of PHY configuration registers based
 * on data input into PhyInit data structures.
 * 
 * This function programs PHY configuration registers based on information
 * provided in the PhyInit data structures (userInputBasic, userInputAdvanced).
 * The user can overwrite the programming of this function by modifying
 * dwc_ddrphy_phyinit_userCustom_customPreTrain().  Please see
 * dwc_ddrphy_phyinit_struct.h for PhyInit data structure definition.
 *
 * \return void
 *
 * List of registers programmed by this function:
 */

//int ceil(float num)
//{
//   if(num < 0)
//        return (int)num;
//    else
//        return (int)num + 1;
//}


void dwc_ddrphy_phyinit_C_initPhyConfig () {

    //char *printf_header;
    //printf_header = "// [phyinit_C_initPhyConfig]";

    //dwc_ddrphy_phyinit_cmnt ("\n\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// Step (C) Initialize PHY Configuration \n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// Load the required PHY configuration registers for the appropriate mode and memory configuration\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("\n\n");

    int lane;
    int b_addr;
    int byte;
    int c_addr;    
    int anib;

    int pstate;
    int p_addr;
    
    //dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_C_initPhyConfig()\n", printf_header);
    
   
    /** 
     *  - Program TxSlewRate:
     *    - TxSlewRate::TxPreDrvMode is dependent on DramType.  
     *    - TxSlewRate::TxPreP and TxSlewRate::TxPreN are technology-specific
     *      User should consult the "Output Slew Rate" section of 
     *      HSpice Model App Note in specific technology for recommended settings
     */
    int TxSlewRate[4];
    int TxPreP[4];          // Default to 0xf (max). Optimal setting is technology specific.
    int TxPreN[4];          // Default to 0xf (max). Optimal setting is technology specific.
    int TxPreDrvMode; 
    
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("// TxPreDrvMode[2] = userInputBasic.Lp4xMode \n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");

    if (userInputBasic.Lp4xMode == 1)
        TxPreDrvMode = 0x5;
    else
        TxPreDrvMode = 0x1;

    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;
   
        TxPreP[pstate] = userInputAdvanced.TxSlewRiseDQ[pstate];
        TxPreN[pstate] = userInputAdvanced.TxSlewFallDQ[pstate];

        TxSlewRate[pstate] = (TxPreDrvMode << csr_TxPreDrvMode_LSB) | (TxPreN[pstate] << csr_TxPreN_LSB) | (TxPreP[pstate] << csr_TxPreP_LSB);

    
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxSlewRate::TxPreDrvMode to 0x%x\n", printf_header, pstate,  userInputBasic.Frequency[pstate], TxPreDrvMode);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxSlewRate::TxPreP to 0x%x\n", printf_header, pstate,  userInputBasic.Frequency[pstate], TxPreP[pstate]);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxSlewRate::TxPreN to 0x%x\n", printf_header, pstate,  userInputBasic.Frequency[pstate], TxPreN[pstate]);
        //dwc_ddrphy_phyinit_cmnt ("%s ### NOTE ### Optimal setting for TxSlewRate::TxPreP and TxSlewRate::TxPreP are technology specific.\n", printf_header);
        //dwc_ddrphy_phyinit_cmnt ("%s ### NOTE ### Please consult the \"Output Slew Rate\" section of HSpice Model App Note in specific technology for recommended settings\n\n", printf_header);

        for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
            c_addr = byte << 12;
            for (lane=0; lane <= b_max ; lane++) {
                b_addr = lane << 8;
                dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | b_addr | csr_TxSlewRate_ADDR), TxSlewRate[pstate]);
            }
        }
    }
   
    /** 
     * - Program ATxSlewRate:
     *  - ATxSlewRate::ATxPreDrvMode is dependent on DramType and whether
     *    the ACX4 instance is used for AC or CK
     *  - ATxSlewRate::ATxPreP and ATxSlewRate::TxPreN are technology-specific
     *    User should consult the "Output Slew Rate" section of 
     *    HSpice Model App Note in specific technology for recommended settings
     */

    int ATxSlewRate;
    int ATxPreP;          // Default to 0xf (max). Optimal setting is technology specific.
    int ATxPreN;          // Default to 0xf (max). Optimal setting is technology specific.
    int ATxPreDrvMode; 
    int CK_ANIB_inst[2];  

    ATxPreP = userInputAdvanced.TxSlewRiseAC;
    ATxPreN = userInputAdvanced.TxSlewFallAC;

    // # of ANIBs      CK ANIB Instance
    // ACX3            ANIB 1 (LP4 only)
    // ACX6            ANIB 1
    // ACX8            ANIB 1
    // ACX10           ANIB 1,6
    // ACX12           ANIB 4,5
    if (userInputBasic.NumAnib == 3 || userInputBasic.NumAnib == 6 || userInputBasic.NumAnib == 8) {
        CK_ANIB_inst[0] = 1;
        CK_ANIB_inst[1] = 1;
    } 
    else if (userInputBasic.NumAnib == 10) {
        CK_ANIB_inst[0] = 1;
        CK_ANIB_inst[1] = 6;
    } 
    else if (userInputBasic.NumAnib == 12) {
        CK_ANIB_inst[0] = 4;
        CK_ANIB_inst[1] = 5;
    } 
    else {
        //dwc_ddrphy_phyinit_assert (0, "Invalid number of ANIBs for protocol.  userInputBasic.NumAnib = %d.\n", userInputBasic.NumAnib);
    }
    
    for (anib=0; anib<userInputBasic.NumAnib; anib++) {
        c_addr = anib << 12;

        if (anib == CK_ANIB_inst[0] || anib == CK_ANIB_inst[1] )  {// CK ANIB instance
        
            ATxPreDrvMode = 0x1;
        
        }
        else {// non-CK ANIB instance
        
            ATxPreDrvMode = 0x1;
        
        }

        ATxSlewRate = (ATxPreDrvMode << csr_ATxPreDrvMode_LSB) | (ATxPreN << csr_ATxPreN_LSB) | (ATxPreP << csr_ATxPreP_LSB);
        
        //dwc_ddrphy_phyinit_cmnt ("%s Programming ATxSlewRate::ATxPreDrvMode to 0x%x, ANIB=%d\n", printf_header, ATxPreDrvMode, anib);
        //dwc_ddrphy_phyinit_cmnt ("%s Programming ATxSlewRate::ATxPreP to 0x%x, ANIB=%d\n", printf_header, ATxPreP, anib);
        //dwc_ddrphy_phyinit_cmnt ("%s Programming ATxSlewRate::ATxPreN to 0x%x, ANIB=%d\n", printf_header, ATxPreN, anib);
        //dwc_ddrphy_phyinit_cmnt ("%s ### NOTE ### Optimal setting for ATxSlewRate::ATxPreP and ATxSlewRate::ATxPreP are technology specific.\n", printf_header);
        //dwc_ddrphy_phyinit_cmnt ("%s ### NOTE ### Please consult the \"Output Slew Rate\" section of HSpice Model App Note in specific technology for recommended settings\n\n", printf_header);

        dwc_ddrphy_phyinit_userCustom_io_write16((tANIB | c_addr | csr_ATxSlewRate_ADDR), ATxSlewRate);

        //Program delay for Atx for LPDDR2 since there is no CA training
        
    }
   



    /** 
     * - Program PllCtrl2: 
     *   - Calculate PLL controls per p-state from Frequency
     */
    
    int PllCtrl2[4];    
    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
          p_addr = pstate << 20;
    
          if (userInputBasic.Frequency[pstate]/2 < 235) {  
            PllCtrl2[pstate] = 0x7;
          } 
          else if(userInputBasic.Frequency[pstate]/2 < 313) { 
            PllCtrl2[pstate] = 0x6;
          }        
          else if(userInputBasic.Frequency[pstate]/2 < 469) { 
            PllCtrl2[pstate] = 0xb; 
          }        
          else if(userInputBasic.Frequency[pstate]/2 < 625) { 
            PllCtrl2[pstate] = 0xa;
          }      
          else if(userInputBasic.Frequency[pstate]/2 < 938) { 
            PllCtrl2[pstate] = 0x19; 
          }      
          else if(userInputBasic.Frequency[pstate]/2 < 1067) { 
            PllCtrl2[pstate] = 0x18; 
          }      
          else { 
            PllCtrl2[pstate] = 0x19;
          }
          
          dwc_ddrphy_phyinit_userCustom_io_write16( (p_addr | tMASTER | csr_PllCtrl2_ADDR), PllCtrl2[pstate] );
          //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d,  Memclk=%dMHz, Programming PllCtrl2 to %x based on DfiClk frequency = %d.\n", printf_header, pstate,  userInputBasic.Frequency[pstate], PllCtrl2[pstate], userInputBasic.Frequency[pstate]/2);
    }
    
    /**
     * - Program ARdPtrInitVal:
     *   - The values programmed here assume ideal properties of DfiClk and Pclk including:
     *     - DfiClk skew
     *     - DfiClk jitter
     *     - DfiClk PVT variations
     *     - Pclk skew
     *     - Pclk jitter
     *
     *     ARdPtrInitVal Programmed differently based on PLL Bypass mode and Frequency:
     *   - PLL Bypassed mode:
     *     - For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 2-5
     *     - For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-5
     *   - PLL Enabled mode:
     *     - For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-5
     *     - For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 0-5
     */
    
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// Program ARdPtrInitVal based on Frequency and PLL Bypass inputs\n");
    //dwc_ddrphy_phyinit_cmnt ("// The values programmed here assume ideal properties of DfiClk\n");
    //dwc_ddrphy_phyinit_cmnt ("// and Pclk including:\n");
    //dwc_ddrphy_phyinit_cmnt ("// - DfiClk skew\n");
    //dwc_ddrphy_phyinit_cmnt ("// - DfiClk jitter\n");
    //dwc_ddrphy_phyinit_cmnt ("// - DfiClk PVT variations\n");
    //dwc_ddrphy_phyinit_cmnt ("// - Pclk skew\n");
    //dwc_ddrphy_phyinit_cmnt ("// - Pclk jitter\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// PLL Bypassed mode:\n");
    //dwc_ddrphy_phyinit_cmnt ("//     For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 2-5\n");
    //dwc_ddrphy_phyinit_cmnt ("//     For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-5\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// PLL Enabled mode:\n");
    //dwc_ddrphy_phyinit_cmnt ("//     For MemClk frequency > 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 1-5\n");
    //dwc_ddrphy_phyinit_cmnt ("//     For MemClk frequency < 933MHz, the valid range of ARdPtrInitVal_p0[3:0] is: 0-5\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n" );
    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        if (userInputBasic.Frequency[pstate] >= 933) {
            ARdPtrInitVal[pstate] = 0x2;
        }
        else {
            ARdPtrInitVal[pstate] = 0x1;
        }
        // Add one UI for synchronizer on SyncBus when PLL is bypassed
        if (userInputBasic.PllBypass[pstate] == 1) {
            ARdPtrInitVal[pstate] ++;
        }
    
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming ARdPtrInitVal to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], ARdPtrInitVal[pstate]);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_ARdPtrInitVal_ADDR), ARdPtrInitVal[pstate]);
    }
    
    
    /**
     * - Program ProcOdtCtl:
     *   - Sets ProcOdtAlwaysOn/ProcOdtAlwaysOff for LPDDR4 for 4 different Pstates using the PIE 
     *     register Seq0BGPR4.
     */
    
    int     Seq0BGPR4[4];    
    int     ProcOdtAlwaysOff[4];
    int     ProcOdtAlwaysOn[4];
    
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("// Seq0BGPR4       = 0: Make ProcOdtAlwaysOn = 0 and ProcOdtAlwaysOff = 0 \n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");

    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
      p_addr = pstate << 20;
      
      ProcOdtAlwaysOff[pstate] = 0;
      ProcOdtAlwaysOn[pstate]  = 0;

      Seq0BGPR4[pstate] = 0x00000000 | ( ProcOdtAlwaysOn[pstate] << csr_ProcOdtAlwaysOn_LSB ) | ( ProcOdtAlwaysOff[pstate] << csr_ProcOdtAlwaysOff_LSB );
      //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming ProcOdtCtl: Seq0BGPR4.ProcOdtAlwaysOff  to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], ProcOdtAlwaysOff[pstate]);
      //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming ProcOdtCtl: Seq0BGPR4.ProcOdtAlwaysOn   to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], ProcOdtAlwaysOn[pstate]);
      dwc_ddrphy_phyinit_userCustom_io_write16((p_addr  | tINITENG  | c0  | csr_Seq0BGPR4_ADDR), Seq0BGPR4[pstate] );      
    }    

    
    /**
     * - Program DbyteDllModeCntrl:
     *   - DllRxPreambleMode
     * - Program DqsPreambleControl:
     *   - Fields:
     *     - TwoTckRxDqsPre
     *     - TwoTckTxDqsPre
     *     - PositionDfeInit
     *     - LP4TglTwoTckTxDqsPre
     *     - LP4PostambleExt
     *     - LP4SttcPreBridgeRxEn
     *   - Dependencies:
     *      - user_input_basic.DramType
 
     *      - user_input_advanced.Lp4RxPreambleMode
     *      - user_input_advanced.WDQSExt
 
     */
    int DqsPreambleControl;
    int TwoTckRxDqsPre[4];
    int TwoTckTxDqsPre=0;
    int PositionDfeInit;
    int LP4TglTwoTckTxDqsPre=0;
    int LP4PostambleExt=0;
    int LP4SttcPreBridgeRxEn=0;
    int WDQSEXTENSION=0;
    int DllRxPreambleMode=0;
    int DbyteDllModeCntrl;
    int DllLockParam;
    int DisDllSeedSel=0;
    int DisDllGainIVSeed=1;
    int LcdlSeed0=0x21;
    int DllGainCtl;
    int DllGainIV=0x1;
    int DllGainTV=0x6;

    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        
            // LPDDR4
            
            TwoTckRxDqsPre[pstate]  = !userInputAdvanced.Lp4RxPreambleMode[pstate]; // set to 1 if static Rx preamble
            LP4SttcPreBridgeRxEn    = !userInputAdvanced.Lp4RxPreambleMode[pstate]; // set to 1 if static Rx preamble 
            DllRxPreambleMode       = 0x1;
            TwoTckTxDqsPre          = 0x1; // Must be 2*Tck Tx preamble according to JEDEC (MR1.OP[2] = 1)
            LP4TglTwoTckTxDqsPre    = 0x1; // Must be toggling Tx preamble
            PositionDfeInit         = 0x0;
            LP4PostambleExt         = userInputAdvanced.Lp4PostambleExt[pstate];
            WDQSEXTENSION           = userInputAdvanced.WDQSExt;
        
        
        DqsPreambleControl = (WDQSEXTENSION << csr_WDQSEXTENSION_LSB) | (LP4SttcPreBridgeRxEn << csr_LP4SttcPreBridgeRxEn_LSB) | (LP4PostambleExt << csr_LP4PostambleExt_LSB) | (LP4TglTwoTckTxDqsPre << csr_LP4TglTwoTckTxDqsPre_LSB) | (PositionDfeInit << csr_PositionDfeInit_LSB) | (TwoTckTxDqsPre << csr_TwoTckTxDqsPre_LSB) | (TwoTckRxDqsPre[pstate] << csr_TwoTckRxDqsPre_LSB) ; 
        
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DqsPreambleControl::TwoTckRxDqsPre to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TwoTckRxDqsPre[pstate]);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DqsPreambleControl::TwoTckTxDqsPre to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TwoTckTxDqsPre);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DqsPreambleControl::PositionDfeInit to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], PositionDfeInit);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DqsPreambleControl::LP4TglTwoTckTxDqsPre to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], LP4TglTwoTckTxDqsPre);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DqsPreambleControl::LP4PostambleExt to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], LP4PostambleExt);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DqsPreambleControl::LP4SttcPreBridgeRxEn to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], LP4SttcPreBridgeRxEn);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DqsPreambleControl to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], DqsPreambleControl);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_DqsPreambleControl_ADDR), DqsPreambleControl);
    
        DbyteDllModeCntrl = DllRxPreambleMode << csr_DllRxPreambleMode_LSB;
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DbyteDllModeCntrl to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate],DbyteDllModeCntrl);
        dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_DbyteDllModeCntrl_ADDR), DbyteDllModeCntrl);

        DllGainCtl = DllGainIV | (DllGainTV<<csr_DllGainTV_LSB);
        DllLockParam =  DisDllSeedSel |  (DisDllGainIVSeed<<csr_DisDllGainIVSeed_LSB)|(LcdlSeed0<<csr_LcdlSeed0_LSB) ;
        
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DllLockParam to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate],DllLockParam);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_DllLockParam_ADDR), DllLockParam);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DllGainCtl to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate],DllGainCtl);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_DllGainCtl_ADDR), DllGainCtl);
        

    }

 
    /**
     * - Program ProcOdtTimeCtl:
     *   - Fields: 
     *     - POdtStartDelay[3:2]
     *     - POdtTailWidth[1:0]
     *   - Dependencies:
     *     - user_input_basic.Frequency
     *     - user_input_basic.DramType
     */
    int ProcOdtTimeCtl[4];
    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        if (userInputAdvanced.WDQSExt) {
            // POdtStartDelay = 0x0
            // POdtTailWidth  = 0x3
            ProcOdtTimeCtl[pstate] = 0x3;
        }
        // Memclk Freq <= 933MHz
        else if (userInputBasic.Frequency[pstate] <= 933) {
            // POdtStartDelay = 0x2
            // POdtTailWidth  = 0x2
            ProcOdtTimeCtl[pstate] = 0xa;
        }
        // 933MHz < Memclk Freq <= 1200MHz
        else if (userInputBasic.Frequency[pstate] <= 1200) {
            if (TwoTckRxDqsPre[pstate] == 1) {
                // POdtStartDelay = 0x0
                // POdtTailWidth  = 0x2
                ProcOdtTimeCtl[pstate] = 0x2;
            }
            else {
                // POdtStartDelay = 0x1
                // POdtTailWidth  = 0x2
                ProcOdtTimeCtl[pstate] = 0x6;
            }
        }
        // Memclk Freq > 1200MHz
        else {
            if (TwoTckRxDqsPre[pstate] == 1) {
                // POdtStartDelay = 0x0
                // POdtTailWidth  = 0x3
                ProcOdtTimeCtl[pstate] = 0x3;
            }
            else {
                // POdtStartDelay = 0x1
                // POdtTailWidth  = 0x3
                ProcOdtTimeCtl[pstate] = 0x7;
            }
        }
    
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming ProcOdtTimeCtl to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], ProcOdtTimeCtl[pstate]);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_ProcOdtTimeCtl_ADDR), ProcOdtTimeCtl[pstate]);
    }
    
    /**
     * - Program TxOdtDrvStren:
     *   - Fields: 
     *     - ODTStrenP_px[5:0]
     *     - ODTStrenN_px[11:6]
     *   - Dependencies:
     *     - user_input_basic.DramType
     *     - user_input_basic.HardMacroVer
     *     - user_input_advanced.ODTImpedance
     */
    int   TxOdtDrvStren[4];
    int   ODTStrenP_state[4];
    int   ODTStrenN_state[4];
    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
      p_addr = pstate << 20;
      
      ODTStrenP_state[pstate] = dwc_ddrphy_phyinit_mapDrvStren ((int) userInputAdvanced.ODTImpedance[pstate], ODTStrenP, userInputBasic.HardMacroVer ); 
      ODTStrenN_state[pstate] = dwc_ddrphy_phyinit_mapDrvStren ((int) userInputAdvanced.ODTImpedance[pstate], ODTStrenN, userInputBasic.HardMacroVer ); 
    
      TxOdtDrvStren[pstate] = (ODTStrenN_state[pstate] << csr_ODTStrenN_LSB) | ODTStrenP_state[pstate];

      //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxOdtDrvStren::ODTStrenP to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], ODTStrenP_state[pstate]);
      //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxOdtDrvStren::ODTStrenN to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], ODTStrenN_state[pstate]);
      for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
          c_addr = byte << 12;
          for (lane=0; lane <= b_max ; lane++) {
              b_addr = lane << 8;
              dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | b_addr | csr_TxOdtDrvStren_ADDR), TxOdtDrvStren[pstate]);
          }
      }
    }

    /**
     * - Program TxImpedanceCtrl1:
     *   - Fields: 
     *     - DrvStrenFSDqP[5:0]
     *     - DrvStrenFSDqN[11:6]
     *   - Dependencies:
     *     - user_input_basic.DramType
     *     - user_input_basic.HardMacroVer
     *     - user_input_advanced.TxImpedance
     */
    int TxImpedanceCtrl1[4];
    int DrvStrenFSDqP_state[4];
    int DrvStrenFSDqN_state[4];
           
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;        
   
        DrvStrenFSDqP_state[pstate] = dwc_ddrphy_phyinit_mapDrvStren ((int) userInputAdvanced.TxImpedance[pstate], DrvStrenFSDqP, userInputBasic.HardMacroVer); 
        DrvStrenFSDqN_state[pstate] = dwc_ddrphy_phyinit_mapDrvStren ((int) userInputAdvanced.TxImpedance[pstate], DrvStrenFSDqN, userInputBasic.HardMacroVer);
   
        TxImpedanceCtrl1[pstate] = (DrvStrenFSDqN_state[pstate] << csr_DrvStrenFSDqN_LSB) | (DrvStrenFSDqP_state[pstate] << csr_DrvStrenFSDqP_LSB);

        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxImpedanceCtrl1::DrvStrenFSDqP to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], DrvStrenFSDqP_state[pstate]);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxImpedanceCtrl1::DrvStrenFSDqN to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], DrvStrenFSDqN_state[pstate]);
        
        for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
            c_addr = byte << 12;
            for (lane=0; lane <= b_max ; lane++) {
                b_addr = lane << 8;
                dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | b_addr | csr_TxImpedanceCtrl1_ADDR), TxImpedanceCtrl1[pstate]);
            }
        }
    }
    
    
    /**
     * - Program ATxImpedance:
     *   - Fields: 
     *     - ADrvStrenP[4:0]
     *     - ADrvStrenN[9:5]
     *   - Dependencies:
     *     - user_input_basic.DramType
     *     - user_input_basic.HardMacroVer
     *     - user_input_advanced.ATxImpedance
     */
    int ATxImpedance;
    int ADrvStrenP_state;
    int ADrvStrenN_state;

    // In hardmacro family E, the default 20 Ohm value is invalid
    // override de default value to be 40ohm
    if ( userInputBasic.HardMacroVer == 4 )  {          
      if( userInputAdvanced.ATxImpedance == 20 ) {
        //dwc_ddrphy_phyinit_assert (1,"%s userInputAdvanced.ATxImpedance == 20 ).\n", printf_header);
        //dwc_ddrphy_phyinit_assert (1,"%s In Hardmacro family E, maximum drive strenght is 40 Ohms \n", printf_header);
        //dwc_ddrphy_phyinit_assert (1,"%s Overriding userInputAdvanced.ATxImpedance to 40.\n", printf_header);
        userInputAdvanced.ATxImpedance = 40;
      }     
    }       
    
    ADrvStrenP_state    = dwc_ddrphy_phyinit_mapDrvStren ((int) userInputAdvanced.ATxImpedance, ADrvStrenP, userInputBasic.HardMacroVer); 
    ADrvStrenN_state    = dwc_ddrphy_phyinit_mapDrvStren ((int) userInputAdvanced.ATxImpedance, ADrvStrenN, userInputBasic.HardMacroVer);

    ATxImpedance  = (ADrvStrenN_state << csr_ADrvStrenN_LSB) | (ADrvStrenP_state << csr_ADrvStrenP_LSB);

    //dwc_ddrphy_phyinit_cmnt ("%s Programming ATxImpedance::ADrvStrenP to 0x%x\n", printf_header, ADrvStrenP_state);
    //dwc_ddrphy_phyinit_cmnt ("%s Programming ATxImpedance::ADrvStrenN to 0x%x\n", printf_header, ADrvStrenN_state);
    
    for (anib=0; anib<userInputBasic.NumAnib; anib++) {
        c_addr = anib << 12;
        dwc_ddrphy_phyinit_userCustom_io_write16(( tANIB | c_addr | csr_ATxImpedance_ADDR), ATxImpedance);
    }

    /**
     * - Program DfiMode:
     *   - Dependencies:
     *     - user_input_basic.DramType
     *     - user_input_basic.Dfi1Exists
     *     - user_input_basic.NumActiveDbyteDfi0 
     *     - user_input_basic.NumActiveDbyteDfi1 
     *     - user_input_basic.NumAnib 
     */
    int DfiMode;
    
    if(userInputBasic.Dfi1Exists == 1) {
        if (userInputBasic.NumActiveDbyteDfi1 > 0)  
            DfiMode = 0x3;  // Both DFI0 and DFI1 active
        else if (userInputBasic.NumActiveDbyteDfi0 > userInputBasic.NumDbyte/2 &&
                 userInputBasic.NumDbyte==4)
            DfiMode = 0x5;  // DFI1 exists but disabled; DFI0 controls entire PHY
        else
            DfiMode = 0x1;  // DFI1 exists but disabled; DFI0 controls 1/2 PHY
    }
    else {
        DfiMode = 0x1;      // DFI1 does not physically exists
    }
    
    //dwc_ddrphy_phyinit_cmnt ("%s Programming DfiMode to 0x%x\n", printf_header, DfiMode);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_DfiMode_ADDR), DfiMode);
    
    /**
     * - Program DfiCAMode:
     *   - Fields:
     *     - DfiLp3CAMode
     *     - DfiD4CAMode
     *     - DfiLp4CAMode
     *     - DfiD4AltCAMode
     *   - Dependencies:
     *     - user_input_basic.DramType
     */
    int DfiCAMode=0;
    
    // LPDDR4
    DfiCAMode = 4;
    
    //dwc_ddrphy_phyinit_cmnt ("%s Programming DfiCAMode to 0x%x\n", printf_header, DfiCAMode);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_DfiCAMode_ADDR), DfiCAMode);
    
    /**
     * - Program CalDrvStr0:
     *   - Fields:
     *     - CalDrvStrPd50[3:0]
     *     - CalDrvStrPu50[7:4]
     *   - Dependencies:
     *     - user_input_advanced.ExtCalResVal
     *     - userInputBasic.HardMacroVer 
     *     - userInputBasic.Lp4xMode
     *     - userInputAdvanced.Lp4LowPowerDrv
     *     - mb_LPDDR4_1D[0].MR3_A0
     */
    uint16_t CalDrvStr0;
    uint16_t CalDrvStrPd50;
    uint16_t CalDrvStrPu50;
   
    if (userInputBasic.HardMacroVer ==  4 )
      {
        switch (userInputAdvanced.ExtCalResVal)
          {
            case 0x1: // 120
              if ( userInputBasic.Lp4xMode) CalDrvStrPu50 = 0x0 ;
              else if ( userInputAdvanced.Lp4LowPowerDrv) 
                {
                  // Program based on MR3.PU_Cal
                  CalDrvStrPu50 = (0x1 & mb_LPDDR4_1D[0].MR3_A0) ? 0x4 /* VDDQ/3 */ : 0x3 /* VDDQ*2.5 */ ;
                }
              else CalDrvStrPu50 = 0x1;
              break;
            case 0x2: // 40
              CalDrvStrPu50 = 0x2;
              break;
            default:
              CalDrvStrPu50 = 0x0;
              //dwc_ddrphy_phyinit_assert (1, "%s userInputAdvanced.ExtCalResVal == %d is not supported in HME.\n", printf_header, userInputAdvanced.ExtCalResVal);
          }
      }
    else
      {
        CalDrvStrPu50 = userInputAdvanced.ExtCalResVal;
      }
        
    CalDrvStrPd50 = CalDrvStrPu50; 
    CalDrvStr0 = (CalDrvStrPu50 << csr_CalDrvStrPu50_LSB) | CalDrvStrPd50;
    
    //dwc_ddrphy_phyinit_cmnt ("%s Programming CalDrvStr0::CalDrvStrPd50 to 0x%x\n", printf_header, CalDrvStrPd50);
    //dwc_ddrphy_phyinit_cmnt ("%s Programming CalDrvStr0::CalDrvStrPu50 to 0x%x\n", printf_header, CalDrvStrPu50);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_CalDrvStr0_ADDR), CalDrvStr0);
    
    /**
     * - Program CalVRefs:
     *   - Impedance calibration VRef for Hard Macro E.
     *   - Dependencies:
     *     - userInputBasic.HardMacroVer 
     *     - userInputBasic.Lp4xMode
     *     - userInputAdvanced.Lp4LowPowerDrv
     *     - mb_LPDDR4_1D[0].MR3_A0
     *     - user_input_basic.HardMacro
     */
    uint16_t CalVRefs;
    if (userInputBasic.HardMacroVer ==  4)
      {
        if (userInputAdvanced.Lp4LowPowerDrv) 
          CalVRefs = (0x1 & mb_LPDDR4_1D[0].MR3_A0) ? 0x0 /* VDDQ/3 */ : 0x1 /* VDDQ*2.5 */ ;
        else 
          CalVRefs = 0x2;
        //dwc_ddrphy_phyinit_cmnt ("%s Programming CalVRefs to 0x%x\n", printf_header, CalVRefs);
        dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_CalVRefs_ADDR), CalVRefs);
      }

    /**
     * - Program CalUclkInfo:
     *   - Impedance calibration CLK Counter.
     *   - Fields:
     *     - CalUClkTicksPer1uS
              
     *   - Dependencies:
     *     - user_input_basic.Frequency
     */
    double  CalUClkTicksPer1uS_double[4];
    uint16_t     CalUClkTicksPer1uS[4];
    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;
    
        // Number of DfiClk cycles per 1us
        CalUClkTicksPer1uS_double[pstate] = (double)  (1.0*userInputBasic.Frequency[pstate])/2.0;
        CalUClkTicksPer1uS[pstate]        = (int) ceil ( CalUClkTicksPer1uS_double[pstate] );
        
        if (CalUClkTicksPer1uS[pstate] < 24)
            CalUClkTicksPer1uS[pstate] = 24;  // Minimum value of CalUClkTicksPer1uS = 24
    
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming CalUclkInfo::CalUClkTicksPer1uS to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], CalUClkTicksPer1uS[pstate]);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_CalUclkInfo_ADDR), CalUClkTicksPer1uS[pstate]);
    }
    
    
    //##############################################################
    //
    // Program Calibration CSRs based on user input 
    //
    // CSRs to program:
    //      CalRate:: CalInterval
    //             :: CalOnce
    //
    // User input dependencies::
    //      CalInterval
    //      CalOnce
    //
    //##############################################################
    int CalRate;
    int CalInterval;
    int CalOnce;

    CalInterval = userInputAdvanced.CalInterval;
    CalOnce = userInputAdvanced.CalOnce;

    CalRate = (CalOnce << csr_CalOnce_LSB) | (CalInterval << csr_CalInterval_LSB);

    //dwc_ddrphy_phyinit_cmnt ("%s Programming CalRate::CalInterval to 0x%x\n", printf_header, CalInterval);
    //dwc_ddrphy_phyinit_cmnt ("%s Programming CalRate::CalOnce to 0x%x\n", printf_header, CalOnce);

    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_CalRate_ADDR), CalRate);

    /**
     * - Program VrefInGlobal:
     *   - DqDqsRcvCntrl and csrVrefInGlobal to select Global VREF
     *     from Master to be used in each DQ
     *   - Fields:
     *     - GlobalVrefInSel: Select Range of GlobalVref DAC. Default: set to 1.
     *     - GlobalVrefInDAC: Vref level is set based on mb_LPDDR4_1D[pstate].PhyVref value.
     *       The following formula is used to convert the PhyVref into the register setting. 
     *       \f{eqnarray*}{
     *           PhyVrefPrcnt &=& \frac{mb LPDDR4 1D[pstate].PhyVref}{128} \\
     *        if GlobalVrefInSel = 1 :
     *           GlobalVrefInDAC &=& 1+\frac{PhyVrefPrcnt}{0.005} \\
     *        if GlobalVrefInSel = 0 :
     *           GlobalVrefInDAC &=& \frac{(PhyVrefPrcnt-0.345)}{0.005} \\
     *           RxVref &=& (GlobalVrefInDAC == 0) ? Hi-Z : (PhyVrefPrcnt \times VDDQ)
     *        \f}
     *   - Dependencies:
     *     - user_input_basic.DramType
     *
     * - Program DqDqsRcvCntrl:
     *   - DqDqsRcvCntrl and csrVrefInGlobal to select Global VREF
     *     from Master to be used in each DQ
     *   - Fields:
     *    - SelAnalogVref
     *    - MajorModeDbyte
     *    - ExtVrefRange
     *    - DfeCtrl
     *    - GainCurrAdj
     *   - Dependencies:
     *     - user_input_basic.DramType
     */
    int VrefInGlobal;
    int DqDqsRcvCntrl;
    int SelAnalogVref = 1; // Use Global VREF from Master
    int MajorModeDbyte = 0;
    int ExtVrefRange_defval = 0;
    int DfeCtrl_defval = 0;
    int GainCurrAdj_defval = 0xb;

    // LPDDR4
    MajorModeDbyte = 2;

    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        int32_t vref_percentVddq = (mb_LPDDR4_1D[pstate].PhyVref)*1000*100/128;
        uint8_t GlobalVrefInDAC = 0x0; 
        uint8_t GlobalVrefInSel = 0x4;
        // check range1 first. Only use range0 if customer input maxes out range1
        GlobalVrefInDAC = (uint8_t)((vref_percentVddq / 500)+1);  // Min value is 1
        if (GlobalVrefInDAC > 127){
           GlobalVrefInDAC = (uint8_t)(max((vref_percentVddq-34500),500) / 500); // Min value is 1
           GlobalVrefInSel = 0x0;
        }
        GlobalVrefInDAC = min(GlobalVrefInDAC,127);

        VrefInGlobal = (GlobalVrefInDAC << csr_GlobalVrefInDAC_LSB) | GlobalVrefInSel;
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming VrefInGlobal::GlobalVrefInSel to 0x%x\n", printf_header, pstate, GlobalVrefInSel);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming VrefInGlobal::GlobalVrefInDAC to 0x%x\n", printf_header, pstate, GlobalVrefInDAC);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming VrefInGlobal to 0x%x\n", printf_header, pstate, VrefInGlobal);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_VrefInGlobal_ADDR), VrefInGlobal);

        DqDqsRcvCntrl = (GainCurrAdj_defval << csr_GainCurrAdj_LSB) | (MajorModeDbyte << csr_MajorModeDbyte_LSB) | (DfeCtrl_defval << csr_DfeCtrl_LSB) | (ExtVrefRange_defval << csr_ExtVrefRange_LSB) | (SelAnalogVref << csr_SelAnalogVref_LSB);

        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming DqDqsRcvCntrl::MajorModeDbyte to 0x%x\n", printf_header, pstate, MajorModeDbyte);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming DqDqsRcvCntrl to 0x%x\n", printf_header, pstate, DqDqsRcvCntrl);
        for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
            c_addr = byte << 12;
            for (lane=0; lane <= b_max ; lane++) {
                b_addr = lane << 8;
                dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | b_addr | csr_DqDqsRcvCntrl_ADDR), DqDqsRcvCntrl);
            }
        }
    }

    
   
    /**
     * - Program DfiFreqRatio :
     *   - Dependencies:
     *     - user_input_basic.DfiFreqRatio
     */
    int DfiFreqRatio[4];
    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        DfiFreqRatio[pstate] = userInputBasic.DfiFreqRatio[pstate];
       
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DfiFreqRatio_p%d to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], pstate, DfiFreqRatio[pstate]);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_DfiFreqRatio_ADDR), DfiFreqRatio[pstate]);
    }
    
    /**
     * - Program TristateModeCA based on DramType and 2T Timing
     *   - Fields:
     *     - CkDisVal
     *     - DisDynAdrTri
     *     - DDR2TMode
     *   - Dependencies:
     *     - user_input_basic.DramType
     *     - user_input_advanced.Is2Ttiming
     *     - user_input_advanced.DisDynAdrTri
     */
    int TristateModeCA[4];
    int DisDynAdrTri[4];
    int DDR2TMode[4];
    int CkDisVal_def; 
                     
    // CkDisVal depends on DramType
    // LPDDR4 or LPDDR3
    CkDisVal_def = 0; // {CLK_t,CLK_c} = 2'b01; 
    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
      p_addr = pstate << 20;
      
  
    // LPDDR4
    DisDynAdrTri[pstate] = 1;
  
      
      DDR2TMode[pstate] = userInputAdvanced.Is2Ttiming[pstate];
      
        TristateModeCA[pstate] = (CkDisVal_def << csr_CkDisVal_LSB) | (DDR2TMode[pstate] << csr_DDR2TMode_LSB) | (DisDynAdrTri[pstate] << csr_DisDynAdrTri_LSB) ;
        
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TristateModeCA::DisDynAdrTri_p%d to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], pstate, DisDynAdrTri[pstate]);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TristateModeCA::DDR2TMode_p%d to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], pstate, DDR2TMode[pstate]);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_TristateModeCA_ADDR), TristateModeCA[pstate]);
        
    }

  
    /**
     * - Program DfiXlat based on Pll Bypass Input
     *   - Dependencies:
     *     - user_input_basic.DramType
     *     - user_input_basic.PllBypass
     */
    //dwc_ddrphy_phyinit_cmnt ("%s Programming DfiFreqXlat*\n", printf_header);
    
    uint16_t loopVector;
    uint16_t dfifreqxlat_dat;
    uint16_t pllbypass_dat;
    uint16_t skipddc_dat=0; // set to vector offset based on Pstate frequency to disable dram drift compensation.         
    pllbypass_dat = (userInputBasic.PllBypass[3] << 12) | (userInputBasic.PllBypass[2]<< 8) | (userInputBasic.PllBypass[1]<< 4) | (userInputBasic.PllBypass[0]);
    
    for (pstate=0; pstate<=3; pstate++) {
        if (userInputBasic.Frequency[pstate] < 333) skipddc_dat = skipddc_dat | (0x5 << 4*pstate);
    }
    

    for(loopVector=0; loopVector<8; loopVector++) {
    
        if (loopVector == 0) {          // Retrain & Relock DfiFreq = 00,01,02,03)  Use StartVec 0 (pll_enabled) or StartVec 1 (pll_bypassed)
            dfifreqxlat_dat = pllbypass_dat + skipddc_dat + 0x0000;
            //dwc_ddrphy_phyinit_cmnt("%s:DEBUG: LP4 : loopVector = %d : dfifreqxlat_dat = %x.\n", printf_header, loopVector, dfifreqxlat_dat);
            dwc_ddrphy_phyinit_userCustom_io_write16((c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), dfifreqxlat_dat); 
        } else if (loopVector == 2) {   // Retrain only DfiFreq = 08,09,0A,0B)  Use StartVec 4 (1, and maybe 2,3, used by verif)
            dwc_ddrphy_phyinit_userCustom_io_write16((c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), 0x4444);
        } else if (loopVector == 3) {   // phymstr type state change, StartVec 8
            dwc_ddrphy_phyinit_userCustom_io_write16((c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), 0x8888);
        } else if (loopVector == 4) {   // Relock only DfiFreq = 10,11,12,13   Use StartVec 5 (pll_enabled) or StartVec 6 (pll_bypassed)
            dfifreqxlat_dat = pllbypass_dat + 0x5555;
            //dwc_ddrphy_phyinit_cmnt("%s:DEBUG: LP4 : loopVector = %d : dfifreqxlat_dat = %x.\n", printf_header, loopVector, dfifreqxlat_dat);
            dwc_ddrphy_phyinit_userCustom_io_write16((c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), dfifreqxlat_dat); 
        } else if (loopVector == 7) {   // LP3-entry DfiFreq = 1F
            dwc_ddrphy_phyinit_userCustom_io_write16((c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), 0xF000);
        } else {                        // everything else
            dwc_ddrphy_phyinit_userCustom_io_write16((c0 | tMASTER | (csr_DfiFreqXlat0_ADDR+loopVector)), 0x0000);
        }

    
    }
 
    /**
     * - Program DqDqsRcvCntrl1 (Receiver Powerdown) and DbyteMiscMode
     *   - see function dwc_ddrphy_phyinit_IsDbyteDisabled() to determine
     *     which DBytes are turned off completely based on PHY configuration.
     *   - Fields:
     *     - DByteDisable
     *     - PowerDownRcvr 
     *     - PowerDownRcvrDqs
     *     - RxPadStandbyEn
     *   - Dependencies:
     *     - user_input_basic.DramType
     *     - user_input_basic.Dfi1Exists
     *     - user_input_basic.NumActiveDbyteDfi0 
     *     - user_input_basic.NumActiveDbyteDfi1 
     *     - user_input_basic.NumDbyte 
        
     *     - user_input_advanced.Lp4DbiRd
        
     */
    uint16_t regData;
    regData = 0x1 << csr_DByteDisable_LSB;
    unsigned int regData1; 
    regData1 = (0x1ff << csr_PowerDownRcvr_LSB | 0x1 << csr_PowerDownRcvrDqs_LSB | 0x1 << csr_RxPadStandbyEn_LSB) ;
    unsigned int regData2; // turn off Rx of DBI lane
    regData2 = (0x100 << csr_PowerDownRcvr_LSB | csr_RxPadStandbyEn_MASK) ;

    int d;
    // Implements Section 1.3 of Pub Databook
    for (d=0;d<userInputBasic.NumDbyte;d++)  // for each dbyte
      {
        c_addr = d*c1;
        if (dwc_ddrphy_phyinit_IsDbyteDisabled(d)) 
          {
            //dwc_ddrphy_phyinit_cmnt ("%s disable Dbyte%d\n",printf_header, d);
            dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_DbyteMiscMode_ADDR), regData);
            dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_DqDqsRcvCntrl1_ADDR), regData1);
          }
        else
          {
           // disable RDBI lane if not used.
        
            if ( userInputAdvanced.Lp4DbiRd[0] == 0 && 
                 userInputAdvanced.Lp4DbiRd[1] == 0 &&  
                 userInputAdvanced.Lp4DbiRd[2] == 0 &&  
                 userInputAdvanced.Lp4DbiRd[3] == 0 )
              {
        
                //dwc_ddrphy_phyinit_cmnt ("%s Disabling Lane 8 Receiver to save power.%d\n",printf_header, d);
                dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_DqDqsRcvCntrl1_ADDR), regData2);
        
              } // DBI
        
          } // DbyteDisable.
      } // for each dbyte

    /**
     * - Program DqDqsRcvCntrl1 (Receiver Powerdown) and DbyteMiscMode
     *   - Fields:
     *     - X4TG
     *     - MasterX4Config
     *   - Dependencies:
     *     - user_input_basic.DramDataWidth
     *
     * \note PHY does not support mixed dram device data width
     */
    int X4TG;
    int MasterX4Config;

    if (userInputBasic.DramDataWidth == 4)
        X4TG = 0xf;
    else
        X4TG = 0x0;

    MasterX4Config = X4TG << csr_X4TG_LSB;

    //dwc_ddrphy_phyinit_cmnt ("%s Programming MasterX4Config::X4TG to 0x%x\n", printf_header, MasterX4Config);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_MasterX4Config_ADDR), MasterX4Config);

    /**
     * - Program DMIPinPresent based on DramType and Read-DBI enable
     *   - Fields:
     *     - RdDbiEnabled
     *   - Dependencies:
     *     - user_input_basic.DramDataWidth
        
     *     - user_input_advanced.Lp4DbiRd
        
     */
    int DMIPinPresent[4];

    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;
        
    

    
    
        DMIPinPresent[pstate] = userInputAdvanced.Lp4DbiRd[pstate];
        
    
    
    
    
    
    
    
    
    

        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DMIPinPresent::RdDbiEnabled to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], DMIPinPresent[pstate]);

        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_DMIPinPresent_ADDR), DMIPinPresent[pstate]);
    }

        

    /**
     * - Program DFIPHYUPD
     *   - Fields:
     *     - DFIPHYUPDMODE 
     *     - DFIPHYUPDCNT 
     *   - Dependencies:
     *     - user_input_advanced.DisablePhyUpdate
     */
    if (userInputAdvanced.DisablePhyUpdate) {
        dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_DFIPHYUPD_ADDR), 0x0);
    }

    uint16_t AForceTriCont=0x0;
    uint16_t Acx4AnibDis= 0x0;
    int NumRank0 = userInputBasic.NumRank_dfi0;
    int NumRank1 = userInputBasic.NumRank_dfi1;
    for (anib=0; (anib<userInputBasic.NumAnib) && userInputAdvanced.DisableUnusedAddrLns; anib++)
      {
        c_addr = anib << 12;
        AForceTriCont=0x0;
        if (userInputBasic.NumAnib == 12) 
          {
            if      (anib == 0  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 0  && NumRank0 == 1) AForceTriCont = 0xe;
            else if (anib == 0  && NumRank0 == 2) AForceTriCont = 0xc;
            else if (anib == 1  && NumRank0 != 0) AForceTriCont = 0x6;
            else if (anib == 1  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 2  && NumRank0 != 0) AForceTriCont = 0x3;
            else if (anib == 2  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 3  && NumRank0 != 0) AForceTriCont = 0xc;
            else if (anib == 3  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 4  && NumRank0 != 0) AForceTriCont = 0xc;
            else if (anib == 4  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 5  && NumRank1 != 0) AForceTriCont = 0xc;
            else if (anib == 5  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 6  && NumRank1 != 0) AForceTriCont = 0xe;
            else if (anib == 6  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 7  && NumRank1 != 0) AForceTriCont = 0xc;
            else if (anib == 7  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 8  && NumRank1 != 0) AForceTriCont = 0x1;
            else if (anib == 8  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 9  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 9  && NumRank0 == 1) AForceTriCont = 0xe;
            else if (anib == 9  && NumRank0 == 2) AForceTriCont = 0xc;
            else if (anib == 10 && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 10 && NumRank1 == 1) AForceTriCont = 0xd;
            else if (anib == 10 && NumRank1 == 2) AForceTriCont = 0xc;
            else if (anib == 11 && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 11 && NumRank1 == 1) AForceTriCont = 0xb;
            else if (anib == 11 && NumRank1 == 2) AForceTriCont = 0x3;
          }
        else if (userInputBasic.NumAnib == 10) 
          {
            if      (anib == 0  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 0  && NumRank0 == 1) AForceTriCont = 0xa;
            else if (anib == 1  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 1  && NumRank0 != 0) AForceTriCont = 0xc;
            else if (anib == 2  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 3  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 3  && NumRank0 != 0) AForceTriCont = 0xc;
            else if (anib == 4                  ) AForceTriCont = 0xf;
            else if (anib == 5  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 5  && NumRank1 == 1) AForceTriCont = 0x6;
            else if (anib == 6  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 6  && NumRank1 != 0) AForceTriCont = 0xc;
            else if (anib == 7  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 8  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 8  && NumRank1 != 0) AForceTriCont = 0xc;
            else if (anib == 9                  ) AForceTriCont = 0xf;
          }
       else if (userInputBasic.NumAnib == 8) 
          {
            if (anib == 0 && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 0 && NumRank0 == 1) AForceTriCont = 0x2;
            else if (anib == 1 && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 1 && NumRank0 == 1) AForceTriCont = 0x8;
            else if (anib == 2 && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 3 && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 3 && NumRank1 == 1) AForceTriCont = 0x2;
            else if (anib == 4 && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 4 && NumRank1 == 1) AForceTriCont = 0x8;
            else if (anib == 5 && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 6 ) AForceTriCont = 0xf;
            else if (anib == 7 ) AForceTriCont = 0xf;
          }
        else if (userInputBasic.NumAnib == 6) 
          {
            if      (anib == 0  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 0  && NumRank0 == 1) AForceTriCont = 0x2;
            else if (anib == 1  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 1  && NumRank0 == 1) AForceTriCont = 0x8;
            else if (anib == 2  && NumRank0 == 0) AForceTriCont = 0xf;
            else if (anib == 3  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 3  && NumRank1 == 1) AForceTriCont = 0x2;
            else if (anib == 4  && NumRank1 == 0) AForceTriCont = 0xf;
            else if (anib == 4  && NumRank1 == 1) AForceTriCont = 0x8;
            else if (anib == 5  && NumRank1 == 0) AForceTriCont = 0xf;
          }
        
        // if all the lanes can be disabled, and Anib is not the first or last disable entire chiplet
        if ( AForceTriCont == 0xf && anib !=0 && anib !=userInputBasic.NumAnib-1) Acx4AnibDis = Acx4AnibDis | (0x1 << anib);
        dwc_ddrphy_phyinit_userCustom_io_write16(( tANIB | c_addr | csr_AForceTriCont_ADDR), AForceTriCont);
      }
    dwc_ddrphy_phyinit_userCustom_io_write16(( tMASTER |  csr_Acx4AnibDis_ADDR), Acx4AnibDis);

    //dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_C_initPhyConfig()\n", printf_header);
}
// End of dwc_ddrphy_phyinit_C_initPhyConfig()

/** @} */
