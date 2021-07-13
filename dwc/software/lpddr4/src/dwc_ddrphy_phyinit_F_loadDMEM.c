/** \file
 *  \addtogroup SrcFunc
 *  @{
 */
#include <stdlib.h>
#include <stdio.h>
#include "dwc_ddrphy_phyinit.h"

/** \brief This function loads the training firmware DMEM image and write the 
 *  Message Block parameters for the training firmware into the SRAM.
 * 
 *  This function performs the following tasks:
 *
 *  -# Load the firmware DMEM segment to initialize the data structures from the
 *  DMEM incv file provided in the training firmware package. 
 *  -# Write the Firmware Message Block with the required contents detailing the training parameters.
 *
 * \return void
 */

void dwc_ddrphy_phyinit_Write_DMEM(int ps, int Train2D) 
{
	if(Train2D == 0)
	{
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54000,(mb_LPDDR4_1D[ps].MsgMisc << 8)|(mb_LPDDR4_1D[ps].Reserved00));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54001,mb_LPDDR4_1D[ps].PmuRevision);
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54002,(mb_LPDDR4_1D[ps].PllBypassEn << 8)|(mb_LPDDR4_1D[ps].Pstate));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54003,(mb_LPDDR4_1D[ps].DRAMFreq));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54004,(mb_LPDDR4_1D[ps].BPZNResVal << 8)|(mb_LPDDR4_1D[ps].DfiFreqRatio));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54005,(mb_LPDDR4_1D[ps].PhyDrvImpedance << 8)|(mb_LPDDR4_1D[ps].PhyOdtImpedance));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54006,(mb_LPDDR4_1D[ps].Lp4Misc << 8)|(mb_LPDDR4_1D[ps].PhyVref));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54007,(mb_LPDDR4_1D[ps].Reserved0E));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54008,(mb_LPDDR4_1D[ps].SequenceCtrl));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54009,(mb_LPDDR4_1D[ps].Reserved13 << 8)|(mb_LPDDR4_1D[ps].HdtCtrl));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400a,(mb_LPDDR4_1D[ps].Reserved15 << 8)|(mb_LPDDR4_1D[ps].Reserved14));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400b,(mb_LPDDR4_1D[ps].Reserved17 << 8)|(mb_LPDDR4_1D[ps].DFIMRLMargin));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400c,(mb_LPDDR4_1D[ps].Lp4Quickboot << 8)|(mb_LPDDR4_1D[ps].UseBroadcastMR));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400d,(mb_LPDDR4_1D[ps].CATrainOpt << 8)|(mb_LPDDR4_1D[ps].Reserved1A));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400e,(mb_LPDDR4_1D[ps].Reserved1D << 8)|(mb_LPDDR4_1D[ps].X8Mode));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400f,(mb_LPDDR4_1D[ps].Share2DVrefResult << 8)|(mb_LPDDR4_1D[ps].Reserved1E));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54010,(mb_LPDDR4_1D[ps].Reserved21 << 8)|(mb_LPDDR4_1D[ps].Reserved20));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54011,(mb_LPDDR4_1D[ps].PhyConfigOverride));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54012,(mb_LPDDR4_1D[ps].CsPresentChA << 8)|(mb_LPDDR4_1D[ps].EnabledDQsChA));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54013,(mb_LPDDR4_1D[ps].CDD_ChA_RR_0_1 << 8)|(mb_LPDDR4_1D[ps].CDD_ChA_RR_1_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54014,(mb_LPDDR4_1D[ps].CDD_ChA_RW_1_0 << 8)|(mb_LPDDR4_1D[ps].CDD_ChA_RW_1_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54015,(mb_LPDDR4_1D[ps].CDD_ChA_RW_0_0 << 8)|(mb_LPDDR4_1D[ps].CDD_ChA_RW_0_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54016,(mb_LPDDR4_1D[ps].CDD_ChA_WR_1_0 << 8)|(mb_LPDDR4_1D[ps].CDD_ChA_WR_1_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54017,(mb_LPDDR4_1D[ps].CDD_ChA_WR_0_0 << 8)|(mb_LPDDR4_1D[ps].CDD_ChA_WR_0_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54018,(mb_LPDDR4_1D[ps].CDD_ChA_WW_0_1 << 8)|(mb_LPDDR4_1D[ps].CDD_ChA_WW_1_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54019,(mb_LPDDR4_1D[ps].MR2_A0 << 8)|(mb_LPDDR4_1D[ps].MR1_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401a,(mb_LPDDR4_1D[ps].MR4_A0 << 8)|(mb_LPDDR4_1D[ps].MR3_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401b,(mb_LPDDR4_1D[ps].MR12_A0 << 8)|(mb_LPDDR4_1D[ps].MR11_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401c,(mb_LPDDR4_1D[ps].MR14_A0 << 8)|(mb_LPDDR4_1D[ps].MR13_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401d,(mb_LPDDR4_1D[ps].MR17_A0 << 8)|(mb_LPDDR4_1D[ps].MR16_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401e,(mb_LPDDR4_1D[ps].MR24_A0 << 8)|(mb_LPDDR4_1D[ps].MR22_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401f,(mb_LPDDR4_1D[ps].MR2_A1 << 8)|(mb_LPDDR4_1D[ps].MR1_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54020,(mb_LPDDR4_1D[ps].MR4_A1 << 8)|(mb_LPDDR4_1D[ps].MR3_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54021,(mb_LPDDR4_1D[ps].MR12_A1 << 8)|(mb_LPDDR4_1D[ps].MR11_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54022,(mb_LPDDR4_1D[ps].MR14_A1 << 8)|(mb_LPDDR4_1D[ps].MR13_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54023,(mb_LPDDR4_1D[ps].MR17_A1 << 8)|(mb_LPDDR4_1D[ps].MR16_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54024,(mb_LPDDR4_1D[ps].MR24_A1 << 8)|(mb_LPDDR4_1D[ps].MR22_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54025,(mb_LPDDR4_1D[ps].CATerminatingRankChA));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54026,0x0);
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54027,0x0);
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54028,0x0);
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54029,0x0);
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402a,0x0);
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402b,(mb_LPDDR4_1D[ps].EnabledDQsChB << 8));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402c,(mb_LPDDR4_1D[ps].CDD_ChB_RR_1_0 << 8)|(mb_LPDDR4_1D[ps].CsPresentChB));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402d,(mb_LPDDR4_1D[ps].CDD_ChB_RW_1_1 << 8)|(mb_LPDDR4_1D[ps].CDD_ChB_RR_0_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402e,(mb_LPDDR4_1D[ps].CDD_ChB_RW_0_1 << 8)|(mb_LPDDR4_1D[ps].CDD_ChB_RW_1_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402f,(mb_LPDDR4_1D[ps].CDD_ChB_WR_1_1 << 8)|(mb_LPDDR4_1D[ps].CDD_ChB_RW_0_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54030,(mb_LPDDR4_1D[ps].CDD_ChB_WR_0_1 << 8)|(mb_LPDDR4_1D[ps].CDD_ChB_WR_1_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54031,(mb_LPDDR4_1D[ps].CDD_ChB_WW_1_0 << 8)|(mb_LPDDR4_1D[ps].CDD_ChB_WR_0_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54032,(mb_LPDDR4_1D[ps].MR1_B0 << 8)|(mb_LPDDR4_1D[ps].CDD_ChB_WW_0_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54033,(mb_LPDDR4_1D[ps].MR3_B0 << 8)|(mb_LPDDR4_1D[ps].MR2_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54034,(mb_LPDDR4_1D[ps].MR11_B0 << 8)|(mb_LPDDR4_1D[ps].MR4_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54035,(mb_LPDDR4_1D[ps].MR13_B0 << 8)|(mb_LPDDR4_1D[ps].MR12_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54036,(mb_LPDDR4_1D[ps].MR16_B0 << 8)|(mb_LPDDR4_1D[ps].MR14_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54037,(mb_LPDDR4_1D[ps].MR22_B0 << 8)|(mb_LPDDR4_1D[ps].MR17_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54038,(mb_LPDDR4_1D[ps].MR1_B1 << 8)|(mb_LPDDR4_1D[ps].MR24_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54039,(mb_LPDDR4_1D[ps].MR3_B1 << 8)|(mb_LPDDR4_1D[ps].MR2_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403a,(mb_LPDDR4_1D[ps].MR11_B1 << 8)|(mb_LPDDR4_1D[ps].MR4_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403b,(mb_LPDDR4_1D[ps].MR13_B1 << 8)|(mb_LPDDR4_1D[ps].MR12_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403c,(mb_LPDDR4_1D[ps].MR16_B1 << 8)|(mb_LPDDR4_1D[ps].MR14_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403d,(mb_LPDDR4_1D[ps].MR22_B1 << 8)|(mb_LPDDR4_1D[ps].MR17_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403e,(mb_LPDDR4_1D[ps].CATerminatingRankChB << 8)|(mb_LPDDR4_1D[ps].MR24_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403f,0x0);    
	}
	else
	{
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54000,(mb_LPDDR4_2D[ps].MsgMisc << 8)|(mb_LPDDR4_2D[ps].Reserved00));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54001,mb_LPDDR4_2D[ps].PmuRevision);
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54002,(mb_LPDDR4_2D[ps].PllBypassEn << 8)|(mb_LPDDR4_2D[ps].Pstate));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54003,(mb_LPDDR4_2D[ps].DRAMFreq));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54004,(mb_LPDDR4_2D[ps].BPZNResVal << 8)|(mb_LPDDR4_2D[ps].DfiFreqRatio));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54005,(mb_LPDDR4_2D[ps].PhyDrvImpedance << 8)|(mb_LPDDR4_2D[ps].PhyOdtImpedance));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54006,(mb_LPDDR4_2D[ps].Lp4Misc << 8)|(mb_LPDDR4_2D[ps].PhyVref));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54007,(mb_LPDDR4_2D[ps].CsTestFail << 8)|(mb_LPDDR4_2D[ps].Reserved0E));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54008,(mb_LPDDR4_2D[ps].SequenceCtrl));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54009,(mb_LPDDR4_2D[ps].Reserved13 << 8)|(mb_LPDDR4_2D[ps].HdtCtrl));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400a,(mb_LPDDR4_2D[ps].Reserved15 << 8)|(mb_LPDDR4_2D[ps].Reserved14));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400b,(mb_LPDDR4_2D[ps].Reserved17 << 8)|(mb_LPDDR4_2D[ps].DFIMRLMargin));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400c,(mb_LPDDR4_2D[ps].Reserved19 << 8)|(mb_LPDDR4_2D[ps].UseBroadcastMR));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400d,(mb_LPDDR4_2D[ps].CATrainOpt << 8)|(mb_LPDDR4_2D[ps].Reserved1A));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400e,(mb_LPDDR4_2D[ps].RX2D_TrainOpt << 8)|(mb_LPDDR4_2D[ps].X8Mode));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5400f,(mb_LPDDR4_2D[ps].Share2DVrefResult << 8)|(mb_LPDDR4_2D[ps].TX2D_TrainOpt));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54010,(mb_LPDDR4_2D[ps].Voltage_Weight2D<< 8)|(mb_LPDDR4_2D[ps].Delay_Weight2D));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54011,(mb_LPDDR4_2D[ps].PhyConfigOverride));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54012,(mb_LPDDR4_2D[ps].CsPresentChA << 8)|(mb_LPDDR4_2D[ps].EnabledDQsChA));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54013,(mb_LPDDR4_2D[ps].CDD_ChA_RR_0_1 << 8)|(mb_LPDDR4_2D[ps].CDD_ChA_RR_1_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54014,(mb_LPDDR4_2D[ps].CDD_ChA_RW_1_0 << 8)|(mb_LPDDR4_2D[ps].CDD_ChA_RW_1_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54015,(mb_LPDDR4_2D[ps].CDD_ChA_RW_0_0 << 8)|(mb_LPDDR4_2D[ps].CDD_ChA_RW_0_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54016,(mb_LPDDR4_2D[ps].CDD_ChA_WR_1_0 << 8)|(mb_LPDDR4_2D[ps].CDD_ChA_WR_1_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54017,(mb_LPDDR4_2D[ps].CDD_ChA_WR_0_0 << 8)|(mb_LPDDR4_2D[ps].CDD_ChA_WR_0_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54018,(mb_LPDDR4_2D[ps].CDD_ChA_WW_0_1 << 8)|(mb_LPDDR4_2D[ps].CDD_ChA_WW_1_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54019,(mb_LPDDR4_2D[ps].MR2_A0 << 8)|(mb_LPDDR4_2D[ps].MR1_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401a,(mb_LPDDR4_2D[ps].MR4_A0 << 8)|(mb_LPDDR4_2D[ps].MR3_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401b,(mb_LPDDR4_2D[ps].MR12_A0 << 8)|(mb_LPDDR4_2D[ps].MR11_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401c,(mb_LPDDR4_2D[ps].MR14_A0 << 8)|(mb_LPDDR4_2D[ps].MR13_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401d,(mb_LPDDR4_2D[ps].MR17_A0 << 8)|(mb_LPDDR4_2D[ps].MR16_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401e,(mb_LPDDR4_2D[ps].MR24_A0 << 8)|(mb_LPDDR4_2D[ps].MR22_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5401f,(mb_LPDDR4_2D[ps].MR2_A1 << 8)|(mb_LPDDR4_2D[ps].MR1_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54020,(mb_LPDDR4_2D[ps].MR4_A1 << 8)|(mb_LPDDR4_2D[ps].MR3_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54021,(mb_LPDDR4_2D[ps].MR12_A1 << 8)|(mb_LPDDR4_2D[ps].MR11_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54022,(mb_LPDDR4_2D[ps].MR14_A1 << 8)|(mb_LPDDR4_2D[ps].MR13_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54023,(mb_LPDDR4_2D[ps].MR17_A1 << 8)|(mb_LPDDR4_2D[ps].MR16_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54024,(mb_LPDDR4_2D[ps].MR24_A1 << 8)|(mb_LPDDR4_2D[ps].MR22_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54025,(mb_LPDDR4_2D[ps].Reserved4B << 8)|(mb_LPDDR4_2D[ps].CATerminatingRankChA));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54026,(mb_LPDDR4_2D[ps].TrainedVREFDQ_A0<< 8)|(mb_LPDDR4_2D[ps].Reserved4C));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54027,(mb_LPDDR4_2D[ps].RxClkDly_Margin_A0<< 8)|(mb_LPDDR4_2D[ps].TrainedVREFDQ_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54028,(mb_LPDDR4_2D[ps].TxDqDly_Margin_A0<< 8)|(mb_LPDDR4_2D[ps].VrefDac_Margin_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54029,(mb_LPDDR4_2D[ps].RxClkDly_Margin_A0<< 8)|(mb_LPDDR4_2D[ps].DeviceVref_Margin_A0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402a,(mb_LPDDR4_2D[ps].TxDqDly_Margin_A1<< 8)|(mb_LPDDR4_2D[ps].VrefDac_Margin_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402b,(mb_LPDDR4_2D[ps].EnabledDQsChB << 8)|(mb_LPDDR4_2D[ps].DeviceVref_Margin_A1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402c,(mb_LPDDR4_2D[ps].CDD_ChB_RR_1_0 << 8)|(mb_LPDDR4_2D[ps].CsPresentChB));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402d,(mb_LPDDR4_2D[ps].CDD_ChB_RW_1_1 << 8)|(mb_LPDDR4_2D[ps].CDD_ChB_RR_0_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402e,(mb_LPDDR4_2D[ps].CDD_ChB_RW_0_1 << 8)|(mb_LPDDR4_2D[ps].CDD_ChB_RW_1_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5402f,(mb_LPDDR4_2D[ps].CDD_ChB_WR_1_1 << 8)|(mb_LPDDR4_2D[ps].CDD_ChB_RW_0_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54030,(mb_LPDDR4_2D[ps].CDD_ChB_WR_0_1 << 8)|(mb_LPDDR4_2D[ps].CDD_ChB_WR_1_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54031,(mb_LPDDR4_2D[ps].CDD_ChB_WW_1_0 << 8)|(mb_LPDDR4_2D[ps].CDD_ChB_WR_0_0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54032,(mb_LPDDR4_2D[ps].MR1_B0 << 8)|(mb_LPDDR4_2D[ps].CDD_ChB_WW_0_1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54033,(mb_LPDDR4_2D[ps].MR3_B0 << 8)|(mb_LPDDR4_2D[ps].MR2_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54034,(mb_LPDDR4_2D[ps].MR11_B0 << 8)|(mb_LPDDR4_2D[ps].MR4_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54035,(mb_LPDDR4_2D[ps].MR13_B0 << 8)|(mb_LPDDR4_2D[ps].MR12_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54036,(mb_LPDDR4_2D[ps].MR16_B0 << 8)|(mb_LPDDR4_2D[ps].MR14_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54037,(mb_LPDDR4_2D[ps].MR22_B0 << 8)|(mb_LPDDR4_2D[ps].MR17_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54038,(mb_LPDDR4_2D[ps].MR1_B1 << 8)|(mb_LPDDR4_2D[ps].MR24_B0));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54039,(mb_LPDDR4_2D[ps].MR3_B1 << 8)|(mb_LPDDR4_2D[ps].MR2_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403a,(mb_LPDDR4_2D[ps].MR11_B1 << 8)|(mb_LPDDR4_2D[ps].MR4_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403b,(mb_LPDDR4_2D[ps].MR13_B1 << 8)|(mb_LPDDR4_2D[ps].MR12_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403c,(mb_LPDDR4_2D[ps].MR16_B1 << 8)|(mb_LPDDR4_2D[ps].MR14_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403d,(mb_LPDDR4_2D[ps].MR22_B1 << 8)|(mb_LPDDR4_2D[ps].MR17_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403e,(mb_LPDDR4_2D[ps].CATerminatingRankChB << 8)|(mb_LPDDR4_2D[ps].MR24_B1));
		dwc_ddrphy_phyinit_userCustom_io_write16(0x5403f,0x0);  
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54040,(mb_LPDDR4_2D[ps].TrainedVREFDQ_B1<< 8)|(mb_LPDDR4_2D[ps].TrainedVREFDQ_B0)); 
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54041,(mb_LPDDR4_2D[ps].VrefDac_Margin_B0<< 8)|(mb_LPDDR4_2D[ps].RxClkDly_Margin_B0)); 
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54042,(mb_LPDDR4_2D[ps].DeviceVref_Margin_B0<< 8)|(mb_LPDDR4_2D[ps].TxDqDly_Margin_B0)); 
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54043,(mb_LPDDR4_2D[ps].VrefDac_Margin_B1<< 8)|(mb_LPDDR4_2D[ps].RxClkDly_Margin_B1)); 
		dwc_ddrphy_phyinit_userCustom_io_write16(0x54044,(mb_LPDDR4_2D[ps].DeviceVref_Margin_B1<< 8)|(mb_LPDDR4_2D[ps].TxDqDly_Margin_B1)); 
	}
}


void dwc_ddrphy_phyinit_F_loadDMEM (int pstate, int Train2D) 
{
    
    char *printf_header;
    if (Train2D)
        printf_header = "// [phyinit_F_loadDMEM, 2D]";
    else
        printf_header = "// [phyinit_F_loadDMEM, 1D]";

    //dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_F_loadDMEM (pstate=%d, Train2D=%d)\n", printf_header, pstate, Train2D);

    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// (F) Load the %dD DMEM image and write the %dD Message Block parameters for the training firmware \n", Train2D+1, Train2D+1);
    //dwc_ddrphy_phyinit_cmnt ("// \n");
    //dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    //dwc_ddrphy_phyinit_cmnt ("// \n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");

    // set a pointer to the message block.
    PMU_SMB_LPDDR4_1D_t *msgBlkPtr;
    if (Train2D == 1) {
      msgBlkPtr = (PMU_SMB_LPDDR4_1D_t *)&mb_LPDDR4_2D[pstate];
    } else {
      msgBlkPtr = &mb_LPDDR4_1D[pstate];
    }   
    // Some basic checks on MessgeBlock
    if ( msgBlkPtr->EnabledDQsChA %16 != 0 || msgBlkPtr->EnabledDQsChB %16 != 0 )
      {
      dwc_ddrphy_phyinit_assert(0,"%s Lp3/Lp4 - Number of Dq's Enabled per Channel much be multipe of 16\n",printf_header);
      }
    if (( msgBlkPtr->EnabledDQsChA > 8*(userInputBasic.NumActiveDbyteDfi0)) || 
        ( msgBlkPtr->EnabledDQsChB > 8*(userInputBasic.NumActiveDbyteDfi1)) || 
        ( msgBlkPtr->EnabledDQsChA == 0 && msgBlkPtr->EnabledDQsChB == 0) 
       )
      {
      dwc_ddrphy_phyinit_assert(0,"%s EnabledDqsChA/B are not set correctly./1\n",printf_header);
      }
  #if 0
  int addr, mem_offset=0, sizeOfMsgBlk;
  int mem[DMEM_SIZE];
  return_offset_lastaddr_t return_type = return_lastaddr;    

  // initialize the dmem structure
  for (addr=0;addr<DMEM_SIZE;addr++) mem[addr]=0;

  if (Train2D == 1) { 	// 2-D
    mem_offset = dwc_ddrphy_phyinit_storeIncvFile (DMEM_INCV_FILENAME_2D, mem, return_type);
    sizeOfMsgBlk = sizeof(mb_LPDDR4_2D[pstate]);
    dwc_ddrphy_phyinit_storeMsgBlk(&(mb_LPDDR4_2D[pstate]), sizeOfMsgBlk, mem);
    }
  else
    { 								// 1-D
    mem_offset = dwc_ddrphy_phyinit_storeIncvFile (DMEM_INCV_FILENAME, mem, return_type);
    sizeOfMsgBlk = sizeof(mb_LPDDR4_1D[pstate]);
    dwc_ddrphy_phyinit_storeMsgBlk(&(mb_LPDDR4_1D[pstate]), sizeOfMsgBlk, mem);
    }

  // Write local dmem array
  if(0==(mem_offset%1)){//Always write an even number of words so no 32bit quantity is uninitialized                                               
    mem_offset++;
  }
  dwc_ddrphy_phyinit_WriteOutMem(mem, DMEM_ST_ADDR, (mem_offset-DMEM_ST_ADDR));
  #else
  dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);
  dwc_ddrphy_phyinit_F_loadDMEM_of_SP(pstate, Train2D); 
  dwc_ddrphy_phyinit_Write_DMEM(pstate, Train2D);
  dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
  #endif 
  //dwc_ddrphy_phyinit_cmnt("%s End of dwc_ddrphy_phyinit_F_loadDMEM()\n", printf_header);
  
}
/** @} */
