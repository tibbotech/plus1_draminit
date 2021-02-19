/** 
 * \file
 * \brief maps impedance values to register settings
 *
 *  \addtogroup SrcFunc
 *  @{
 */
#include <stdlib.h>
#include <string.h>
#include "dwc_ddrphy_phyinit.h"

/** \brief maps impedance values to register settings  
 * 
 *  Reads the pull-up/pull-down driver impedance from DrvStren_ohm input
 *  and encodes that value for the CSR field specified in TargetCSR input,
 *  based on DDR protocol and user_input_basic.HardMacroVer.
 *
 *  @param[in] DrvStren_ohm drive strenght / ODT impedance in Ohms
 *  
 *  @param[in] TargetCSR Target CSR for the impedance value. on of following
 *  enum DrvType:
 *    - DrvStrenFSDqP
 *    - DrvStrenFSDqN
 *    - ODTStrenP
 *    - ODTStrenN
 *    - ADrvStren
 *    - ADrvStrenN
 *
 *  @param[in] HardMacroVer Hardmacro version. should be same as
 *  user_input_basic.HardMacroVer
 */
int dwc_ddrphy_phyinit_mapDrvStren (int DrvStren_ohm, enum DrvType TargetCSR, int HardMacroVer) {

    int Stren_setting;

    // String array which follows the order of the DrvType enum. When debugging this array is used to know which TargetCSR has been selected.
    char debugDrvType[6][14] = {"DrvStrenFSDqP", "DrvStrenFSDqN", "ODTStrenP", "ODTStrenN", "ADrvStrenP", "ADrvStrenN"};
    
    char *printf_header;
    printf_header = "// [phyinit_C_initPhyConfig]";
    
    if (HardMacroVer == 4 )  { 
    
      //--------------------------------------------------------------------------------------
      //HardMacro family E
      //--------------------------------------------------------------------------------------

      if ( TargetCSR == DrvStrenFSDqP ) {

      //......................................................................................
      // DrvStrenFSDqP
      //......................................................................................
      

        if ( userInputBasic.Lp4xMode == 1 ) {      
          if      (DrvStren_ohm == 0)   Stren_setting = 0x07; // High-impedance
          else if (DrvStren_ohm == 120) Stren_setting = 0x06;
          else if (DrvStren_ohm == 60)  Stren_setting = 0x04;
          else if (DrvStren_ohm == 40)  Stren_setting = 0x00;
          else {
            dwc_ddrphy_phyinit_assert(0, "%s userInputAdvanced.TxImpedance 0x%x Ohms value is not valid for Hardmacro family E. Valid values are: 0(HiZ), 40 Ohms, 60 Ohms and 120 Ohms.\n", printf_header, DrvStren_ohm);
          }        
        } else {
          if      (DrvStren_ohm == 0)   Stren_setting = 0x07; // High-impedance
          else if (DrvStren_ohm == 120) Stren_setting = 0x0F;
          else if (DrvStren_ohm == 60)  Stren_setting = 0x1F;
          else if (DrvStren_ohm == 40)  Stren_setting = 0x3F;
          else {
            dwc_ddrphy_phyinit_assert(0, "%s userInputAdvanced.TxImpedance 0x%x Ohms value is not valid for Hardmacro family E. Valid values are: 0(HiZ), 40 Ohms, 60 Ohms and 120 Ohms.\n", printf_header, DrvStren_ohm);
          }        
        }
        

      } else if ( TargetCSR == DrvStrenFSDqN ) { 
      
      //......................................................................................
      // DrvStrenFSDqN
      //......................................................................................
      
      
        if      (DrvStren_ohm == 0)   Stren_setting = 0x00; // High-impedance
        else if (DrvStren_ohm == 120) Stren_setting = 0x08;
        else if (DrvStren_ohm == 60)  Stren_setting = 0x18;
        else if (DrvStren_ohm == 40)  Stren_setting = 0x38;
        else {
          dwc_ddrphy_phyinit_assert (0, "%s userInputAdvanced.TxImpedance 0x%x Ohms value is not valid for Hardmacro family E. Valid values are: 0(HiZ), 40 Ohms, 60 Ohms and 120 Ohms.\n", printf_header, DrvStren_ohm);
        } 
               
      
      } else if ( TargetCSR == ODTStrenP ) {
      
      //......................................................................................
      // ODTStrenP
      //......................................................................................
      

        Stren_setting = 0x00; // High-impedance        
        
            
      } else if ( TargetCSR == ODTStrenN ) {
      
      //......................................................................................
      // ODTStrenN
      //......................................................................................
      

        if      (DrvStren_ohm == 0)   Stren_setting = 0x00; // High-impedance
        else if (DrvStren_ohm == 120) Stren_setting = 0x08;
        else if (DrvStren_ohm == 60)  Stren_setting = 0x18;
        else if (DrvStren_ohm == 40)  Stren_setting = 0x38;
        else {
          dwc_ddrphy_phyinit_assert (0, "%s userInputAdvanced.ODTImpedance 0x%x Ohms value is not valid for Hardmacro family E. Valid values are: 0(HiZ), 40 Ohms, 60 Ohms and 120 Ohms.\n", printf_header, DrvStren_ohm);
        }       
        
      
      
      } else if ( TargetCSR == ADrvStrenP ) {
      
      //......................................................................................
      // ADrvStrenP
      //......................................................................................
      

        if ( userInputBasic.Lp4xMode == 1 ) {      
          if      (DrvStren_ohm == 120) Stren_setting = 0x18;
          else if (DrvStren_ohm == 60)  Stren_setting = 0x10;
          else if (DrvStren_ohm == 40)  Stren_setting = 0x00;
          else {
            dwc_ddrphy_phyinit_assert (0, "%s userInputAdvanced.ATxImpedance 0x%x Ohms value is not valid for Hardmacro family E. Valid values are: 40 Ohms, 60 Ohms and 120 Ohms.\n", printf_header, DrvStren_ohm);
          }        
        } else {
          if      (DrvStren_ohm == 120) Stren_setting = 0x1C;
          else if (DrvStren_ohm == 60)  Stren_setting = 0x1D;
          else if (DrvStren_ohm == 40)  Stren_setting = 0x1F;
          else{
            dwc_ddrphy_phyinit_assert(0, "%s userInputAdvanced.ATxImpedance 0x%x Ohms value is not valid for Hardmacro family E. Valid values are: 40 Ohms, 60 Ohms and 120 Ohms.\n", printf_header, DrvStren_ohm);
          }        
        }
      
            
      } else if ( TargetCSR == ADrvStrenN ) {
      
      //......................................................................................
      // ADrvStrenN
      //......................................................................................
      
        if      (DrvStren_ohm == 120) Stren_setting = 0x00;
        else if (DrvStren_ohm == 60)  Stren_setting = 0x01;
        else if (DrvStren_ohm == 40)  Stren_setting = 0x03;
        else {
          dwc_ddrphy_phyinit_assert (0, "%s userInputAdvanced.ATxImpedance 0x%x Ohms value is not valid for Hardmacro family E. Valid values are: 40 Ohms, 60 Ohms and 120 Ohms.\n", printf_header, DrvStren_ohm);
        }        
      
      } else {
        dwc_ddrphy_phyinit_assert (0, "%s TargetCSR = %s (%d), in dwc_ddrphy_phyinit_mapDrvStren, is not valid.\n", printf_header, debugDrvType[TargetCSR], TargetCSR);
      } 
      
    
    } else {

      //--------------------------------------------------------------------------------------
      //Other HardMacro family
      //--------------------------------------------------------------------------------------
      
      if ( ( TargetCSR == DrvStrenFSDqP  ) || ( TargetCSR == DrvStrenFSDqN  ) ) {

        //......................................................................................
        // DrvStrenFSDqP or DrvStrenFSDqP
        //......................................................................................

        if      (DrvStren_ohm == 0 ) Stren_setting = 0x00; // High-impedance
        else if (DrvStren_ohm < 29 ) Stren_setting = 0x3f;
        else if (DrvStren_ohm < 31 ) Stren_setting = 0x3e;
        else if (DrvStren_ohm < 33 ) Stren_setting = 0x3b;
        else if (DrvStren_ohm < 35 ) Stren_setting = 0x3a;
        else if (DrvStren_ohm < 38 ) Stren_setting = 0x39;
        else if (DrvStren_ohm < 41 ) Stren_setting = 0x38;
        else if (DrvStren_ohm < 45 ) Stren_setting = 0x1b;
        else if (DrvStren_ohm < 50 ) Stren_setting = 0x1a;
        else if (DrvStren_ohm < 56 ) Stren_setting = 0x19;
        else if (DrvStren_ohm < 64 ) Stren_setting = 0x18;
        else if (DrvStren_ohm < 74 ) Stren_setting = 0x0b;
        else if (DrvStren_ohm < 88 ) Stren_setting = 0x0a;
        else if (DrvStren_ohm < 108) Stren_setting = 0x09;
        else if (DrvStren_ohm < 140) Stren_setting = 0x08;
        else if (DrvStren_ohm < 200) Stren_setting = 0x03;
        else if (DrvStren_ohm < 360) Stren_setting = 0x02;
        else if (DrvStren_ohm < 481) Stren_setting = 0x01;
        else                         Stren_setting = 0x00; // High-impedance 
        
      } else if ( TargetCSR == ODTStrenP ) {
            
        //......................................................................................
        // ODTStrenP
        //......................................................................................
      

        // LPDDR4 - P is high-Z
        
        Stren_setting = 0x00; // High-impedance
        
      
      } else if ( TargetCSR == ODTStrenN ) {
      
        //......................................................................................
        // ODTStrenN
        //......................................................................................
        

        // LPDDR4 - N is non-zero        
        
        if      (DrvStren_ohm == 0 )  Stren_setting = 0x00; // High-impedance
        else if (DrvStren_ohm < 29 )  Stren_setting = 0x3f;
        else if (DrvStren_ohm < 31 )  Stren_setting = 0x3e;
        else if (DrvStren_ohm < 33 )  Stren_setting = 0x3b;
        else if (DrvStren_ohm < 35 )  Stren_setting = 0x3a;
        else if (DrvStren_ohm < 38 )  Stren_setting = 0x39;
        else if (DrvStren_ohm < 41 )  Stren_setting = 0x38;
        else if (DrvStren_ohm < 45 )  Stren_setting = 0x1b;
        else if (DrvStren_ohm < 50 )  Stren_setting = 0x1a;
        else if (DrvStren_ohm < 56 )  Stren_setting = 0x19;
        else if (DrvStren_ohm < 64 )  Stren_setting = 0x18;
        else if (DrvStren_ohm < 74 )  Stren_setting = 0x0b;
        else if (DrvStren_ohm < 88 )  Stren_setting = 0x0a;
        else if (DrvStren_ohm < 108 ) Stren_setting = 0x09;
        else if (DrvStren_ohm < 140 ) Stren_setting = 0x08;
        else if (DrvStren_ohm < 200 ) Stren_setting = 0x03;
        else if (DrvStren_ohm < 360 ) Stren_setting = 0x02;
        else if (DrvStren_ohm < 481 ) Stren_setting = 0x01;
        else                          Stren_setting = 0x00; // High-impedance
        
        
      } else if ( ( TargetCSR == ADrvStrenP  ) || ( TargetCSR == ADrvStrenN  ) ) {

        //......................................................................................
        // ADrvStrenP or ADrvStrenN
        //......................................................................................

        if      (DrvStren_ohm == 120)   Stren_setting = 0x00;
        else if (DrvStren_ohm == 60)    Stren_setting = 0x01;
        else if (DrvStren_ohm == 40)    Stren_setting = 0x03;
        else if (DrvStren_ohm == 30)    Stren_setting = 0x07;
        else if (DrvStren_ohm == 24)    Stren_setting = 0x0F;
        else if (DrvStren_ohm == 20)    Stren_setting = 0x1F;     
        else {
          dwc_ddrphy_phyinit_assert (0, "%s userInputAdvanced.ATxImpedance 0x%x Ohms value is not valid. Valid values are: 20, 24, 30, 40, 60 and 120 Ohms.\n", printf_header, DrvStren_ohm);
        }

      } else {
        dwc_ddrphy_phyinit_assert (0, "%s TargetCSR = %s (%d), in dwc_ddrphy_phyinit_mapDrvStren, is not valid.\n", printf_header, debugDrvType[TargetCSR], TargetCSR);

      }    
      
    }
 
    return Stren_setting;
}
/** @} */ 
