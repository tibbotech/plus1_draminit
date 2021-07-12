/** \file 
 * \brief API to set internal data structures to pre-programmed user structures.
 * \addtogroup SrcFunc
 * @{
 */

#include <stdlib.h>
//#include <string.h>
#include "dwc_ddrphy_phyinit.h"

/** @brief API to program user_input_basic to pre-programmed structures 
 *
 * The function copies the content of the pointed data structure into the
 * global structure used by PhyInit.
 *
 * @param[in] structType   integer determines the type of input structure.
 * 
 *  value | PhyInit Data Structure Type
 *  ----- | ----------------------------
 *      0 | user_input_basic
 *      1 | user_input_advanced
 *      2 | user_input_sim
 *      3 | runtime_config
 *
 * @param[in] userStruct pointer to PhyInit data structure of structType.

 * @returns EXIT_SUCCESS / EXIT_FAILURE 
 */

typedef unsigned char		UINT8;
void *memcpy(UINT8 *dest, UINT8 *src, int n)
{
	UINT8 *src_end = (UINT8 *)((unsigned long)(src) + n);
	UINT8 *to = (UINT8 *)(dest);

	while (src != src_end) {
		*to = *src;
		to++;
		src++;
	}

	return (void *)(dest);
}

int dwc_ddrphy_phyinit_setStruct (const void *userStruct, int structType) {
#if 0  
    char *printf_header;
    printf_header = "// [dwc_ddrphy_phyinit_setStrcut]";

    if ( userStruct == NULL ) dwc_ddrphy_phyinit_assert (0, "%s userStruct is NULL.", printf_header);
    if ( structType < 0 ) dwc_ddrphy_phyinit_assert (0, "%s structType < 0 is invalid", printf_header);
    if ( structType > 3 ) dwc_ddrphy_phyinit_assert (0, "%s structType > 3 is invalid", printf_header);
#if 0
    switch (structType)
      {
        case 0 : 
            memcpy ( (void *) &userInputBasic, userStruct, sizeof (user_input_basic_t));
	        return EXIT_SUCCESS ;

        case 1 : 
            memcpy ( (void *) &userInputAdvanced, userStruct, sizeof (user_input_advanced_t));
	        return EXIT_SUCCESS ;

        case 2 : 
            memcpy ( (void *) &userInputSim, userStruct, sizeof (user_input_sim_t));
	        return EXIT_SUCCESS ;

        case 3 : 
            memcpy ( (void *) &runtimeConfig, userStruct, sizeof (runtime_config_t));
	        return EXIT_SUCCESS ;

        default :
            dwc_ddrphy_phyinit_assert (0, "%s structType < 0 is invalid", printf_header);
	        return EXIT_FAILURE ;
      }
#else
	switch (structType)
	  {
		case 0 : 
			memcpy ( (UINT8 *) &userInputBasic, userStruct, sizeof (user_input_basic_t));
			return EXIT_SUCCESS ;

		case 1 : 
			memcpy ( (UINT8 *) &userInputAdvanced, userStruct, sizeof (user_input_advanced_t));
			return EXIT_SUCCESS ;

		case 2 : 
			memcpy ( (UINT8 *) &userInputSim, userStruct, sizeof (user_input_sim_t));
			return EXIT_SUCCESS ;

		case 3 : 
			memcpy ( (UINT8 *) &runtimeConfig, userStruct, sizeof (runtime_config_t));
			return EXIT_SUCCESS ;

		default :
			dwc_ddrphy_phyinit_assert (0, "%s structType < 0 is invalid", printf_header);
			return EXIT_FAILURE ;
	  }

#endif 
#else
return 0;
#endif 
}
/** @} */
