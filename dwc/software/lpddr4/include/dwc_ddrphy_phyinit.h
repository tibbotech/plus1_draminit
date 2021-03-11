/** \file */

#include <dwc_ddrphy_phyinit_userCustom.h>
/*! \def DWC_DDRPHY_PHYINIT_RID
 * cdefine for a PhyInit Revision ID
 */
#define DWC_DDRPHY_PHYINIT_RID 201810002

//#############################################################################
// Global Structures : instantiated in dwc_ddrphy_globals.c
//#############################################################################
extern runtime_config_t              runtimeConfig;

extern user_input_basic_t            userInputBasic;
extern user_input_advanced_t         userInputAdvanced;
extern user_input_sim_t              userInputSim;

extern PMU_SMB_LPDDR4_1D_t        mb_LPDDR4_1D[4];
extern PMU_SMB_LPDDR4_1D_t        shdw_LPDDR4_1D[4];
extern PMU_SMB_LPDDR4_2D_t        mb_LPDDR4_2D[4];
extern PMU_SMB_LPDDR4_2D_t        shdw_LPDDR4_2D[4];

// Function definitions
int dwc_ddrphy_phyinit_setMb (int ps, char *field, int value, int Train2D);
int dwc_ddrphy_phyinit_softSetMb (int ps, char *field, int value, int Train2D);
void dwc_ddrphy_phyinit_initStruct(int Train2D);
//void SP_REG_W(unsigned int addr,int value);
//unsigned short SP_REG_R(unsigned int addr);


char* ApbStr;   //tonyh test
