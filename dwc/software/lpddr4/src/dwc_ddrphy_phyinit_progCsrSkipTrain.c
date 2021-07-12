/** \file
 *  \addtogroup SrcFunc
 *  @{
 */
//#include <math.h>
#include "dwc_ddrphy_phyinit.h"

/** \brief This function programs registers that are normally set by training
 * firmware.
 *
 * This function is used in lieu of running 1D or 1D+2D training steps. PhyInit
 * calls this function when skip_train = 1 (SkipTrain) or 2 (DevInit). When
 * skip_train=1, PhyInit does not execute training firmware and this function is
 * called instead to program PHY registers according to DRAM timing parameters
 * specified in userInputSim data structure.  See documentation of
 * dwc_ddrphy_phyinit_struct.h file details of timing parameters available in
 * skip training.  When skip_train=2 is selected, PhyInit executes training
 * firmware with SequenceCtrl set in the training firmware message block to only
 * perform DRAM device initialization. This function is then called to program
 * PHY timing registers in lieu of running training.
 *
 * \warning dwc_ddrphy_phyinit_progCsrSkipTrain() only supports zero board
 * delay model.  If system board delays are set or randomized, full 1D or 1D/2D
 * initialization flow must be executed. 
 * 
 * This function replaces these steps in the PHY Initialization sequence:
 *
 *  - (E) Set the PHY input clocks to the desired frequency
 *  - (F) Write the Message Block parameters for the training firmware 
 *  - (G) Execute the Training Firmware 
 *  - (H) Read the Message Block results
 *
 * \returns \c void
 */

#if 0
int floor(float val) {
    int temp, result;
	temp = val * 10;
    if((temp%10) && (temp<0)) 
    {
    	result = temp/10 - 1;
    	return result;
    }
	else
	{
		result = temp/10;
        return result;
	}	
}
#endif
#if 0
int ceil(float val) {
    int temp, result;
	temp = val * 10;
    if((temp%10) && (temp>0)) 
    {
    	result = temp/10 + 1;
    	return result;
    }
	else
	{
		result = temp/10;
        return result;
	}	
}

float fmodf (float x, float y)
{
	int a;
	float temp;
	a=x/y;
	temp = x-a*y;
	return temp;
}

float fmod (float x, float y)
{
	int a;
	float temp;
	a=x/y;
	temp = x-a*y;
	return temp;
}

#endif 

