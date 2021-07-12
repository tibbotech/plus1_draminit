/** \file
 * \brief implements of a print function 
 *
 *  \addtogroup SrcFunc
 *  @{
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "dwc_ddrphy_phyinit.h"
extern FILE *outFilePtr; // defined in the dwc_ddrphy_phyinit_globals.c
extern char *CmntStr; // defined in dwc_ddrphy_phyinit globals.


/** @brief Wrapper to printf
 * 
 * This function is called by PhyInit to print comments to output txt file. The user
 * may have a different implementation as required by their \ref useModel.  In This
 * implementation for \ref useModel, the input string fmt is prepended with
 * commnt_string specified via the command line and then printed to the output
 * txt. See \ref ctrlout for more details. 
 *
 * @param[in] fmt Input
 *
 * @returns void
 */
void dwc_ddrphy_phyinit_cmnt (const char *fmt,...)
{
#if 0 //tonyh test 
	va_list argptr;
	char *header;
	unsigned int arg1,arg2;
	
	va_start(argptr,fmt); 
	if (*fmt == '%')
	{
	  header=va_arg(argptr,char *);    
	  prn_string(header);  
	  fmt+=2;
	}  
	arg1=va_arg(argptr,int); 
	arg2=va_arg(argptr,int);   
	prn_string_for_lpddr4(fmt,arg1,arg2);	
	va_end(argptr);
#endif
#if 0
	va_list argptr;
	va_start(argptr,fmt);
	fprintf(outFilePtr,"%s", CmntStr);
	vfprintf(outFilePtr,fmt, argptr);
	va_end(argptr);
#endif 
  return;
}
/** @} */
