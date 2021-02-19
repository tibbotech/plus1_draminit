#include <stdio.h>
#include <stdlib.h>
#include "dwc_ddrphy_phyinit_userCustom.h"

void dwc_ddrphy_phyinit_userCustom_customPreTrain () { 

    char *printf_header;
    printf_header = "// [phyinit_userCustom_customPreTrain]";

    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("// dwc_ddrphy_phyihunit_userCustom_customPreTrain is a user-editable function.\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_userCustom_customPreTrain()\n", printf_header);


    // === Example to override SequenceCtrl in 1D message block for each p-state
    // === using dwc_ddrphy_phyinit_setMb (int ps, char *field, int value, int Train2D)
    dwc_ddrphy_phyinit_setMb (0, "SequenceCtrl", 0x031f, 0)  // p-state 0, 1D
    dwc_ddrphy_phyinit_setMb (1, "SequenceCtrl", 0x031f, 0)  // p-state 1, 1D
    dwc_ddrphy_phyinit_setMb (2, "SequenceCtrl", 0x031f, 0)  // p-state 2, 1D
    dwc_ddrphy_phyinit_setMb (3, "SequenceCtrl", 0x031f, 0)  // p-state 3, 1D

    // === Example to override SequenceCtrl in 2D message block for p-state 0
    // === using dwc_ddrphy_phyinit_setMb (int ps, char *field, int value, int Train2D)
    dwc_ddrphy_phyinit_setMb (0, "SequenceCtrl", 0x0061, 1)  // p-state 0, 2D

    // === Example to override SequenceCtrl in 1D message block for each p-state
    // === by assigning the data structure sub-fields directly
    mb_DDR4U_1D[0].SequenceCtrl = 0x031f;                     // p-state 0, 1D
    mb_DDR4U_1D[1].SequenceCtrl = 0x031f;                     // p-state 1, 1D
    mb_DDR4U_1D[2].SequenceCtrl = 0x031f;                     // p-state 2, 1D
    mb_DDR4U_1D[3].SequenceCtrl = 0x031f;                     // p-state 3, 1D
    
    // === Example to override SequenceCtrl in 2D message block for p-state 0
    // === by assigning the data structure sub-fields directly
    mb_DDR4U_2D[0].SequenceCtrl = 0x0061;                     // p-state 0, 2D


    // === Example to override value in CSR ARdPtrInitVal for all p-states 
    // === P0/P1/P2/P3, tMASTER, csr_ARdPtrInitVal_ADDR make up the full address of the CSR
    // === They are defined in dwc_ddrphy_csr_ALL_cdefines.h included in Phyinit source folder
    // === P0/P1/P2/P3             - p-state 0/1/2/3
    // === tMASTER                 - MASTER block-type
    // === csr_ARdPtrInitVal_ADDR  - register
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P0 | tMASTER | csr_ARdPtrInitVal_ADDR), 0x3);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P1 | tMASTER | csr_ARdPtrInitVal_ADDR), 0x3);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P2 | tMASTER | csr_ARdPtrInitVal_ADDR), 0x3);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P3 | tMASTER | csr_ARdPtrInitVal_ADDR), 0x3);


    dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_userCustom_customPreTrain()\n", printf_header);
}