void dwc_ddrphy_phyinit_progCsrSkipTrain (int skip_train) {

    char *printf_header;
    printf_header = "[dwc_ddrphy_phyinit_progCsrSkipTrain]";

    //dwc_ddrphy_phyinit_print ("\n\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// Training firmware is *NOT* executed. This function replaces these steps\n");
    //dwc_ddrphy_phyinit_cmnt ("// in the PHY Initialization sequence:\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("//  (E) Set the PHY input clocks to the desired frequency \n");
    //dwc_ddrphy_phyinit_cmnt ("//  (F) Write the Message Block parameters for the training firmware \n");
    //dwc_ddrphy_phyinit_cmnt ("//  (G) Execute the Training Firmware \n");
    //dwc_ddrphy_phyinit_cmnt ("//  (H) Read the Message Block results\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_print ("\n\n");

    int pstate;

    int byte;
    int c_addr;

    int nibble;
    int u_addr;

    int lane;
    int r_addr;

    int p_addr;
    float UIps;
    float dficlk_period;

    int tSTAOFF[4];
    int tPDM[4];
    int tCASL_add[4];

    //dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_progCsrSkipTrain()\n", printf_header);
    
    // Calculate total number of timing groups (ranks)
    int NumRank_total;
    NumRank_total = userInputBasic.NumRank_dfi0 + userInputBasic.NumRank_dfi1 * userInputBasic.Dfi1Exists;
    //dwc_ddrphy_phyinit_cmnt ("%s NumRank_total = %d\n", printf_header, NumRank_total);

    //##############################################################
    // 
    // set tSTAOFF and tPDM 
    // 
    //##############################################################
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {

        UIps = (1.0 / userInputBasic.Frequency[pstate]) * 1E6 * 0.5;;
       
    
        // LPDDR3 or LPDDR4
         tSTAOFF[pstate] = 0;
         tPDM[pstate]    = 0;
    

         // If tDPM > tSTAOFF, push out read/write by 1 Memclk
         if (tPDM[pstate] > tSTAOFF[pstate]) tCASL_add[pstate] = 2 * UIps;
         else                      tCASL_add[pstate] = 0;
    }

    //##############################################################
    // 
    // Program ATxDlY 
    //
    // for LPPDR3:    
    // Set CK ANIBs delay to 0, all other ANIBs to 0x10 (1/2 UI)
    //
    // For HMD and LPDDR4X Mode:
    // If MEMCLK <= 533 MHz make ATxDlY = 0x40
    //
    // For DDR4, DDR3 and LPDDR4, leave AtxDly[6:0] at default (0x0)
    //
    //##############################################################
    int ATxDly;

   
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
      p_addr = pstate << 20;
      
      //HMD
      if ( userInputBasic.HardMacroVer ==  3 && userInputBasic.Lp4xMode ==  1 && userInputBasic.Frequency[pstate] <= 533 ) {
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming ATxDly to 0x40 for all ACX4 macros\n", printf_header, pstate);
        ATxDly = 0x40;
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tANIB | cbrd | csr_ATxDly_ADDR), ATxDly);
      }
    }


    //##############################################################
    // 
    // Program DFIMRL per P-state according to this formula:
    //
    //         DFIMRL = ceiling( (ARdPtrinitval*UI + PHY_Tx_Insertion_Dly + PHY_Rx_Insertion_Dly + PHY_Rx_Fifo_dly + tDQSCK + tSTAOFF) / dficlk_period)
    // 
    // All terms in above equation specified in ps
    // tDQSCK - determine from memory model
    // tSTAOFF - determine from memory model if Dimm type is RDIMM or LRDIMM, otherwise
    // PHY_Tx_Insertion_Dly = 200ps
    // PHY_Rx_Insertion_Dly = 200ps
    // PHY_Rx_Fifo_Dly      = 200ps + 4UI
    // 
    //##############################################################
    int PHY_Tx_Insertion_Dly = 200;
    int PHY_Rx_Insertion_Dly = 200;
    int PHY_Rx_Fifo_Dly;


    int DFIMRL[4];
    float DFIMRL_in_ps;
    float DFIMRL_in_dficlk;
        

    // ### Set DFIMRL per p-state ### //
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        UIps = (1.0 / userInputBasic.Frequency[pstate]) * 1E6 * 0.5;;
        dficlk_period = 4 * UIps;
        
        PHY_Rx_Fifo_Dly = 200 + 4*UIps;
        //dwc_ddrphy_phyinit_cmnt ("%s PHY_Rx_Fifo_Dly = %d\n", printf_header, PHY_Rx_Fifo_Dly);
        //dwc_ddrphy_phyinit_cmnt ("%s PHY_Tx_Insertion_Dly = %d\n", printf_header, PHY_Tx_Insertion_Dly);
        //dwc_ddrphy_phyinit_cmnt ("%s PHY_Rx_Insertion_Dly = %d\n", printf_header, PHY_Rx_Insertion_Dly);

        DFIMRL_in_ps = (ARdPtrInitVal[pstate] * UIps) + PHY_Tx_Insertion_Dly + PHY_Rx_Insertion_Dly + PHY_Rx_Fifo_Dly + userInputSim.tDQSCK + tSTAOFF[pstate] + tCASL_add[pstate] + tPDM[pstate];
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DFIMRL_in_ps to %fps\n", printf_header, pstate, userInputBasic.Frequency[pstate], DFIMRL_in_ps);

		DFIMRL_in_dficlk = (float) DFIMRL_in_ps / dficlk_period;
		DFIMRL[pstate] = ceil(DFIMRL_in_dficlk) + mb_LPDDR4_1D[pstate].DFIMRLMargin;
		
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DFIMRL to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], DFIMRL[pstate]);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming HwtMRL to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], DFIMRL[pstate]);

        //dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | cbrd | csr_DFIMRL_ADDR), DFIMRL[pstate]);
        for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
            c_addr = byte << 12;
            dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | csr_DFIMRL_ADDR), DFIMRL[pstate]);
        }

        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_HwtMRL_ADDR), DFIMRL[pstate]);
    } 



    //##############################################################
    //
    // Program TxDqsDlyTg0/1/2/3[9:0] per P-State:
    //
    // If Dimm Type is RDIMM or LRDIMM, program as follow:
    //
    //         TxDqsDlyTg*[9:6] = floor( (4*UI + tSTAOFF) / UI)
    //         TxDqsDlyTg*[5:0] = ceiling( (tSTAOFF%UI / UI) * 32)
    //
    // tSTAOFF and UI expressed in ps
    //
    // For HMD and LPDDR4X and MEMCLK <= 533 mhz:
    //    TxDqsDlyTg*[9:6] = 0x5
    // 
    // For other dimm types, leave TDqsDlyTg*[9:0] at default 0x100
    // 
    // ppp_0001_cccc_uuuu_1101_0000
    //
    // if DDR3 or DDR4
    //      num_timingroup = NumRank_dfi0;
    // else
    //      num_timingroup = NumRank_dfi0 + NumRank_dfi1 * Dfi1Exists;
    //   
    // 
    //##############################################################
    int TxDqsDly[4];

    int TxDqsDlyTg_9to6;  // Coarse delay - 1UI per increment
    int TxDqsDlyTg_5to0;  // Fine delay - 1/32UI per increment

    int TxDqsDlyTg_coarse_default = 4;
    int TxDqsDlyTg_fine_default = 0;


    // ### Set per p-state ### //
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        UIps = (1.0 / userInputBasic.Frequency[pstate]) * 1E6 * 0.5;;
                
        //HMD 
        if ( userInputBasic.HardMacroVer ==  3 && userInputBasic.Lp4xMode ==  1 && userInputBasic.Frequency[pstate] <= 533 ) {          
          TxDqsDlyTg_9to6 = 0x5; 
        } else {
          TxDqsDlyTg_9to6 = floor( (TxDqsDlyTg_coarse_default * UIps + tSTAOFF[pstate] + tCASL_add[pstate] - tPDM[pstate]) / UIps); 
        }
        TxDqsDlyTg_5to0 = ceil( fmodf((TxDqsDlyTg_fine_default*UIps/32 + tSTAOFF[pstate] + tCASL_add[pstate] - tPDM[pstate]), UIps) / UIps * 32);                        
        
        // Bit-5 of LCDL is no longer used, so bumping bit-5 of fine_dly up to coarse_dly 
        if (TxDqsDlyTg_5to0 >= 32) {
            TxDqsDlyTg_9to6 = TxDqsDlyTg_9to6 + 1;
            TxDqsDlyTg_5to0 = TxDqsDlyTg_5to0 - 32;
        }

        TxDqsDly[pstate] = (TxDqsDlyTg_9to6 << 6) | TxDqsDlyTg_5to0;
        //TxDqsDly[pstate] = 0x100; //tonyh test temp
        if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x1) >> 0) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x1) >> 0)) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxDqsDlyTg0 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TxDqsDly[pstate]);
        if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x2) >> 1) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x2) >> 1)) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxDqsDlyTg1 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TxDqsDly[pstate]);
        if (0x0) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxDqsDlyTg2 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TxDqsDly[pstate]);
        if (0x0) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxDqsDlyTg3 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TxDqsDly[pstate]);
      
		for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
            c_addr = byte << 12;
            for (nibble=0; nibble < 2; nibble++) {
                u_addr = nibble << 8;
                if ( !dwc_ddrphy_phyinit_IsDbyteDisabled(byte) ) {
                    if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x1) >> 0) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x1) >> 0)) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg0_ADDR), TxDqsDly[pstate]);
                    if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x2) >> 1) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x2) >> 1)) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg1_ADDR), TxDqsDly[pstate]);
                    if (0x0) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg2_ADDR), TxDqsDly[pstate]);
                    if (0x0) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | u_addr | csr_TxDqsDlyTg3_ADDR), TxDqsDly[pstate]);
                  
                }
            }
        }
    }
    
    //##############################################################
    //
    // Program TxDqDlyTg0/1/2/3[8:0] per P-state:
    //
    // TxDqDlyTg0/1/2/3 - leave at default 16'h0010 for LPDDR3
    // 
    // DDR3/4 RDIMM or LRDIMM
    // LPDDR4
    //     TxDqDlyTg*[8:6] = floor( (TxDqsDlyTg*[5:0]*UI/32 + tDQS2DQ + 0.5UI) / UI)
    //     TxDqDlyTg*[5:0] = ceil( ((TxDqsDlyTg*[5:0]*UI/32 + tDQS2DQ + 0.5UI)%UI / UI) * 32 )
    // 
    //##############################################################
    int TxDqDly[4];

    int TxDqDly_8to6;  // Coarse delay - 1UI per increment
    int TxDqDly_5to0;  // Fine delay - 1/32UI per increment

    // ### Set per p-state ### //
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        UIps = (1.0 / userInputBasic.Frequency[pstate]) * 1E6 * 0.5;;
       
        TxDqsDlyTg_5to0 = TxDqsDly[pstate] & 0x3f;
        
        TxDqDly_8to6 = floor( (TxDqsDlyTg_5to0*UIps/32 + userInputSim.tDQS2DQ + UIps/2) / UIps ); 
        TxDqDly_5to0 = ceil( (fmodf((TxDqsDlyTg_5to0*UIps/32 + userInputSim.tDQS2DQ + UIps/2), UIps) / UIps) * 32 );
        
        // Bit-5 of LCDL is no longer used, so bumping bit-5 of fine_dly up to coarse_dly 
        if (TxDqDly_5to0 >= 32) {
            TxDqDly_8to6 = TxDqDly_8to6 + 1;
            TxDqDly_5to0 = TxDqDly_5to0 - 32;
        }

        TxDqDly[pstate] = (TxDqDly_8to6 << 6) | TxDqDly_5to0;

		//TxDqDly[pstate]=0x45; //tonyh test temp
      
        if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x1) >> 0) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x1) >> 0)) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxDqDlyTg0 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TxDqDly[pstate]);
        if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x2) >> 1) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x2) >> 1)) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxDqDlyTg1 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TxDqDly[pstate]);
        if (0x0) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxDqDlyTg2 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TxDqDly[pstate]);
        if (0x0) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming TxDqDlyTg3 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], TxDqDly[pstate]);
      


        for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
            c_addr = byte << 12;
            for (lane=0; lane < 9; lane++) {
                r_addr = lane << 8;
                if ( !dwc_ddrphy_phyinit_IsDbyteDisabled(byte) ) {
                  
                    if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x1) >> 0) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x1) >> 0)) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg0_ADDR), TxDqDly[pstate]);
                    if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x2) >> 1) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x2) >> 1)) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg1_ADDR), TxDqDly[pstate]);
                    if (0x0) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg2_ADDR), TxDqDly[pstate]);
                    if (0x0) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | r_addr | csr_TxDqDlyTg3_ADDR), TxDqDly[pstate]);
                  
                }
            }
        }
    }

    
    //##############################################################
    //
    // Program RxEnDly0/1/2/3[10:0] per P-State:
    //
    //         RxEnDly[10:6] = floor( (4*UI + tDQSCK + tSTAOFF) / UI)
    //         RxEnDly[5:0]  = ceil( ((tDQSCK + tSTAOFF) % UI) * 32 )
    //
    // tDQSCK, tSTAOFF and UI expressed in ps
    // tSTAOFF is 0 if dimm type is not RDIMM nor LRDIMM
    //
    //##############################################################
    int RxEnDly[4];

    int RxEnDly_10to6;  // Coarse delay - 1UI per increment
    int RxEnDly_5to0;  // Fine delay - 1/32UI per increment

    int RxEnDly_coarse_default = 4; 
    int RxEnDly_fine_default = 0;
    float RxEnDly_offset = 0.0;  // 0 offset is 1UI before the first DQS.
    
    
    // ### Set per p-state ### //
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;

        UIps = (1.0 / userInputBasic.Frequency[pstate]) * 1E6 * 0.5;;
       
        // Compensate for PptEnRxEnBackOff
        float BackOff = 0.0;
        float ZeroBackOff = 0.0;
        ZeroBackOff = 24.0/32.0;
        if ( userInputAdvanced.Lp4RxPreambleMode[pstate] == 1 ) { 
            BackOff = 1.0-(4.0/32.0); 
        }else{ 
            BackOff = userInputAdvanced.RxEnBackOff-(4.0/32.0);
        }

        if (skip_train == 1 && !userInputAdvanced.DisableRetraining ) {
            RxEnDly_offset = userInputBasic.Frequency[pstate] < 333 ?  BackOff*UIps : ZeroBackOff*UIps;
        } else if (skip_train == 1 && userInputAdvanced.DisableRetraining ) {
            RxEnDly_offset = ZeroBackOff*UIps;
        } else if (skip_train == 2 && !userInputAdvanced.DisableRetraining ) {
            RxEnDly_offset = ZeroBackOff*UIps;
        } else if (skip_train == 2 && userInputAdvanced.DisableRetraining ) {
            RxEnDly_offset = userInputBasic.Frequency[pstate] >= 333 ?  BackOff*UIps : ZeroBackOff*UIps;
        } else if (skip_train != 0 ) {
            dwc_ddrphy_phyinit_assert (0, "%s invalid skip_train settings. skip_train = %d, %d",printf_header, skip_train, userInputAdvanced.DisableRetraining); 
        }

        float FineStepPs = UIps/32.0; // fine steps in ps
        float totPs =  (32*RxEnDly_coarse_default*FineStepPs + RxEnDly_fine_default*FineStepPs +userInputSim.tDQSCK + tSTAOFF[pstate] + tCASL_add[pstate] + tPDM[pstate] + RxEnDly_offset) ; 
        float totFineStep = totPs / FineStepPs;
        RxEnDly_10to6 = totFineStep / 32.0; 
        RxEnDly_5to0  = fmod (totFineStep , 32.0);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming RxEnDly_10to6=%d, Rxendly_5to0=%d\n", printf_header, pstate, userInputBasic.Frequency[pstate], RxEnDly_10to6, RxEnDly_5to0);

        // Bit-5 of LCDL is no longer used, so bumping bit-5 of fine_dly up to coarse_dly 
        if (RxEnDly_5to0 >= 32) {
            RxEnDly_10to6 = RxEnDly_10to6 + 1;
            RxEnDly_5to0 = RxEnDly_5to0 - 32;
        }

        RxEnDly[pstate] = (RxEnDly_10to6 << 6) | RxEnDly_5to0;

      
        if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x1) >> 0) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x1) >> 0)) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming RxEnDlyTg0 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], RxEnDly[pstate]);
        if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x2) >> 1) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x2) >> 1)) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming RxEnDlyTg1 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], RxEnDly[pstate]);
        if (0x0) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming RxEnDlyTg2 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], RxEnDly[pstate]);
        if (0x0) dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming RxEnDlyTg3 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], RxEnDly[pstate]);
      

        for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
            c_addr = byte << 12;
            for (nibble=0; nibble < 2; nibble++) {
                u_addr = nibble << 8;
                if ( !dwc_ddrphy_phyinit_IsDbyteDisabled(byte) ) {
                  
                    if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x1) >> 0) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x1) >> 0)) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg0_ADDR), RxEnDly[pstate]);
                    if (((mb_LPDDR4_1D[pstate].CsPresentChA & 0x2) >> 1) | ((mb_LPDDR4_1D[pstate].CsPresentChB & 0x2) >> 1)) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg1_ADDR), RxEnDly[pstate]);
                    if (0x0) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg2_ADDR), RxEnDly[pstate]);
                    if (0x0) dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | u_addr | csr_RxEnDlyTg3_ADDR), RxEnDly[pstate]);
                  
                }
                   
            }
        }
    }

    
    uint16_t regData;
    //##############################################################
    //
    // Programming Seq0BGPR1/2/3 for LPDDR3 and LPDDR4
    //
    //##############################################################
    int extraDly;
    
        extraDly=3; // LPDDR4

    unsigned int p; 
    for(pstate=0; pstate<userInputBasic.NumPStates; pstate++) { // for each p_addr
        switch (pstate) {
        case 0:
          p=p0;
          break;
        case 1:
          p=p1;
          break;
        case 2:
          p=p2;
          break;
        case 3:
          p=p3;
          break;
        default:
          p=p0;
        }

        int rl=0,wl=0; // computed read/write latencies.
    
        uint16_t mr_wls, mr_dbi_rd, mr_rl, mr_wl ; // extracted feild from MR.
        mr_rl     = userInputAdvanced.Lp4RL[pstate]; // RL[2:0] 
        mr_wl     = userInputAdvanced.Lp4WL[pstate]; // WL[5:3] 
        mr_wls    = userInputAdvanced.Lp4WLS[pstate]; // WLS 
        mr_dbi_rd = userInputAdvanced.Lp4DbiRd[pstate]; // DBI-RD
        switch (mr_dbi_rd<<3 | mr_rl ) {
            // DBI-RD Disabled
            case  0: rl =  6; break;
            case  1: rl = 10; break;
            case  2: rl = userInputBasic.DramDataWidth == 8 ? 16 : 14; break;
            case  3: rl = userInputBasic.DramDataWidth == 8 ? 22 : 20; break;
            case  4: rl = userInputBasic.DramDataWidth == 8 ? 26 : 24; break;
            case  5: rl = userInputBasic.DramDataWidth == 8 ? 32 : 28; break;
            case  6: rl = userInputBasic.DramDataWidth == 8 ? 36 : 32; break;
            case  7: rl = userInputBasic.DramDataWidth == 8 ? 40 : 36; break;
            // DBI-RD Enabled
            case  8: rl =  6; break;
            case  9: rl = 12; break;
            case 10: rl = userInputBasic.DramDataWidth == 8 ? 18 : 16; break;
            case 11: rl = userInputBasic.DramDataWidth == 8 ? 24 : 22; break;
            case 12: rl = userInputBasic.DramDataWidth == 8 ? 30 : 28; break;
            case 13: rl = userInputBasic.DramDataWidth == 8 ? 36 : 32; break;
            case 14: rl = userInputBasic.DramDataWidth == 8 ? 40 : 36; break;
            case 15: rl = userInputBasic.DramDataWidth == 8 ? 44 : 40; break;
            default:rl =  6;
        }    
        switch (mr_wls<<3|mr_wl) {
            // DBI-RD Disabled
            case  0: wl =  4; break;
            case  1: wl =  6; break;
            case  2: wl =  8; break;
            case  3: wl = 10; break;
            case  4: wl = 12; break;
            case  5: wl = 14; break;
            case  6: wl = 16; break;
            case  7: wl = 18; break;
            // DBI-RD Enabled
            case  8: wl =  4; break;
            case  9: wl =  8; break;
            case 10: wl = 12; break;
            case 11: wl = 18; break;
            case 12: wl = 22; break;
            case 13: wl = 26; break;
            case 14: wl = 30; break;
            case 15: wl = 34; break;
            default: wl =  4;
        }    
    

        // Program Seq0b_GPRx
        dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Programming PIE RL=%d WL=%d\n", printf_header,pstate,rl,wl);
        regData = (rl-5+extraDly) << csr_AcsmRCasLat_LSB;
        dwc_ddrphy_phyinit_userCustom_io_write16((p | c0 | tINITENG | r2 | csr_Seq0BGPR1_ADDR), regData);
    
    
        //HMD
        if ( userInputBasic.HardMacroVer ==  3 && userInputBasic.Lp4xMode == 1)
          regData = 0;
        else
          regData = (wl-5+extraDly) << csr_AcsmWCasLat_LSB;
    

        dwc_ddrphy_phyinit_userCustom_io_write16((p | c0 | tINITENG | r2 | csr_Seq0BGPR2_ADDR), regData);

        regData = (rl-5+extraDly+4+8) << csr_AcsmRCasLat_LSB;
        dwc_ddrphy_phyinit_userCustom_io_write16((p | c0 | tINITENG | r2 | csr_Seq0BGPR3_ADDR), regData);
    
    } // for each p_addr

    //##############################################################
    //
    // Program HwtLpCsEnA and HwtLpCsEnB based on number of ranks per channel
    // Applicable only for LPDDR3 and LPDDR4.  These CSRs have no effect
    // for DDR3/4
    //
    // CSRs to program:
    //      HwtLpCsEnA
    //      HwtLpCsEnB
    //
    // User input dependencies:
    //      DramType
    //      Dfi1Exists
    //      NumRank_dfi0
    //      NumRank_dfi1
    //
    //##############################################################
    int HwtLpCsEnA;
    int HwtLpCsEnB;

    // Channel A - 1'b01 if signal-rank, 2'b11 if dual-rank
    HwtLpCsEnA = userInputBasic.NumRank_dfi0 | 0x1;
    //dwc_ddrphy_phyinit_cmnt ("%s Programming HwtLpCsEnA to 0x%x\n", printf_header, HwtLpCsEnA);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_HwtLpCsEnA_ADDR), HwtLpCsEnA);
    
    // Channel B - 1'b01 if signal-rank, 2'b11 if dual-rank 
    // if DFI1 exists but disabled, NumRank_dfi0 is used to program CsEnB
    if (userInputBasic.Dfi1Exists == 1 && userInputBasic.NumActiveDbyteDfi1 == 0) {
        HwtLpCsEnB = userInputBasic.NumRank_dfi0 | 0x1;
        //dwc_ddrphy_phyinit_cmnt ("%s Programming HwtLpCsEnB to 0x%x\n", printf_header, HwtLpCsEnB);
        dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_HwtLpCsEnB_ADDR), HwtLpCsEnB);
    } else if (userInputBasic.Dfi1Exists == 1 && userInputBasic.NumActiveDbyteDfi1 > 0) {
        HwtLpCsEnB = userInputBasic.NumRank_dfi1 | 0x1;
        //dwc_ddrphy_phyinit_cmnt ("%s Programming HwtLpCsEnB to 0x%x\n", printf_header, HwtLpCsEnB);
        dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_HwtLpCsEnB_ADDR), HwtLpCsEnB);
    } else { // Disable Channel B
        HwtLpCsEnB = 0x0;
        //dwc_ddrphy_phyinit_cmnt ("%s Programming HwtLpCsEnB to 0x%x\n", printf_header, HwtLpCsEnB);
        dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_HwtLpCsEnB_ADDR), HwtLpCsEnB);
    }


    //##############################################################
    //
    // Program PptDqsCntInvTrnTg0 and PptDqsCntInvTrnTg1 
    // Calculated based on tDQS2DQ and Frequencey
    // Applicable to LPDDR4 only
    //
    // 65536*(tdqs2dq_value_rank<rank>_chan<chan>*2)/(2*2048*UIps_int)
    //
    // CSRs to program:
    //      PptDqsCntInvTrnTg0
    //      PptDqsCntInvTrnTg1
    //
    // User input dependencies:
    //      tDQS2DQ
    //      Frequency
    //
    //##############################################################
    int rank;
    int PptDqsCntInvTrnTg0[4];
    int PptDqsCntInvTrnTg1[4];
    
    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        p_addr = pstate << 20;
        
        UIps = (1.0 / userInputBasic.Frequency[pstate]) * 1E6 * 0.5;;
   
        // ### Set per timing group ### //
        for (rank=0; rank<NumRank_total; rank++) {
            switch (rank) {
                case 0: {
                     PptDqsCntInvTrnTg0[pstate] = ceil (65536 * (userInputSim.tDQS2DQ * 2) / (2 * 2048 * UIps) );
                    //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming PptDqsCntInvTrnTg0 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], PptDqsCntInvTrnTg0[pstate]);
                    break; }
                case 1: {
                     PptDqsCntInvTrnTg1[pstate] = ceil (65536 * (userInputSim.tDQS2DQ * 2) / (2 * 2048 * UIps) );
                    //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming PptDqsCntInvTrnTg1 to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], PptDqsCntInvTrnTg1[pstate]);
                    break; }
            }

            for (byte=0; byte<userInputBasic.NumDbyte; byte++) {
                c_addr = byte << 12;
                if (rank == 0)
                    dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | csr_PptDqsCntInvTrnTg0_ADDR), PptDqsCntInvTrnTg0[pstate]);
                else if (rank == 1)
                    dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tDBYTE | c_addr | csr_PptDqsCntInvTrnTg1_ADDR), PptDqsCntInvTrnTg1[pstate]);
            }
        }
    }

    //##############################################################
    // CSRs to program:
    //      PptCtlStatic:: DOCByteSelTg0/1
    //                   :: PptEnRxEnBackOff
    //
    // User input dependencies::
    //      DramType
    //      DramDataWidth
    // Sim dependencies::
    //      DramByteSwap
    //##############################################################
    //dwc_ddrphy_phyinit_cmnt ("%s Programming PptCtlStatic CSR\n", printf_header );
    // The customer will setup some feilds in this csr so the fw needs to do a read-modify-write here.
    int PptEnRxEnBackOff ;
    // LPDDR4
    if ( userInputAdvanced.Lp4RxPreambleMode[0] == 1 ) { // Rx-preamble mode for PS0
//dwc_ddrphy_phyinit_cmnt ("%s Programming PptCtlStatic detected toggling preamble\n", printf_header );
        PptEnRxEnBackOff = 0x1; // toggling RD_PRE
    }else{ 
        PptEnRxEnBackOff = userInputAdvanced.RxEnBackOff; // static RD_PRE
    }
    
    unsigned int DOCByteTg0, DOCByteTg1;
	 DOCByteTg0 = 0;
	 DOCByteTg1 = 0;
    for (byte=0; byte < userInputBasic.NumDbyte ; byte++) { // Each Dbyte could have a different configuration.
      c_addr = byte*c1;
      if (userInputBasic.DramDataWidth==8) {

        // all ranks are bytemode
        if (mb_LPDDR4_1D[0].X8Mode == 0xf ) { DOCByteTg0 = 0x0; DOCByteTg1 = 0x0;}
        // no ranks are byte mode
        else if (mb_LPDDR4_1D[0].X8Mode == 0x0 && byte%2== 0) { DOCByteTg0 = 0x0; DOCByteTg1 = 0x0;}
        else if (mb_LPDDR4_1D[0].X8Mode == 0x0 && byte%2== 1) { DOCByteTg0 = 0x1; DOCByteTg1 = 0x1;}
        // rank0 is byte mode
        else if (mb_LPDDR4_1D[0].X8Mode == 0x5 && byte%2== 0) { DOCByteTg0 = 0x0; DOCByteTg1 = 0x0;}
        else if (mb_LPDDR4_1D[0].X8Mode == 0x5 && byte%2== 1) { DOCByteTg0 = 0x0; DOCByteTg1 = 0x1;}
        // rank1 is byte mode
        else if (mb_LPDDR4_1D[0].X8Mode == 0xa && byte%2== 0) { DOCByteTg0 = 0x0; DOCByteTg1 = 0x0;}
        else if (mb_LPDDR4_1D[0].X8Mode == 0xa && byte%2== 1) { DOCByteTg0 = 0x1; DOCByteTg1 = 0x0;}
        else {
          dwc_ddrphy_phyinit_assert(0, "%s Unexpected value for mb_LPDDR4_1D[0].X8Mode == %d", printf_header, mb_LPDDR4_1D[0].X8Mode);
        }
      }else if (byte%2==0){
        DOCByteTg0 = 0x1 & (userInputAdvanced.DramByteSwap>>byte);
        DOCByteTg1 = 0x1 & (userInputAdvanced.DramByteSwap>>byte);
      }else{
        DOCByteTg0 = 0x1 & ~(userInputAdvanced.DramByteSwap>>byte);
        DOCByteTg1 = 0x1 & ~(userInputAdvanced.DramByteSwap>>byte);
      }

      uint8_t PptEnTg1; 
      PptEnTg1 = ((userInputBasic.NumRank_dfi0 == 2) || (userInputBasic.NumRank_dfi1 == 2)) ? 0x1 : 0x0;
      regData = ( 0x1 << csr_PptEnDqs2DqTg0_LSB |
               PptEnTg1 << csr_PptEnDqs2DqTg1_LSB |
               0x1 << csr_PptEnRxEnDlyTg0_LSB |
               PptEnTg1 << csr_PptEnRxEnDlyTg1_LSB |
               PptEnRxEnBackOff << csr_PptEnRxEnBackOff_LSB |
               DOCByteTg0 << csr_DOCByteSelTg0_LSB | 
               DOCByteTg1 << csr_DOCByteSelTg1_LSB );
        
      dwc_ddrphy_phyinit_userCustom_io_write16((c_addr | tDBYTE | csr_PptCtlStatic_ADDR), regData);
    }
   

    //##############################################################
    //
    // Program HwtCAMode based on dram type 
    //
    // CSRs to program:
    //      HwtCAMode::HwtLp3CAMode
    //               ::HwtD4CAMode
    //               ::HwtLp4CAMode
    //               ::HwtD4AltCAMode
    //               ::HwtCsInvert
    //               ::HwtDBIInvert
    //
    // User input dependencies::
    //      DramType
    //
    //##############################################################
    int HwtCAMode;
    int HwtLp3CAMode = 0;
    int HwtD4CAMode = 0;
    int HwtLp4CAMode = 0;
    int HwtD4AltCAMode = 0;
    int HwtCsInvert = 0;
    int HwtDBIInvert = 0;
    
    // LPDDR4
    HwtLp4CAMode = 1;
    HwtCsInvert = 1;
    HwtDBIInvert = 1;

    HwtCAMode = (HwtDBIInvert << csr_HwtDBIInvert_LSB) | (HwtCsInvert << csr_HwtCsInvert_LSB) | (HwtD4AltCAMode << csr_HwtD4AltCAMode_LSB) | (HwtLp4CAMode << csr_HwtLp4CAMode_LSB) | (HwtD4CAMode << csr_HwtD4CAMode_LSB) | (HwtLp3CAMode << csr_HwtLp3CAMode_LSB);
    
    //dwc_ddrphy_phyinit_cmnt ("%s Programming HwtCAMode to 0x%x\n", printf_header, HwtCAMode);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_HwtCAMode_ADDR), HwtCAMode);
    

    //##############################################################
    //
    // Program DllGainCtl and DllLockParam based on Frequency
    //
    //##############################################################
    int memCK_freq;
    int lcdlSeed;
    int wdDllLockParam;
    int DllGainIV;
    int DllGainTV;
    int wdDllGainCtl;

    // nominal stepsize, in units of tenths of a ps,
    // if nominal=4.7ps use 47
    int stepsize_x10=47;

    for (pstate=0; pstate<userInputBasic.NumPStates; pstate++) {
        memCK_freq = userInputBasic.Frequency[pstate];
        // lcdlSeed = ((1000000/memCK_freq)/2)/lcdl_stepsize  ...where default lcdl_stepsize=4.7  in simulation
        p_addr = pstate << 20;

                                 DllGainIV=0x00; DllGainTV=0x02; 
        if(memCK_freq >=  332) { DllGainIV=0x01; DllGainTV=0x03; } 
        if(memCK_freq >=  532) { DllGainIV=0x02; DllGainTV=0x04; } 
        if(memCK_freq >=  800) { DllGainIV=0x03; DllGainTV=0x05; } 
        if(memCK_freq >= 1200) { DllGainIV=0x04; DllGainTV=0x05; }

        //lcdlSeed= (1000000/(2*memCK_freq)) * (100/(120*(stepsize_nominal)));
        lcdlSeed= (1000000 * 10 *100) /(2*memCK_freq * stepsize_x10 * 105); // *100/105 is to bias the seed low

        if(lcdlSeed > 511-32)  lcdlSeed = 511-32;
        if(lcdlSeed < 32    )  lcdlSeed = 32;  

        //                         lcdlSeed= 511; 
        //if(memCK_freq >=  250) { lcdlSeed= 425; } //0
        //if(memCK_freq >=  332) { lcdlSeed= 320; } //1
        //if(memCK_freq >=  400) { lcdlSeed= 265; } //1
        //if(memCK_freq >=  532) { lcdlSeed= 199; } //1
        //if(memCK_freq >=  666) { lcdlSeed= 159; } //2
        //if(memCK_freq >=  800) { lcdlSeed= 132; } //2 must be locked to 2lsb
        //if(memCK_freq >=  850) { lcdlSeed= 125; } //2 must be locked to 2lsb
        //if(memCK_freq >=  900) { lcdlSeed= 118; } //2 must be locked to 2lsb
        //if(memCK_freq >=  950) { lcdlSeed= 112; } // must be locked to 2lsb
        //if(memCK_freq >= 1000) { lcdlSeed= 106; } // must be locked to 2lsb
        //if(memCK_freq >= 1050) { lcdlSeed= 101; } // must be locked to 2lsb
        //if(memCK_freq >= 1100) { lcdlSeed=  96; } // must be locked to 2lsb
        //if(memCK_freq >= 1150) { lcdlSeed=  92; } // must be locked to 2lsb
        //if(memCK_freq >= 1200) { lcdlSeed=  88; } // must be locked to 2lsb
        //if(memCK_freq >= 1300) { lcdlSeed=  82; } // must be locked to 2lsb
        //if(memCK_freq >= 1400) { lcdlSeed=  76; } // must be locked to 2lsb
        //if(memCK_freq >= 1500) { lcdlSeed=  70; } // must be locked to 2lsb
        //if(memCK_freq >= 1600) { lcdlSeed=  66; } // must be locked to 2lsb
        //if(memCK_freq >= 1800) { lcdlSeed=  59; } // must be locked to 2lsb
        //if(memCK_freq >= 1900) { lcdlSeed=  56; } // must be locked to 2lsb
        //if(memCK_freq >= 2000) { lcdlSeed=  53; } // must be locked to 2lsb

        wdDllGainCtl   =  (csr_DllSeedSel_MASK & 0x0000) | (csr_DllGainTV_MASK  & (DllGainTV << csr_DllGainTV_LSB)) | (csr_DllGainIV_MASK  & (DllGainIV << csr_DllGainIV_LSB));

        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DllGainCtl::DllGainIV=0x%x, DllGainTV=0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], DllGainIV, DllGainTV);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DllGainCtl to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], wdDllGainCtl);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_DllGainCtl_ADDR), wdDllGainCtl);


        wdDllLockParam = (csr_LcdlSeed0_MASK & (lcdlSeed << csr_LcdlSeed0_LSB)) | (csr_DisDllGainIVSeed_MASK & 0xffff ) | (csr_DisDllSeedSel_MASK & 0x0000);

        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DllLockParam::LcdlSeed0 to %d \n", printf_header, pstate, userInputBasic.Frequency[pstate], lcdlSeed);
        //dwc_ddrphy_phyinit_cmnt ("%s Pstate=%d, Memclk=%dMHz, Programming DllLockParam to 0x%x\n", printf_header, pstate, userInputBasic.Frequency[pstate], wdDllLockParam);
        dwc_ddrphy_phyinit_userCustom_io_write16((p_addr | tMASTER | csr_DllLockParam_ADDR), wdDllLockParam);
    
    }


   
    //##############################################################
    //
    // Program AcsmCtrl23 for Fw and Ppt.
    //
    // CSRs to program:
    //   AcsmCtrl23::AcsmCsMask
	//               AcsmCsMode
    //
    // User input dependencies:
    //
    //##############################################################
    regData = (0x0f << csr_AcsmCsMask_LSB | 0x1 << csr_AcsmCsMode_LSB );
	//dwc_ddrphy_phyinit_cmnt ("%s Programming AcsmCtrl23 to 0x%x\n", printf_header, regData);
	dwc_ddrphy_phyinit_userCustom_io_write16((c0 | tACSM | csr_AcsmCtrl23_ADDR), regData);


    //##############################################################
    // 
    // Set PllForceCal to 1 and PllDacValIn to some arbitrary value
    //
    //##############################################################
    int dacval_in = 0x10;
    int force_cal = 1;
    int maxrange = 0x1f;
    int pllctrl3;

    pllctrl3 = (dacval_in << csr_PllDacValIn_LSB) | (force_cal << csr_PllForceCal_LSB) | (maxrange << csr_PllMaxRange_LSB);

	//dwc_ddrphy_phyinit_cmnt ("%s Programming PllCtrl3::PllDacValIn to 0x%x\n", printf_header, dacval_in);
	//dwc_ddrphy_phyinit_cmnt ("%s Programming PllCtrl3::PllForceCal to 0x%x\n", printf_header, force_cal);
	//dwc_ddrphy_phyinit_cmnt ("%s Programming PllCtrl3::PllMaxRange to 0x%x\n", printf_header, maxrange);
	//dwc_ddrphy_phyinit_cmnt ("%s Programming PllCtrl3 to 0x%x\n", printf_header, pllctrl3);

    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_PllCtrl3_ADDR), pllctrl3);


    //##############################################################
    //
    // Setting PhyInLP3 to 0 to cause PIE to execute LP2 sequence instead of INIT on first dfi_init_start
    // This prevents any DRAM commands before DRAM is initialized, which is the case for skip_train
    //
    // Moved to here from dwc_dddrphy_phyinit_I_loadPIEImage()
    // These should not be needed on S3-exit
    //
    // Note this executes for SkipTrain only, *not* DevInit+SkipTrain
    // DevInit+SkipTrain already initializes DRAM and thus don't need to avoid DRAM commands
    //
    //##############################################################
    if (skip_train == 1) {
        //dwc_ddrphy_phyinit_cmnt ("%s Special skipTraining configuration to Prevernt DRAM Commands on the first dfi status interface handshake.\n", printf_header);
        //dwc_ddrphy_phyinit_cmnt ("%s In order to see this behavior, the frist dfi_freq should be in the range of 0x0f < dfi_freq_sel[4:0] < 0x14.\n", printf_header);
        dwc_ddrphy_phyinit_userCustom_io_write16((tINITENG | csr_PhyInLP3_ADDR),                         0x0);
    }

//##############################################################
//##############################################################
    //dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_progCsrSkipTrain()\n", printf_header);
}
// End of dwc_ddrphy_phyinit_progCsrSkipTrain()
//#################################################################################################################
//#################################################################################################################


/** @} */
