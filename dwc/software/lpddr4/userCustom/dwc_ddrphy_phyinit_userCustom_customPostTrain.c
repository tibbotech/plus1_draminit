/** \file 
 *  \brief user customizations after training
 *  \addtogroup CustFunc
 *  @{
 */
#include "dwc_ddrphy_phyinit_userCustom.h"

/** \brief This function is called after the execution of training firmware is
 * complete. The user can use this function to override any CSR value programmed
 * by PhyInit or training firmware.
 *
 * The purpose of this function is to override any register values programmed
 * by the training firmware or dwc_ddrphy_phyinit_progCsrSkipTrain() when
 * skip_train is used. This function is executed after training firmware has
 * completed.
 *
 * \note **Warning!**
 * This function must not override any values in userInputBasic,
 * userInputAdvanced or message block data structures.  Only PHY register values
 * are to be modified with this function. To override PHY register values
 * programmed by PhyInit, users must use
 * dwc_ddrphy_phyinit_userCustom_io_write16() function calls placed within this
 * function.
 *
 * Sequence of Events in this function must be:
 * -# Enable APB access
 * -# Issue register writes
 * -# Isolate APB access
 *
 * A \ref examples/simple/dwc_ddrphy_phyinit_userCustom_customPostTrain.c example of this function 
 * 
 * \return void
 *
 */
void dwc_ddrphy_phyinit_userCustom_customPostTrain () { 

    //char *printf_header;
    //printf_header = "// [dwc_ddrphy_phyinit_userCustom_customPostTrain]";

   // dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// dwc_ddrphy_phyinit_userCustom_customPostTrain is a user-editable function.\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_print ("dwc_ddrphy_phyinit_userCustom_customPostTrain ();\n\n");



    //dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_userCustom_customPostTrain()\n", printf_header);
}

/** @} */
