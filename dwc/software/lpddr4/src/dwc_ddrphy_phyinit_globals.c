/** \file
 * \brief instantiates all the global structures.
 * \addtogroup SrcFunc
 * @{
 */

#include <stdio.h>
#include "dwc_ddrphy_phyinit.h"

/// stores input string from -comment_string
char* CmntStr       = ""; 
/// stores input string from -apb_string
char* ApbStr        = ""; 

// === Global Struct Defines === //
/// instance of runtime objects
runtime_config_t             runtimeConfig;
/// instance of useInputBasic
user_input_basic_t           userInputBasic;
/// instance of userInputAdvanced
user_input_advanced_t        userInputAdvanced;
/// instance of userInputSim
user_input_sim_t             userInputSim;

// === Firmware Message Block Structs === //
/// 1D message block instance
PMU_SMB_LPDDR4_1D_t        mb_LPDDR4_1D[4]; 
/// Shadow of 1D message block. Used by PhyInit to track user changes to the
/// data structure.
PMU_SMB_LPDDR4_1D_t        shdw_LPDDR4_1D[4]; 

/// 2D message block instance
PMU_SMB_LPDDR4_2D_t        mb_LPDDR4_2D[4];
/// Shadow of 2D message block. Used by PhyInit to track user changes to the
/// data structure.
PMU_SMB_LPDDR4_2D_t        shdw_LPDDR4_2D[4];

// === Global variables  === //
/// File pointer for out txtfile
FILE *outFilePtr;

/// PHY Identifier when using multiple PHY's
int CurrentPhyInst;

/*! Represent the value stored in Step C into the register with the same name.
 * Defined as a global variable so that implementation of
 * dwc_ddrphy_phyinit_progCsrSkipTrain() function does not require a PHY read
 * register implementation.
 */
int ARdPtrInitVal[4];  
/** @} */
