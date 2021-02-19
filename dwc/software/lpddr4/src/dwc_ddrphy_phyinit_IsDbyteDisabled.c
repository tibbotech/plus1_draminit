/** \file
 * \brief Helper function to determine dbyte should be disabled.
 * \addtogroup SrcFunc
 * @{
 */
#include <stdlib.h>
#include <math.h>
#include "dwc_ddrphy_phyinit.h"
/** @brief Helper function to determine if a given DByte is Disabled given PhyInit inputs.
 * @return 1 if disabled, 0 if enabled.
 */
int dwc_ddrphy_phyinit_IsDbyteDisabled(int DbyteNumber) {
    
    char *printf_header;
    printf_header = "// [dwc_ddrphy_phyinit_IsDbyteDisabled]";

    int DisableDbyte;
    DisableDbyte = 0; // default assume Dbyte is Enabled.

    int nad0 = userInputBasic.NumActiveDbyteDfi0;
    int nad1 = userInputBasic.NumActiveDbyteDfi1;

    if (nad0+nad1>userInputBasic.NumDbyte) { 
        dwc_ddrphy_phyinit_assert (0, "%s invalid PHY configuration:NumActiveDbyteDfi0(%d)+NumActiveDbyteDfi1(%d)>NumDbytes(%d).\n",printf_header,nad0,nad1,userInputBasic.NumDbyte);
    }

    // Implements Section 1.3 of Pub Databook
    if (userInputBasic.Dfi1Exists ) { 
        if (userInputBasic.NumActiveDbyteDfi1 == 0 ) { // only dfi0 (ChA) is enabled, dfi1 (ChB) disabled
            DisableDbyte = (DbyteNumber > userInputBasic.NumActiveDbyteDfi0-1 ) ? 1:0;
        } 
        else { // DFI1 enabled
				//tonyh test
			   // DisableDbyte = ((userInputBasic.NumActiveDbyteDfi0-1 < DbyteNumber) && (DbyteNumber < (floor(userInputBasic.NumDbyte/2)))) ? 1 :
               //                (DbyteNumber > (floor(userInputBasic.NumDbyte/2)+userInputBasic.NumActiveDbyteDfi1-1) ) ? 1:0;
        } 
    } 
    else if (userInputBasic.Dfi1Exists == 0x0) {
        DisableDbyte = (DbyteNumber > userInputBasic.NumActiveDbyteDfi0-1 ) ? 1:0;
    } 
    else {
        dwc_ddrphy_phyinit_assert(0,"%s invalid PHY configuration:Dfi1Exists is neither 1 or 0.\n",printf_header);
    }

    // Qualify results against MessageBlock 

    if ( mb_LPDDR4_1D[0].EnabledDQsChA < 1 ||  mb_LPDDR4_1D[0].EnabledDQsChA > 8*userInputBasic.NumActiveDbyteDfi0 ) {
        dwc_ddrphy_phyinit_assert(0,"%s EnabledDQsChA(%d). Value must be 0 < EnabledDQsChA < userInputBasic.NumActiveDbyteDfi0*8.\n",printf_header,mb_LPDDR4_1D[0].EnabledDQsChA);
    } 

    if ( userInputBasic.Dfi1Exists && userInputBasic.NumActiveDbyteDfi1 > 0 && 
         (mb_LPDDR4_1D[0].EnabledDQsChB < 1 ||  mb_LPDDR4_1D[0].EnabledDQsChB > 8*userInputBasic.NumActiveDbyteDfi1) ) {
        dwc_ddrphy_phyinit_assert(0,"%s EnabledDQsChB(%d). Value must be 0 < EnabledDQsChB < userInputBasic.NumActiveDbyteDfi1*8.\n",printf_header,mb_LPDDR4_1D[0].EnabledDQsChB);
    }  

     
    return DisableDbyte;
}
/** @} */
