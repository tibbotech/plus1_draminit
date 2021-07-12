/** \file */
#include <string.h>
#include "dwc_ddrphy_phyinit.h"

/** \addtogroup SrcFunc
 *  @{
 */

/** @brief API to read PhyInit data structures 
 *
 *  This function can be used to read user_input_basic, user_input_advanced and
 *  user_input_sim data structures.
 *  
 *  Some fields are defined as arrays in the data structure. Example to set
 *  PllBypass for Pstate 3:
 *
 *  @code{.c}
 *  dwc_ddrphy_phyinit_getUserInput("PllBypass[3]", 0x1);
 *  @endcode
 *  
 *  \note field strings do not overlap between PhyInit structures.
 *
 *  @param[in]   field   A string representing the field to read. bracket
 *  notation can be used to set array fields. example  string: "PllBypass[0]"
 *  set the field UserInputBasic.PllBypass[0].
 *  fields is an array, 
 * 
 *  @return field value on success. -1 when string does not match fields in any oh PhyInit
 *  data structures.
 */
int dwc_ddrphy_phyinit_getUserInput (char *field) {
  
    char *printf_header;
    printf_header = "// [dwc_ddrphy_phyinit_getUserInput]";

        if( strcmp(field, "Frequency[0]") == 0)
                return userInputBasic.Frequency[0];
        else if( strcmp(field, "Frequency[1]") == 0)
                return userInputBasic.Frequency[1];
        else if( strcmp(field, "Frequency[2]") == 0)
                return userInputBasic.Frequency[2];
        else if( strcmp(field, "Frequency[3]") == 0)
                return userInputBasic.Frequency[3];
        else if( strcmp(field, "NumRank_dfi0") == 0)
                return userInputBasic.NumRank_dfi0;
        else if( strcmp(field, "ReadDBIEnable[0]") == 0)
                return userInputBasic.ReadDBIEnable[0];
        else if( strcmp(field, "ReadDBIEnable[1]") == 0)
                return userInputBasic.ReadDBIEnable[1];
        else if( strcmp(field, "ReadDBIEnable[2]") == 0)
                return userInputBasic.ReadDBIEnable[2];
        else if( strcmp(field, "ReadDBIEnable[3]") == 0)
                return userInputBasic.ReadDBIEnable[3];
        else if( strcmp(field, "Lp4xMode") == 0)
                return userInputBasic.Lp4xMode;
        else if( strcmp(field, "DimmType") == 0)
                return userInputBasic.DimmType;
        else if( strcmp(field, "DfiMode") == 0)
                return userInputBasic.DfiMode;
        else if( strcmp(field, "DramType") == 0)
                return userInputBasic.DramType;
        else if( strcmp(field, "HardMacroVer") == 0)
                return userInputBasic.HardMacroVer;
        else if( strcmp(field, "DfiFreqRatio[0]") == 0)
                return userInputBasic.DfiFreqRatio[0];
        else if( strcmp(field, "DfiFreqRatio[1]") == 0)
                return userInputBasic.DfiFreqRatio[1];
        else if( strcmp(field, "DfiFreqRatio[2]") == 0)
                return userInputBasic.DfiFreqRatio[2];
        else if( strcmp(field, "DfiFreqRatio[3]") == 0)
                return userInputBasic.DfiFreqRatio[3];
        else if( strcmp(field, "NumAnib") == 0)
                return userInputBasic.NumAnib;
        else if( strcmp(field, "NumDbyte") == 0)
                return userInputBasic.NumDbyte;
        else if( strcmp(field, "DramDataWidth") == 0)
                return userInputBasic.DramDataWidth;
        else if( strcmp(field, "PllBypass[0]") == 0)
                return userInputBasic.PllBypass[0];
        else if( strcmp(field, "PllBypass[1]") == 0)
                return userInputBasic.PllBypass[1];
        else if( strcmp(field, "PllBypass[2]") == 0)
                return userInputBasic.PllBypass[2];
        else if( strcmp(field, "PllBypass[3]") == 0)
                return userInputBasic.PllBypass[3];
        else if( strcmp(field, "Dfi1Exists") == 0)
                return userInputBasic.Dfi1Exists;
        else if( strcmp(field, "Train2D") == 0)
                return userInputBasic.Train2D;
        else if( strcmp(field, "NumRank_dfi1") == 0)
                return userInputBasic.NumRank_dfi1;
        else if( strcmp(field, "NumActiveDbyteDfi0") == 0)
                return userInputBasic.NumActiveDbyteDfi0;
        else if( strcmp(field, "NumPStates") == 0)
                return userInputBasic.NumPStates;
        else if( strcmp(field, "NumActiveDbyteDfi1") == 0)
                return userInputBasic.NumActiveDbyteDfi1;
        else if( strcmp(field, "TxSlewRiseAC") == 0)
                return userInputAdvanced.TxSlewRiseAC;
        else if( strcmp(field, "TxSlewFallDQ[0]") == 0)
                return userInputAdvanced.TxSlewFallDQ[0];
        else if( strcmp(field, "TxSlewFallDQ[1]") == 0)
                return userInputAdvanced.TxSlewFallDQ[1];
        else if( strcmp(field, "TxSlewFallDQ[2]") == 0)
                return userInputAdvanced.TxSlewFallDQ[2];
        else if( strcmp(field, "TxSlewFallDQ[3]") == 0)
                return userInputAdvanced.TxSlewFallDQ[3];
        else if( strcmp(field, "Lp4WLS[0]") == 0)
                return userInputAdvanced.Lp4WLS[0];
        else if( strcmp(field, "Lp4WLS[1]") == 0)
                return userInputAdvanced.Lp4WLS[1];
        else if( strcmp(field, "Lp4WLS[2]") == 0)
                return userInputAdvanced.Lp4WLS[2];
        else if( strcmp(field, "Lp4WLS[3]") == 0)
                return userInputAdvanced.Lp4WLS[3];
        else if( strcmp(field, "TxImpedance[0]") == 0)
                return userInputAdvanced.TxImpedance[0];
        else if( strcmp(field, "TxImpedance[1]") == 0)
                return userInputAdvanced.TxImpedance[1];
        else if( strcmp(field, "TxImpedance[2]") == 0)
                return userInputAdvanced.TxImpedance[2];
        else if( strcmp(field, "TxImpedance[3]") == 0)
                return userInputAdvanced.TxImpedance[3];
        else if( strcmp(field, "DramByteSwap") == 0)
                return userInputAdvanced.DramByteSwap;
        else if( strcmp(field, "CalInterval") == 0)
                return userInputAdvanced.CalInterval;
        else if( strcmp(field, "D4RxPreambleLength[0]") == 0)
                return userInputAdvanced.D4RxPreambleLength[0];
        else if( strcmp(field, "D4RxPreambleLength[1]") == 0)
                return userInputAdvanced.D4RxPreambleLength[1];
        else if( strcmp(field, "D4RxPreambleLength[2]") == 0)
                return userInputAdvanced.D4RxPreambleLength[2];
        else if( strcmp(field, "D4RxPreambleLength[3]") == 0)
                return userInputAdvanced.D4RxPreambleLength[3];
        else if( strcmp(field, "Lp4PostambleExt[0]") == 0)
                return userInputAdvanced.Lp4PostambleExt[0];
        else if( strcmp(field, "Lp4PostambleExt[1]") == 0)
                return userInputAdvanced.Lp4PostambleExt[1];
        else if( strcmp(field, "Lp4PostambleExt[2]") == 0)
                return userInputAdvanced.Lp4PostambleExt[2];
        else if( strcmp(field, "Lp4PostambleExt[3]") == 0)
                return userInputAdvanced.Lp4PostambleExt[3];
        else if( strcmp(field, "CalOnce") == 0)
                return userInputAdvanced.CalOnce;
        else if( strcmp(field, "SnpsUmctlF0RC5x[0]") == 0)
                return userInputAdvanced.SnpsUmctlF0RC5x[0];
        else if( strcmp(field, "SnpsUmctlF0RC5x[1]") == 0)
                return userInputAdvanced.SnpsUmctlF0RC5x[1];
        else if( strcmp(field, "SnpsUmctlF0RC5x[2]") == 0)
                return userInputAdvanced.SnpsUmctlF0RC5x[2];
        else if( strcmp(field, "SnpsUmctlF0RC5x[3]") == 0)
                return userInputAdvanced.SnpsUmctlF0RC5x[3];
        else if( strcmp(field, "WDQSExt") == 0)
                return userInputAdvanced.WDQSExt;
        else if( strcmp(field, "RxEnBackOff") == 0)
                return userInputAdvanced.RxEnBackOff;
        else if( strcmp(field, "Lp4LowPowerDrv") == 0)
                return userInputAdvanced.Lp4LowPowerDrv;
        else if( strcmp(field, "TrainSequenceCtrl") == 0)
                return userInputAdvanced.TrainSequenceCtrl;
        else if( strcmp(field, "EnableDfiCsPolarityFix") == 0)
                return userInputAdvanced.EnableDfiCsPolarityFix;
        else if( strcmp(field, "Lp4WL[0]") == 0)
                return userInputAdvanced.Lp4WL[0];
        else if( strcmp(field, "Lp4WL[1]") == 0)
                return userInputAdvanced.Lp4WL[1];
        else if( strcmp(field, "Lp4WL[2]") == 0)
                return userInputAdvanced.Lp4WL[2];
        else if( strcmp(field, "Lp4WL[3]") == 0)
                return userInputAdvanced.Lp4WL[3];
        else if( strcmp(field, "PhyMstrTrainInterval[0]") == 0)
                return userInputAdvanced.PhyMstrTrainInterval[0];
        else if( strcmp(field, "PhyMstrTrainInterval[1]") == 0)
                return userInputAdvanced.PhyMstrTrainInterval[1];
        else if( strcmp(field, "PhyMstrTrainInterval[2]") == 0)
                return userInputAdvanced.PhyMstrTrainInterval[2];
        else if( strcmp(field, "PhyMstrTrainInterval[3]") == 0)
                return userInputAdvanced.PhyMstrTrainInterval[3];
        else if( strcmp(field, "Lp4nWR[0]") == 0)
                return userInputAdvanced.Lp4nWR[0];
        else if( strcmp(field, "Lp4nWR[1]") == 0)
                return userInputAdvanced.Lp4nWR[1];
        else if( strcmp(field, "Lp4nWR[2]") == 0)
                return userInputAdvanced.Lp4nWR[2];
        else if( strcmp(field, "Lp4nWR[3]") == 0)
                return userInputAdvanced.Lp4nWR[3];
        else if( strcmp(field, "MemAlertPUImp") == 0)
                return userInputAdvanced.MemAlertPUImp;
        else if( strcmp(field, "TxSlewRiseDQ[0]") == 0)
                return userInputAdvanced.TxSlewRiseDQ[0];
        else if( strcmp(field, "TxSlewRiseDQ[1]") == 0)
                return userInputAdvanced.TxSlewRiseDQ[1];
        else if( strcmp(field, "TxSlewRiseDQ[2]") == 0)
                return userInputAdvanced.TxSlewRiseDQ[2];
        else if( strcmp(field, "TxSlewRiseDQ[3]") == 0)
                return userInputAdvanced.TxSlewRiseDQ[3];
        else if( strcmp(field, "DisableRetraining") == 0)
                return userInputAdvanced.DisableRetraining;
        else if( strcmp(field, "MemAlertVrefLevel") == 0)
                return userInputAdvanced.MemAlertVrefLevel;
        else if( strcmp(field, "Lp4DbiRd[0]") == 0)
                return userInputAdvanced.Lp4DbiRd[0];
        else if( strcmp(field, "Lp4DbiRd[1]") == 0)
                return userInputAdvanced.Lp4DbiRd[1];
        else if( strcmp(field, "Lp4DbiRd[2]") == 0)
                return userInputAdvanced.Lp4DbiRd[2];
        else if( strcmp(field, "Lp4DbiRd[3]") == 0)
                return userInputAdvanced.Lp4DbiRd[3];
        else if( strcmp(field, "Lp4RxPreambleMode[0]") == 0)
                return userInputAdvanced.Lp4RxPreambleMode[0];
        else if( strcmp(field, "Lp4RxPreambleMode[1]") == 0)
                return userInputAdvanced.Lp4RxPreambleMode[1];
        else if( strcmp(field, "Lp4RxPreambleMode[2]") == 0)
                return userInputAdvanced.Lp4RxPreambleMode[2];
        else if( strcmp(field, "Lp4RxPreambleMode[3]") == 0)
                return userInputAdvanced.Lp4RxPreambleMode[3];
        else if( strcmp(field, "DisDynAdrTri[0]") == 0)
                return userInputAdvanced.DisDynAdrTri[0];
        else if( strcmp(field, "DisDynAdrTri[1]") == 0)
                return userInputAdvanced.DisDynAdrTri[1];
        else if( strcmp(field, "DisDynAdrTri[2]") == 0)
                return userInputAdvanced.DisDynAdrTri[2];
        else if( strcmp(field, "DisDynAdrTri[3]") == 0)
                return userInputAdvanced.DisDynAdrTri[3];
        else if( strcmp(field, "Lp4DbiWr[0]") == 0)
                return userInputAdvanced.Lp4DbiWr[0];
        else if( strcmp(field, "Lp4DbiWr[1]") == 0)
                return userInputAdvanced.Lp4DbiWr[1];
        else if( strcmp(field, "Lp4DbiWr[2]") == 0)
                return userInputAdvanced.Lp4DbiWr[2];
        else if( strcmp(field, "Lp4DbiWr[3]") == 0)
                return userInputAdvanced.Lp4DbiWr[3];
        else if( strcmp(field, "SnpsUmctlOpt") == 0)
                return userInputAdvanced.SnpsUmctlOpt;
        else if( strcmp(field, "ODTImpedance[0]") == 0)
                return userInputAdvanced.ODTImpedance[0];
        else if( strcmp(field, "ODTImpedance[1]") == 0)
                return userInputAdvanced.ODTImpedance[1];
        else if( strcmp(field, "ODTImpedance[2]") == 0)
                return userInputAdvanced.ODTImpedance[2];
        else if( strcmp(field, "ODTImpedance[3]") == 0)
                return userInputAdvanced.ODTImpedance[3];
        else if( strcmp(field, "PhyInitSequenceNum") == 0)
                return userInputAdvanced.PhyInitSequenceNum;
        else if( strcmp(field, "DisablePhyUpdate") == 0)
                return userInputAdvanced.DisablePhyUpdate;
        else if( strcmp(field, "D4TxPreambleLength[0]") == 0)
                return userInputAdvanced.D4TxPreambleLength[0];
        else if( strcmp(field, "D4TxPreambleLength[1]") == 0)
                return userInputAdvanced.D4TxPreambleLength[1];
        else if( strcmp(field, "D4TxPreambleLength[2]") == 0)
                return userInputAdvanced.D4TxPreambleLength[2];
        else if( strcmp(field, "D4TxPreambleLength[3]") == 0)
                return userInputAdvanced.D4TxPreambleLength[3];
        else if( strcmp(field, "MemAlertEn") == 0)
                return userInputAdvanced.MemAlertEn;
        else if( strcmp(field, "MemAlertSyncBypass") == 0)
                return userInputAdvanced.MemAlertSyncBypass;
        else if( strcmp(field, "PhyMstrMaxReqToAck[0]") == 0)
                return userInputAdvanced.PhyMstrMaxReqToAck[0];
        else if( strcmp(field, "PhyMstrMaxReqToAck[1]") == 0)
                return userInputAdvanced.PhyMstrMaxReqToAck[1];
        else if( strcmp(field, "PhyMstrMaxReqToAck[2]") == 0)
                return userInputAdvanced.PhyMstrMaxReqToAck[2];
        else if( strcmp(field, "PhyMstrMaxReqToAck[3]") == 0)
                return userInputAdvanced.PhyMstrMaxReqToAck[3];
        else if( strcmp(field, "DisableUnusedAddrLns") == 0)
                return userInputAdvanced.DisableUnusedAddrLns;
        else if( strcmp(field, "ATxImpedance") == 0)
                return userInputAdvanced.ATxImpedance;
        else if( strcmp(field, "ExtCalResVal") == 0)
                return userInputAdvanced.ExtCalResVal;
        else if( strcmp(field, "TxSlewFallAC") == 0)
                return userInputAdvanced.TxSlewFallAC;
        else if( strcmp(field, "EnableHighClkSkewFix") == 0)
                return userInputAdvanced.EnableHighClkSkewFix;
        else if( strcmp(field, "Lp4RL[0]") == 0)
                return userInputAdvanced.Lp4RL[0];
        else if( strcmp(field, "Lp4RL[1]") == 0)
                return userInputAdvanced.Lp4RL[1];
        else if( strcmp(field, "Lp4RL[2]") == 0)
                return userInputAdvanced.Lp4RL[2];
        else if( strcmp(field, "Lp4RL[3]") == 0)
                return userInputAdvanced.Lp4RL[3];
        else if( strcmp(field, "Is2Ttiming[0]") == 0)
                return userInputAdvanced.Is2Ttiming[0];
        else if( strcmp(field, "Is2Ttiming[1]") == 0)
                return userInputAdvanced.Is2Ttiming[1];
        else if( strcmp(field, "Is2Ttiming[2]") == 0)
                return userInputAdvanced.Is2Ttiming[2];
        else if( strcmp(field, "Is2Ttiming[3]") == 0)
                return userInputAdvanced.Is2Ttiming[3];
        else if( strcmp(field, "tSTAOFF[0]") == 0)
                return userInputSim.tSTAOFF[0];
        else if( strcmp(field, "tSTAOFF[1]") == 0)
                return userInputSim.tSTAOFF[1];
        else if( strcmp(field, "tSTAOFF[2]") == 0)
                return userInputSim.tSTAOFF[2];
        else if( strcmp(field, "tSTAOFF[3]") == 0)
                return userInputSim.tSTAOFF[3];
        else if( strcmp(field, "tDQSCK") == 0)
                return userInputSim.tDQSCK;
        else if( strcmp(field, "tDQS2DQ") == 0)
                return userInputSim.tDQS2DQ;
        else if( strcmp(field, "tPDM[0]") == 0)
                return userInputSim.tPDM[0];
        else if( strcmp(field, "tPDM[1]") == 0)
                return userInputSim.tPDM[1];
        else if( strcmp(field, "tPDM[2]") == 0)
                return userInputSim.tPDM[2];
        else if( strcmp(field, "tPDM[3]") == 0)
                return userInputSim.tPDM[3];
        dwc_ddrphy_phyinit_assert(0,"%s Invalid field : %s\n", printf_header,field);
        return -1;
}
/** @} */
