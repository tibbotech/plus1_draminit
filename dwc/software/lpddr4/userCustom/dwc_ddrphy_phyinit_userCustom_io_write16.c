/** \file 
 * \brief implementation of APB write
 * \addtogroup CustFunc
 *  @{
 */
#include "dwc_ddrphy_phyinit.h"
extern char *ApbStr; // Pointer to special string for APB commands. defined in the dwc_ddrphy_phyinit_globals.c
/** \brief function to implement a register write to the PHY
 * 
 * The default behvior of dwc_ddrphy_phyinit_userCustom_io_write16() is to print
 * the APB write commands calculated by PhyInit. User can edit this function to
 * print differently, or implement a mechanism to trigger a APB write event in
 * simulation.
 *
 * \param adr 32-bit integer indicating address of CSR to be written
 * \param dat 16-bit integer for the value to be written to the CSR
 * \returns \c void
 */
void dwc_ddrphy_phyinit_userCustom_io_write16(uint32_t adr, uint16_t dat) {
    //dwc_ddrphy_phyinit_print ("%s", ApbStr);
    //dwc_ddrphy_phyinit_print ("dwc_ddrphy_apb_wr(0%x,0x%d);\n", adr, dat);
   	dwc_ddrphy_apb_wr(adr,dat);

    // Keep track of register adr/data pairs for retention exit
    dwc_ddrphy_phyinit_trackReg(adr);
}


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
/** @} */
