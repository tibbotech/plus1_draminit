/** \file
 *  \addtogroup SrcFunc
 *  @{
 */
#include <stdlib.h>
#include <stdio.h>
#include "dwc_ddrphy_phyinit.h"

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

void Changing_SDRAMRDQS_of_Zebu_Emulation(void)
{
	dwc_ddrphy_phyinit_userCustom_io_write16(0xd0000,0x0); //DWC_DDRPHYA_APBONLY0_MicroContMuxSel
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10068,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10069,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1006A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1006B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10168,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10169,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1016A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1016B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10268,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10269,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1026A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1026B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10368,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10369,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1036A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1036B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10468,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10469,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1046A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1046B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10568,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10569,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1056A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1056B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10668,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10669,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1066A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1066B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10768,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10769,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1076A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1076B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10868,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x10869,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1086A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1086B,0x006F);

	dwc_ddrphy_phyinit_userCustom_io_write16(0x11068,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11069,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1106A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1106B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11168,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11169,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1116A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1116B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11268,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11269,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1126A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1126B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11368,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11369,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1136A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1136B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11468,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11469,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1146A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1146B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11568,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11569,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1156A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1156B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11668,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11669,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1166A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1166B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11768,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11769,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1176A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1176B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11868,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x11869,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1186A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1186B,0x006F);

	dwc_ddrphy_phyinit_userCustom_io_write16(0x12068,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12069,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1206A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1206B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12168,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12169,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1216A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1216B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12268,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12269,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1226A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1226B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12368,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12369,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1236A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1236B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12468,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12469,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1246A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1246B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12568,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12569,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1256A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1256B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12668,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12669,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1266A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1266B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12768,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12769,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1276A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1276B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12868,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x12869,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1286A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1286B,0x006F);

	dwc_ddrphy_phyinit_userCustom_io_write16(0x13068,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13069,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1306A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1306B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13168,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13169,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1316A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1316B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13268,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13269,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1326A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1326B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13368,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13369,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1336A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1336B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13468,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13469,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1346A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1346B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13568,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13569,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1356A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1356B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13668,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13669,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1366A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1366B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13768,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13769,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1376A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1376B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13868,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x13869,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1386A,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x1386B,0x006F);
	dwc_ddrphy_phyinit_userCustom_io_write16(0xd0000,0x1); //DWC_DDRPHYA_APBONLY0_MicroContMuxSel
}

 
void dwc_ddrphy_phyinit_D_loadIMEM (int Train2D)
{
    char *printf_header;
    if (Train2D)
        printf_header = "// [dwc_ddrphy_phyinit_D_loadIMEM, 2D]";
    else
        printf_header = "// [dwc_ddrphy_phyinit_D_loadIMEM, 1D]";
    
    dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_D_loadIMEM (Train2D=%d)\n", printf_header, Train2D);

    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// (D) Load the %dD IMEM image\n", Train2D+1);
    //dwc_ddrphy_phyinit_cmnt ("// \n");
    //dwc_ddrphy_phyinit_cmnt ("// This function loads the training firmware IMEM image into the SRAM.\n");
    //dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    //dwc_ddrphy_phyinit_cmnt ("// \n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");


  /*
   * Set MemResetL to avoid glitch on BP_MemReset_L during training
   */
  int MemResetL;

  if (Train2D == 0) 
  { // 1-D 
    MemResetL = csr_ProtectMemReset_MASK;
    dwc_ddrphy_phyinit_cmnt ("%s Programming MemResetL to 0x%x\n", printf_header, MemResetL);
    dwc_ddrphy_phyinit_userCustom_io_write16((tMASTER | csr_MemResetL_ADDR), MemResetL);
  }

#if 0
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
  fflush(stdout);
#else
	Changing_SDRAMRDQS_of_Zebu_Emulation();
	dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);
	dwc_ddrphy_phyinit_D_loadIMEM_of_SP(Train2D);
	dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
#endif 

  //dwc_ddrphy_phyinit_cmnt ("%s End of dwc_ddrphy_phyinit_D_loadIMEM()\n", printf_header);
}

/** @} */
