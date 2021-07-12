/** \file
 * \brief Helper function to determine dbyte should be disabled.
 * \addtogroup SrcFunc
 * @{
 */
#include <stdlib.h>
//#include <math.h>
#include "dwc_ddrphy_phyinit.h"
/** @brief Helper function to determine if a given DByte is Disabled given PhyInit inputs.
 * @return 1 if disabled, 0 if enabled.
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
	//dwc_ddrphy_phyinit_cmnt ("%s userInputBasic.Dfi1Exists = %d\n", printf_header, userInputBasic.Dfi1Exists);

    // Implements Section 1.3 of Pub Databook
    if (userInputBasic.Dfi1Exists ) { 
        if (userInputBasic.NumActiveDbyteDfi1 == 0 ) { // only dfi0 (ChA) is enabled, dfi1 (ChB) disabled
            DisableDbyte = (DbyteNumber > userInputBasic.NumActiveDbyteDfi0-1 ) ? 1:0;
			dwc_ddrphy_phyinit_cmnt ("%s DisableDbyte00 = %d\n", printf_header, DisableDbyte);
		} 
        else { // DFI1 enabled

			//dwc_ddrphy_phyinit_cmnt ("%s userInputBasic.NumActiveDbyteDfi0 = %d\n", printf_header, userInputBasic.NumActiveDbyteDfi0);
			//dwc_ddrphy_phyinit_cmnt ("%s DbyteNumber = %d\n", printf_header, DbyteNumber);
			//dwc_ddrphy_phyinit_cmnt ("%s userInputBasic.NumDbyte = %d\n", printf_header, userInputBasic.NumDbyte);
			//dwc_ddrphy_phyinit_cmnt ("%s userInputBasic.NumActiveDbyteDfi1 = %d\n", printf_header, userInputBasic.NumActiveDbyteDfi1);
					  

			DisableDbyte = ((userInputBasic.NumActiveDbyteDfi0-1 < DbyteNumber) && (DbyteNumber < (floor(userInputBasic.NumDbyte/2)))) ? 1 :
                               (DbyteNumber > (floor(userInputBasic.NumDbyte/2)+userInputBasic.NumActiveDbyteDfi1-1) ) ? 1:0;
			//dwc_ddrphy_phyinit_cmnt ("%s DisableDbyte01 = %d\n", printf_header, DisableDbyte);
			//DisableDbyte = 0; //tonyh test temp
			//dwc_ddrphy_phyinit_cmnt ("%s DisableDbyte = %d\n", printf_header, DisableDbyte);
			
		} 
    } 
    else if (userInputBasic.Dfi1Exists == 0x0) {
        DisableDbyte = (DbyteNumber > userInputBasic.NumActiveDbyteDfi0-1 ) ? 1:0;
		dwc_ddrphy_phyinit_cmnt ("%s DisableDbyte02 = %d\n", printf_header, DisableDbyte);
	} 
    else {
        dwc_ddrphy_phyinit_assert(0,"%s invalid PHY configuration:Dfi1Exists is neither 1 or 0.\n",printf_header);
		dwc_ddrphy_phyinit_cmnt ("%s DisableDbyte03 = %d\n", printf_header, DisableDbyte);

	}

    // Qualify results against MessageBlock 

    if ( mb_LPDDR4_1D[0].EnabledDQsChA < 1 ||  mb_LPDDR4_1D[0].EnabledDQsChA > 8*userInputBasic.NumActiveDbyteDfi0 ) {
        dwc_ddrphy_phyinit_assert(0,"%s EnabledDQsChA(%d). Value must be 0 < EnabledDQsChA < userInputBasic.NumActiveDbyteDfi0*8.\n",printf_header,mb_LPDDR4_1D[0].EnabledDQsChA);
		dwc_ddrphy_phyinit_cmnt ("%s DisableDbyte04 = %d\n", printf_header, DisableDbyte);

	} 

    if ( userInputBasic.Dfi1Exists && userInputBasic.NumActiveDbyteDfi1 > 0 && 
         (mb_LPDDR4_1D[0].EnabledDQsChB < 1 ||  mb_LPDDR4_1D[0].EnabledDQsChB > 8*userInputBasic.NumActiveDbyteDfi1) ) {
        dwc_ddrphy_phyinit_assert(0,"%s EnabledDQsChB(%d). Value must be 0 < EnabledDQsChB < userInputBasic.NumActiveDbyteDfi1*8.\n",printf_header,mb_LPDDR4_1D[0].EnabledDQsChB);
		dwc_ddrphy_phyinit_cmnt ("%s DisableDbyte05 = %d\n", printf_header, DisableDbyte);

	}  

     
    return DisableDbyte;
}
/** @} */
