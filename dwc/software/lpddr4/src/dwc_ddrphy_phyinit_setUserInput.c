/** \file 
 * \brief API used to change PhyInit fields.
 * \addtogroup SrcFunc
 * @{
 */

#include <string.h>
#include "dwc_ddrphy_phyinit.h"

/** @brief API to program PhyInit data structures 
 *
 *  This function can be used to program user_input_basic, user_input_advanced,
 *  user_input_sim and runtime_config PhyInit data structures.  PhyInit uses the
 *  information in these data structures to program PHY registers.  For details
 *  information about the fields see the data structure documentation.
 *  
 *  Some fields are defined as arrays in the data structure. Example to set
 *  PllBypass for Pstate 3:
 *
 *  @code{.c}
 *  dwc_ddrphy_phyinit_setUserInput("PllBypass[3]", 0x1);
 *  @endcode
 *  
 *  \note field strings do not overlap between PhyInit structures.
 *
 *  @param[in]   field   A string representing the field to be programed. bracket
 *  notation can be used to set array fields. example  string: "PllBypass[0]"
 *  set the field UserInputBasic.PllBypass[0].
 *  feilds is an array, 
 *  @param[in]   value   value to be set on the field.
 * 
 *  @return 0 on success. -1 when string does not match fields in any oh PhyInit
 *  data structures.
 */
