/** \file
 *  \addtogroup SrcFunc
 *  @{
 */
#include <stdlib.h>
#include <stdio.h>
#include <dwc_ddrphy_phyinit.h>


/** \brief This function loads the training firmware IMEM image into the SRAM.
 * 
 *  This function reads the incv files form the firmware package to generate a
 *  set of apb writes to load IMEM image into the SRAM. The exact steps in this
 *  function are as follows:
 *
 *  -# Ensure DRAM is in reset.
 *  -# Load the microcontroller memory with the provided training firmware
 *  -# Initialize the firmware mailbox structures to be able to communicate with
 *  the firmware (see "Mailbox facility for firmware" in the "DesignWare Cores
 *  DDR PHY Training Application Note".
 *
 * \return void
 */
 

void dwc_ddrphy_phyinit_D_loadIMEM (int Train2D) {
    
    char *printf_header;
    if (Train2D)
        printf_header = "// [dwc_ddrphy_phyinit_D_loadIMEM, 2D]";
    else
        printf_header = "// [dwc_ddrphy_phyinit_D_loadIMEM, 1D]";
    
    dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_D_loadIMEM (Train2D=%d)\n", printf_header, Train2D);

    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("//\n");
    dwc_ddrphy_phyinit_cmnt ("// (D) Load the %dD IMEM image\n", Train2D+1);
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("// This function loads the training firmware IMEM image into the SRAM.\n");
    dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    dwc_ddrphy_phyinit_cmnt ("// \n");
    dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    dwc_ddrphy_phyinit_cmnt ("\n");
    dwc_ddrphy_phyinit_cmnt ("\n");


  /*
   * Set MemResetL to avoid glitch on BP_MemReset_L during training
   */

  {
		struct xboot_hdr *xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET);
		int len;
		prn_dword(xhdr->magic);
		prn_dword(xhdr->length);
		prn_dword(xhdr->checksum);
		len = sizeof(struct xboot_hdr)	+ xhdr->length;
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + len);
		int len0;
		prn_dword(xhdr->magic);
		prn_dword(xhdr->length);
		prn_dword(xhdr->checksum);
		len0 = len + sizeof(struct xboot_hdr) + xhdr->length;
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + len0);
		int len1;
		prn_dword(xhdr->magic);
		prn_dword(xhdr->length);
		prn_dword(xhdr->checksum);
		len1 = len0 + sizeof(struct xboot_hdr)	+ xhdr->length;
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + len1);
		int len2;
		prn_dword(xhdr->magic);
		prn_dword(xhdr->length);
		prn_dword(xhdr->checksum);
		len2 = len1 + sizeof(struct xboot_hdr)	+ xhdr->length;
		xhdr = (struct xboot_hdr*)(SPI_FLASH_BASE + SPI_XBOOT_OFFSET + len2);
		int len3;
		prn_dword(xhdr->magic);
		prn_dword(xhdr->length);
		prn_dword(xhdr->checksum);
		len3 = len2 + sizeof(struct xboot_hdr)	+ xhdr->length;		

	}

  
#if 0   
  int MemResetL;

  if (Train2D == 0) 
    { // 1-D 
    MemResetL = csr_ProtectMemReset_MASK;
    dwc_ddrphy_phyinit_cmnt ("%s Programming MemResetL to 0x%x\n", printf_header, MemResetL);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_MemResetL_ADDR), MemResetL);
    }

  int addr, mem_offset=0;
  int mem[IMEM_SIZE];
  return_offset_lastaddr_t return_type = return_offset;

  // initialize the dmem structure
  for (addr=0;addr<IMEM_SIZE;addr++) mem[addr]=0;

  // Read the IMEM INCV file into the array
  if (Train2D == 1) 
    { // 2-D
    mem_offset = dwc_ddrphy_phyinit_storeIncvFile (IMEM_INCV_FILENAME_2D, mem, return_type);
    }
  else
    {
    mem_offset = dwc_ddrphy_phyinit_storeIncvFile (IMEM_INCV_FILENAME, mem, return_type);
    }

  // Write local imem array
  dwc_ddrphy_phyinit_WriteOutMem(mem, mem_offset, IMEM_SIZE);

  //dwc_ddrphy_phyinit_cmnt("%s WriteImem: COMPLETED\n", printf_header);
  #if 1  //tonyh test
  #else
  fflush(stdout);
  #endif 

  dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_D_loadIMEM()\n", printf_header);
#endif 

}
/** @} */
