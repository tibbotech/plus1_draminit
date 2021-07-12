/** @file
 *  @brief writes local memory content into the SRAM via APB interface.
 *  @addtogroup SrcFunc
 *  @{
 */
#include <stdio.h>
#include "dwc_ddrphy_phyinit.h"

/** 
 * @brief writes local memory content into the SRAM via APB interface.
 *
 * This function issued APB writes commands to SRAM address based on values
 * stored in a local PhyInit array that contains consolidated IMEM and DMEM
 * data. 
 * @param[in] mem[] Local memory array.
 * @param[in] mem_offset offset index. if provided, skips to the offset index
 * from the local array and issues APB commands from mem_offset to mem_size. 
 * @param[in] mem_size size of the memroy (in mem array index)
 * @returns void
 */
void dwc_ddrphy_phyinit_WriteOutMem (int mem[], int mem_offset, int mem_size) {
    
  //char *printf_header;
  //printf_header = "// [dwc_ddrphy_phyinit_WriteOutMem]";

  int index;
  //dwc_ddrphy_phyinit_cmnt ("// 1.	Enable access to the internal CSRs by setting the MicroContMuxSel CSR to 0.\n");
  //dwc_ddrphy_phyinit_cmnt ("//       This allows the memory controller unrestricted access to the configuration CSRs. \n");
  dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);


  //dwc_ddrphy_phyinit_cmnt ("%s STARTING. offset 0x%x size 0x%x\n",printf_header, mem_offset, mem_size);
  for (index=0;index<mem_size;index++) {
    // routine call option
    //   dwc_ddrphy_phyinit_print ("WriteOutMem: Attempting Write: Adr:0x%x Dat: 0x%x\n",index + mem_offset,mem[index]);
    dwc_ddrphy_phyinit_userCustom_io_write16(index + mem_offset,mem[index]);
	//fflush(stdout); tonyh test

  }
  //dwc_ddrphy_phyinit_cmnt ("%s DONE.  Index 0x%x\n",printf_header,index);
  //dwc_ddrphy_phyinit_cmnt ("// 2.	Isolate the APB access from the internal CSRs by setting the MicroContMuxSel CSR to 1. \n");
  //dwc_ddrphy_phyinit_cmnt ("//      This allows the firmware unrestricted access to the configuration CSRs. \n");
  dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
  //fflush(stdout); tonyh test

}
/** @} */
