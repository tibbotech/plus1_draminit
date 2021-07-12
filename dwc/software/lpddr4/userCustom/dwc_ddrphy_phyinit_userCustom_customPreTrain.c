/** \file
 *  \brief user customization before training
 *  \addtogroup CustFunc
 *  @{
 */
#include <stdio.h>
#include <stdlib.h>
#include "dwc_ddrphy_phyinit_userCustom.h"

/** @brief This function is called before training firmware is executed. Any
 * register override in this function might affect the firmware training
 * results.
 *
 * This function is executed before firmware execution loop. Thus this function
 * should be used only for the following:
 *
 *  - Override PHY register values written by
 *  dwc_ddrphy_phyinit_C_initPhyConfig.  An example use case is when this
 *  function does not perform the exact programing desired by the user.
 *  - Write custom PHY registers that need to take effect before training
 *  firmware execution.
 *
 * User shall use dwc_ddrphy_phyinit_userCustom_io_write16 to write PHY
 * registers in order for the register to be tracked by PhyInit for 
 * retention restore. 
 * 
 * \warning **IMPORTANT** In this function, user shall not override any values in
 * user_input_basic, user_input_advanced or messageBlock data structures. These
 * changes should be done in dwc_ddrphy_phyinit_userCustom_overrideUserInput().
 * Changing these structures affects steps C, I and other helper functions. At
 * this point of execution flow step C has finished and changes will only be
 * seen by Step I. This will cause unexpected and untested behavior of PhyInit.
 *                                                                                        
 * To override settings in the message block, users can use any of two methods:
 * -# calling dwc_ddrphy_phyinit_setMb(char *field, int value) function
 * -# assign values to the fields in the message block data structure directly
 * 
 * \ref examples/simple/dwc_ddrphy_phyinit_userCustom_customPreTrain.c example of this function 
 *
 * @return Void
 * 
 */
void dwc_ddrphy_phyinit_userCustom_customPreTrain () { 

    //char *printf_header;
    //printf_header = "// [phyinit_userCustom_customPreTrain]";

    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// dwc_ddrphy_phyihunit_userCustom_customPreTrain is a user-editable function.\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_userCustom_customPreTrain()\n", printf_header);
    //dwc_ddrphy_phyinit_print ("dwc_ddrphy_phyinit_userCustom_customPostTrain ();\n\n");

    //dwc_ddrphy_phyinit_userCustom_io_write16 (0xd0000, 0x0);
    //dwc_ddrphy_phyinit_print ("dwc_ddrphy_phyinit_userCustom_customPreTrain();\n\n");
    //dwc_ddrphy_phyinit_userCustom_io_write16 (0xd0000, 0x1);

    // === Example to write technology specific programming in PHY databook.
    // === using dwc_ddrphy_phyinit_setMb (int ps, char *field, int value, int Train2D)
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P0 | tMASTER | csr_PllCtrl1_ADDR), 0x21);
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P1 | tMASTER | csr_PllCtrl1_ADDR), 0x21);
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P2 | tMASTER | csr_PllCtrl1_ADDR), 0x21);
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P0 | tMASTER | csr_PllCtrl4_ADDR), 0x17f);
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P1 | tMASTER | csr_PllCtrl4_ADDR), 0x17f);
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P2 | tMASTER | csr_PllCtrl4_ADDR), 0x17f);

    // === Example to override SequenceCtrl in 2D message block for p-state 0
    // === using dwc_ddrphy_phyinit_setMb (int ps, char *field, int value, int Train2D)
    //dwc_ddrphy_phyinit_setMb (0, "SequenceCtrl", 0x0061, 1)  // p-state 0, 2D

    // === Example to override SequenceCtrl in 1D message block for each p-state
    // === by assigning the data structure sub-fields directly
    //mb_DDR4U_1D[0].SequenceCtrl = 0x031f;                     // p-state 0, 1D
    //mb_DDR4U_1D[1].SequenceCtrl = 0x031f;                     // p-state 1, 1D
    //mb_DDR4U_1D[2].SequenceCtrl = 0x031f;                     // p-state 2, 1D
    //mb_DDR4U_1D[3].SequenceCtrl = 0x031f;                     // p-state 3, 1D
    
    // === Example to override SequenceCtrl in 2D message block for p-state 0
    // === by assigning the data structure sub-fields directly
    //mb_DDR4U_2D[0].SequenceCtrl = 0x0061;                     // p-state 0, 2D


    // === Example to override value in CSR ARdPtrInitVal for all p-states 
    // === P0/P1/P2/P3, tMASTER, csr_ARdPtrInitVal_ADDR make up the full address of the CSR
    // === They are defined in dwc_ddrphy_csr_ALL_cdefines.h included in Phyinit source folder
    // === P0/P1/P2/P3             - p-state 0/1/2/3
    // === tMASTER                 - MASTER block-type
    // === csr_ARdPtrInitVal_ADDR  - register
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P0 | tMASTER | csr_ARdPtrInitVal_ADDR), 0x3);
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P1 | tMASTER | csr_ARdPtrInitVal_ADDR), 0x3);
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P2 | tMASTER | csr_ARdPtrInitVal_ADDR), 0x3);
    //dwc_ddrphy_phyinit_userCustom_io_write16 ((P3 | tMASTER | csr_ARdPtrInitVal_ADDR), 0x3);


    //dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_userCustom_customPreTrain()\n", printf_header);
}
/** @} */
