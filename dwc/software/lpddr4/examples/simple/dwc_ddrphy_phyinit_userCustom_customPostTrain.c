#include "dwc_ddrphy_phyinit_userCustom.h"
void dwc_ddrphy_phyinit_userCustom_customPostTrain () { 

    char *printf_header;
    printf_header = "// [dwc_ddrphy_phyinit_userCustom_customPostTrain]";

    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("// dwc_ddrphy_phyinit_userCustom_customPostTrain is a user-editable function.\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_print ("dwc_ddrphy_phyinit_userCustom_customPostTrain ();\n\n");


    // 1. Enable APB access. 
    // Uncomments if you adding any register writes in this function.
    dwc_ddrphy_phyinit_cmnt ("// Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. \n");
    dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);

    // 2. Issue register writes
    // === Example to override value in CSR ARdPtrInitVal for ANIB 0-3 and P-state 0
    // === P0, tANIB, c0/c1/c2/c3, csr_ATxDly_ADDR make up the full address of the CSR
    // === They are defined in dwc_ddrphy_csr_ALL_cdefines.h included in Phyinit source folder
    // === P0               - p-state 0
    // === tANIB            - ANIB block-type
    // === c0/c1/c2/c3      - ANIB instance number
    // === csr_ATxDly_ADDR  - register
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P0 | tANIB | c0 | csr_ATxDly_ADDR), 0x12);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P0 | tANIB | c1 | csr_ATxDly_ADDR), 0x12);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P0 | tANIB | c2 | csr_ATxDly_ADDR), 0x12);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P0 | tANIB | c3 | csr_ATxDly_ADDR), 0x12);


    // === Example to override value in CSR TxDqsDlyTg0 for DBYTE 0-1, upper and lower 4-bits, and P-state 2
    // === P0, tANIB, c0/c1/c2/c3, csr_ATxDly_ADDR make up the full address of the CSR
    // === They are defined in dwc_ddrphy_csr_ALL_cdefines.h included in Phyinit source folder
    // === P2                   - p-state 2
    // === tDBYTE               - DBYTE block-type
    // === c0/c1/c2/c3          - DBYTE instance number
    // === u0/u1                - upper/lower 4-bits
    // === csr_TxDqsDlyTg0_ADDR  - register
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P2 | tDBYTE | c0 | u0 | csr_TxDqsDlyTg0_ADDR), 0x105);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P2 | tDBYTE | c0 | u1 | csr_TxDqsDlyTg0_ADDR), 0x105);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P2 | tDBYTE | c1 | u0 | csr_TxDqsDlyTg0_ADDR), 0x105);
    dwc_ddrphy_phyinit_userCustom_io_write16 ((P2 | tDBYTE | c1 | u1 | csr_TxDqsDlyTg0_ADDR), 0x105);

    // 3. Isolate APB access. 
    // Uncomments if you are adding any register writes in this function.
    dwc_ddrphy_phyinit_cmnt ("// Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. \n");
    dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);

    dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_userCustom_customPostTrain()\n", printf_header);
}