int dwc_ddrphy_phyinit_setUserInput (char *field, int value) {
  
    char *printf_header;
    printf_header = "// [dwc_ddrphy_phyinit_setUserInput]";

    if( strcmp(field, "debug") == 0)
            runtimeConfig.debug = value;
    else if( strcmp(field, "skip_train") == 0)
            runtimeConfig.skip_train = value;
    else if( strcmp(field, "Train2D") == 0)
            runtimeConfig.Train2D = value;
    else if( strcmp(field, "RetEn") == 0)
            runtimeConfig.RetEn = value;
    else if( strcmp(field, "Frequency[0]") == 0)
            userInputBasic.Frequency[0] = value;
    else if( strcmp(field, "Frequency[1]") == 0)
            userInputBasic.Frequency[1] = value;
    else if( strcmp(field, "Frequency[2]") == 0)
            userInputBasic.Frequency[2] = value;
    else if( strcmp(field, "Frequency[3]") == 0)
            userInputBasic.Frequency[3] = value;
    else if( strcmp(field, "NumRank_dfi0") == 0)
            userInputBasic.NumRank_dfi0 = value;
    else if( strcmp(field, "ReadDBIEnable[0]") == 0)
            userInputBasic.ReadDBIEnable[0] = value;
    else if( strcmp(field, "ReadDBIEnable[1]") == 0)
            userInputBasic.ReadDBIEnable[1] = value;
    else if( strcmp(field, "ReadDBIEnable[2]") == 0)
            userInputBasic.ReadDBIEnable[2] = value;
    else if( strcmp(field, "ReadDBIEnable[3]") == 0)
            userInputBasic.ReadDBIEnable[3] = value;
    else if( strcmp(field, "Lp4xMode") == 0)
            userInputBasic.Lp4xMode = value;
    else if( strcmp(field, "DimmType") == 0)
            userInputBasic.DimmType = value;
    else if( strcmp(field, "DfiMode") == 0)
            userInputBasic.DfiMode = value;
    else if( strcmp(field, "DramType") == 0)
            userInputBasic.DramType = value;
    else if( strcmp(field, "HardMacroVer") == 0)
            userInputBasic.HardMacroVer = value;
    else if( strcmp(field, "DfiFreqRatio[0]") == 0)
            userInputBasic.DfiFreqRatio[0] = value;
    else if( strcmp(field, "DfiFreqRatio[1]") == 0)
            userInputBasic.DfiFreqRatio[1] = value;
    else if( strcmp(field, "DfiFreqRatio[2]") == 0)
            userInputBasic.DfiFreqRatio[2] = value;
    else if( strcmp(field, "DfiFreqRatio[3]") == 0)
            userInputBasic.DfiFreqRatio[3] = value;
    else if( strcmp(field, "NumAnib") == 0)
            userInputBasic.NumAnib = value;
    else if( strcmp(field, "NumDbyte") == 0)
            userInputBasic.NumDbyte = value;
    else if( strcmp(field, "DramDataWidth") == 0)
            userInputBasic.DramDataWidth = value;
    else if( strcmp(field, "PllBypass[0]") == 0)
            userInputBasic.PllBypass[0] = value;
    else if( strcmp(field, "PllBypass[1]") == 0)
            userInputBasic.PllBypass[1] = value;
    else if( strcmp(field, "PllBypass[2]") == 0)
            userInputBasic.PllBypass[2] = value;
    else if( strcmp(field, "PllBypass[3]") == 0)
            userInputBasic.PllBypass[3] = value;
    else if( strcmp(field, "Dfi1Exists") == 0)
            userInputBasic.Dfi1Exists = value;
    else if( strcmp(field, "Train2D") == 0)
            userInputBasic.Train2D = value;
    else if( strcmp(field, "NumRank_dfi1") == 0)
            userInputBasic.NumRank_dfi1 = value;
    else if( strcmp(field, "NumActiveDbyteDfi0") == 0)
            userInputBasic.NumActiveDbyteDfi0 = value;
    else if( strcmp(field, "NumPStates") == 0)
            userInputBasic.NumPStates = value;
    else if( strcmp(field, "NumActiveDbyteDfi1") == 0)
            userInputBasic.NumActiveDbyteDfi1 = value;
    else if( strcmp(field, "TxSlewRiseAC") == 0)
            userInputAdvanced.TxSlewRiseAC = value;
    else if( strcmp(field, "TxSlewFallDQ[0]") == 0)
            userInputAdvanced.TxSlewFallDQ[0] = value;
    else if( strcmp(field, "TxSlewFallDQ[1]") == 0)
            userInputAdvanced.TxSlewFallDQ[1] = value;
    else if( strcmp(field, "TxSlewFallDQ[2]") == 0)
            userInputAdvanced.TxSlewFallDQ[2] = value;
    else if( strcmp(field, "TxSlewFallDQ[3]") == 0)
            userInputAdvanced.TxSlewFallDQ[3] = value;
    else if( strcmp(field, "Lp4WLS[0]") == 0)
            userInputAdvanced.Lp4WLS[0] = value;
    else if( strcmp(field, "Lp4WLS[1]") == 0)
            userInputAdvanced.Lp4WLS[1] = value;
    else if( strcmp(field, "Lp4WLS[2]") == 0)
            userInputAdvanced.Lp4WLS[2] = value;
    else if( strcmp(field, "Lp4WLS[3]") == 0)
            userInputAdvanced.Lp4WLS[3] = value;
    else if( strcmp(field, "TxImpedance[0]") == 0)
            userInputAdvanced.TxImpedance[0] = value;
    else if( strcmp(field, "TxImpedance[1]") == 0)
            userInputAdvanced.TxImpedance[1] = value;
    else if( strcmp(field, "TxImpedance[2]") == 0)
            userInputAdvanced.TxImpedance[2] = value;
    else if( strcmp(field, "TxImpedance[3]") == 0)
            userInputAdvanced.TxImpedance[3] = value;
    else if( strcmp(field, "DramByteSwap") == 0)
            userInputAdvanced.DramByteSwap = value;
    else if( strcmp(field, "CalInterval") == 0)
            userInputAdvanced.CalInterval = value;
    else if( strcmp(field, "D4RxPreambleLength[0]") == 0)
            userInputAdvanced.D4RxPreambleLength[0] = value;
    else if( strcmp(field, "D4RxPreambleLength[1]") == 0)
            userInputAdvanced.D4RxPreambleLength[1] = value;
    else if( strcmp(field, "D4RxPreambleLength[2]") == 0)
            userInputAdvanced.D4RxPreambleLength[2] = value;
    else if( strcmp(field, "D4RxPreambleLength[3]") == 0)
            userInputAdvanced.D4RxPreambleLength[3] = value;
    else if( strcmp(field, "Lp4PostambleExt[0]") == 0)
            userInputAdvanced.Lp4PostambleExt[0] = value;
    else if( strcmp(field, "Lp4PostambleExt[1]") == 0)
            userInputAdvanced.Lp4PostambleExt[1] = value;
    else if( strcmp(field, "Lp4PostambleExt[2]") == 0)
            userInputAdvanced.Lp4PostambleExt[2] = value;
    else if( strcmp(field, "Lp4PostambleExt[3]") == 0)
            userInputAdvanced.Lp4PostambleExt[3] = value;
    else if( strcmp(field, "CalOnce") == 0)
            userInputAdvanced.CalOnce = value;
    else if( strcmp(field, "SnpsUmctlF0RC5x[0]") == 0)
            userInputAdvanced.SnpsUmctlF0RC5x[0] = value;
    else if( strcmp(field, "SnpsUmctlF0RC5x[1]") == 0)
            userInputAdvanced.SnpsUmctlF0RC5x[1] = value;
    else if( strcmp(field, "SnpsUmctlF0RC5x[2]") == 0)
            userInputAdvanced.SnpsUmctlF0RC5x[2] = value;
    else if( strcmp(field, "SnpsUmctlF0RC5x[3]") == 0)
            userInputAdvanced.SnpsUmctlF0RC5x[3] = value;
    else if( strcmp(field, "WDQSExt") == 0)
            userInputAdvanced.WDQSExt = value;
    else if( strcmp(field, "RxEnBackOff") == 0)
            userInputAdvanced.RxEnBackOff = value;
    else if( strcmp(field, "Lp4LowPowerDrv") == 0)
            userInputAdvanced.Lp4LowPowerDrv = value;
    else if( strcmp(field, "TrainSequenceCtrl") == 0)
            userInputAdvanced.TrainSequenceCtrl = value;
    else if( strcmp(field, "EnableDfiCsPolarityFix") == 0)
            userInputAdvanced.EnableDfiCsPolarityFix = value;
    else if( strcmp(field, "Lp4WL[0]") == 0)
            userInputAdvanced.Lp4WL[0] = value;
    else if( strcmp(field, "Lp4WL[1]") == 0)
            userInputAdvanced.Lp4WL[1] = value;
    else if( strcmp(field, "Lp4WL[2]") == 0)
            userInputAdvanced.Lp4WL[2] = value;
    else if( strcmp(field, "Lp4WL[3]") == 0)
            userInputAdvanced.Lp4WL[3] = value;
    else if( strcmp(field, "PhyMstrTrainInterval[0]") == 0)
            userInputAdvanced.PhyMstrTrainInterval[0] = value;
    else if( strcmp(field, "PhyMstrTrainInterval[1]") == 0)
            userInputAdvanced.PhyMstrTrainInterval[1] = value;
    else if( strcmp(field, "PhyMstrTrainInterval[2]") == 0)
            userInputAdvanced.PhyMstrTrainInterval[2] = value;
    else if( strcmp(field, "PhyMstrTrainInterval[3]") == 0)
            userInputAdvanced.PhyMstrTrainInterval[3] = value;
    else if( strcmp(field, "Lp4nWR[0]") == 0)
            userInputAdvanced.Lp4nWR[0] = value;
    else if( strcmp(field, "Lp4nWR[1]") == 0)
            userInputAdvanced.Lp4nWR[1] = value;
    else if( strcmp(field, "Lp4nWR[2]") == 0)
            userInputAdvanced.Lp4nWR[2] = value;
    else if( strcmp(field, "Lp4nWR[3]") == 0)
            userInputAdvanced.Lp4nWR[3] = value;
    else if( strcmp(field, "MemAlertPUImp") == 0)
            userInputAdvanced.MemAlertPUImp = value;
    else if( strcmp(field, "TxSlewRiseDQ[0]") == 0)
            userInputAdvanced.TxSlewRiseDQ[0] = value;
    else if( strcmp(field, "TxSlewRiseDQ[1]") == 0)
            userInputAdvanced.TxSlewRiseDQ[1] = value;
    else if( strcmp(field, "TxSlewRiseDQ[2]") == 0)
            userInputAdvanced.TxSlewRiseDQ[2] = value;
    else if( strcmp(field, "TxSlewRiseDQ[3]") == 0)
            userInputAdvanced.TxSlewRiseDQ[3] = value;
    else if( strcmp(field, "DisableRetraining") == 0)
            userInputAdvanced.DisableRetraining = value;
    else if( strcmp(field, "MemAlertVrefLevel") == 0)
            userInputAdvanced.MemAlertVrefLevel = value;
    else if( strcmp(field, "Lp4DbiRd[0]") == 0)
            userInputAdvanced.Lp4DbiRd[0] = value;
    else if( strcmp(field, "Lp4DbiRd[1]") == 0)
            userInputAdvanced.Lp4DbiRd[1] = value;
    else if( strcmp(field, "Lp4DbiRd[2]") == 0)
            userInputAdvanced.Lp4DbiRd[2] = value;
    else if( strcmp(field, "Lp4DbiRd[3]") == 0)
            userInputAdvanced.Lp4DbiRd[3] = value;
    else if( strcmp(field, "Lp4RxPreambleMode[0]") == 0)
            userInputAdvanced.Lp4RxPreambleMode[0] = value;
    else if( strcmp(field, "Lp4RxPreambleMode[1]") == 0)
            userInputAdvanced.Lp4RxPreambleMode[1] = value;
    else if( strcmp(field, "Lp4RxPreambleMode[2]") == 0)
            userInputAdvanced.Lp4RxPreambleMode[2] = value;
    else if( strcmp(field, "Lp4RxPreambleMode[3]") == 0)
            userInputAdvanced.Lp4RxPreambleMode[3] = value;
    else if( strcmp(field, "DisDynAdrTri[0]") == 0)
            userInputAdvanced.DisDynAdrTri[0] = value;
    else if( strcmp(field, "DisDynAdrTri[1]") == 0)
            userInputAdvanced.DisDynAdrTri[1] = value;
    else if( strcmp(field, "DisDynAdrTri[2]") == 0)
            userInputAdvanced.DisDynAdrTri[2] = value;
    else if( strcmp(field, "DisDynAdrTri[3]") == 0)
            userInputAdvanced.DisDynAdrTri[3] = value;
    else if( strcmp(field, "Lp4DbiWr[0]") == 0)
            userInputAdvanced.Lp4DbiWr[0] = value;
    else if( strcmp(field, "Lp4DbiWr[1]") == 0)
            userInputAdvanced.Lp4DbiWr[1] = value;
    else if( strcmp(field, "Lp4DbiWr[2]") == 0)
            userInputAdvanced.Lp4DbiWr[2] = value;
    else if( strcmp(field, "Lp4DbiWr[3]") == 0)
            userInputAdvanced.Lp4DbiWr[3] = value;
    else if( strcmp(field, "SnpsUmctlOpt") == 0)
            userInputAdvanced.SnpsUmctlOpt = value;
    else if( strcmp(field, "ODTImpedance[0]") == 0)
            userInputAdvanced.ODTImpedance[0] = value;
    else if( strcmp(field, "ODTImpedance[1]") == 0)
            userInputAdvanced.ODTImpedance[1] = value;
    else if( strcmp(field, "ODTImpedance[2]") == 0)
            userInputAdvanced.ODTImpedance[2] = value;
    else if( strcmp(field, "ODTImpedance[3]") == 0)
            userInputAdvanced.ODTImpedance[3] = value;
    else if( strcmp(field, "PhyInitSequenceNum") == 0)
            userInputAdvanced.PhyInitSequenceNum = value;
    else if( strcmp(field, "DisablePhyUpdate") == 0)
            userInputAdvanced.DisablePhyUpdate = value;
    else if( strcmp(field, "D4TxPreambleLength[0]") == 0)
            userInputAdvanced.D4TxPreambleLength[0] = value;
    else if( strcmp(field, "D4TxPreambleLength[1]") == 0)
            userInputAdvanced.D4TxPreambleLength[1] = value;
    else if( strcmp(field, "D4TxPreambleLength[2]") == 0)
            userInputAdvanced.D4TxPreambleLength[2] = value;
    else if( strcmp(field, "D4TxPreambleLength[3]") == 0)
            userInputAdvanced.D4TxPreambleLength[3] = value;
    else if( strcmp(field, "MemAlertEn") == 0)
            userInputAdvanced.MemAlertEn = value;
    else if( strcmp(field, "MemAlertSyncBypass") == 0)
            userInputAdvanced.MemAlertSyncBypass = value;
    else if( strcmp(field, "PhyMstrMaxReqToAck[0]") == 0)
            userInputAdvanced.PhyMstrMaxReqToAck[0] = value;
    else if( strcmp(field, "PhyMstrMaxReqToAck[1]") == 0)
            userInputAdvanced.PhyMstrMaxReqToAck[1] = value;
    else if( strcmp(field, "PhyMstrMaxReqToAck[2]") == 0)
            userInputAdvanced.PhyMstrMaxReqToAck[2] = value;
    else if( strcmp(field, "PhyMstrMaxReqToAck[3]") == 0)
            userInputAdvanced.PhyMstrMaxReqToAck[3] = value;
    else if( strcmp(field, "DisableUnusedAddrLns") == 0)
            userInputAdvanced.DisableUnusedAddrLns = value;
    else if( strcmp(field, "ATxImpedance") == 0)
            userInputAdvanced.ATxImpedance = value;
    else if( strcmp(field, "ExtCalResVal") == 0)
            userInputAdvanced.ExtCalResVal = value;
    else if( strcmp(field, "TxSlewFallAC") == 0)
            userInputAdvanced.TxSlewFallAC = value;
    else if( strcmp(field, "EnableHighClkSkewFix") == 0)
            userInputAdvanced.EnableHighClkSkewFix = value;
    else if( strcmp(field, "Lp4RL[0]") == 0)
            userInputAdvanced.Lp4RL[0] = value;
    else if( strcmp(field, "Lp4RL[1]") == 0)
            userInputAdvanced.Lp4RL[1] = value;
    else if( strcmp(field, "Lp4RL[2]") == 0)
            userInputAdvanced.Lp4RL[2] = value;
    else if( strcmp(field, "Lp4RL[3]") == 0)
            userInputAdvanced.Lp4RL[3] = value;
    else if( strcmp(field, "Is2Ttiming[0]") == 0)
            userInputAdvanced.Is2Ttiming[0] = value;
    else if( strcmp(field, "Is2Ttiming[1]") == 0)
            userInputAdvanced.Is2Ttiming[1] = value;
    else if( strcmp(field, "Is2Ttiming[2]") == 0)
            userInputAdvanced.Is2Ttiming[2] = value;
    else if( strcmp(field, "Is2Ttiming[3]") == 0)
            userInputAdvanced.Is2Ttiming[3] = value;
    else if( strcmp(field, "tSTAOFF[0]") == 0)
            userInputSim.tSTAOFF[0] = value;
    else if( strcmp(field, "tSTAOFF[1]") == 0)
            userInputSim.tSTAOFF[1] = value;
    else if( strcmp(field, "tSTAOFF[2]") == 0)
            userInputSim.tSTAOFF[2] = value;
    else if( strcmp(field, "tSTAOFF[3]") == 0)
            userInputSim.tSTAOFF[3] = value;
    else if( strcmp(field, "tDQSCK") == 0)
            userInputSim.tDQSCK = value;
    else if( strcmp(field, "tDQS2DQ") == 0)
            userInputSim.tDQS2DQ = value;
    else if( strcmp(field, "tPDM[0]") == 0)
            userInputSim.tPDM[0] = value;
    else if( strcmp(field, "tPDM[1]") == 0)
            userInputSim.tPDM[1] = value;
    else if( strcmp(field, "tPDM[2]") == 0)
            userInputSim.tPDM[2] = value;
    else if( strcmp(field, "tPDM[3]") == 0)
            userInputSim.tPDM[3] = value;
    else {
		dwc_ddrphy_phyinit_assert(0,"%s unknown PhyInit field name '%s'\n", printf_header, field);
		return -1;
    }
	dwc_ddrphy_phyinit_cmnt ("%s Setting PHYINIT field '%s' to 0x%x\n", printf_header, field, value);
	return 0;
}
/** @} */
