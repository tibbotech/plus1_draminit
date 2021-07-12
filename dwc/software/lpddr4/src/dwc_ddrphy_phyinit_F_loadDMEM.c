/** \file
 *  \addtogroup SrcFunc
 *  @{
 */
#include <stdlib.h>
#include <stdio.h>
#include "dwc_ddrphy_phyinit.h"

/** \brief This function loads the training firmware DMEM image and write the 
 *  Message Block parameters for the training firmware into the SRAM.
 * 
 *  This function performs the following tasks:
 *
 *  -# Load the firmware DMEM segment to initialize the data structures from the
 *  DMEM incv file provided in the training firmware package. 
 *  -# Write the Firmware Message Block with the required contents detailing the training parameters.
 *
 * \return void
 */

void dwc_ddrphy_phyinit_Write_DMEM( void) 
{
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54000,0x600);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54001,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54002,0x100);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54003,0xc80);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54004,0x2);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54005,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54006,0x14);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54007,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54008,0x1);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54009,0xc8);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5400a,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5400b,0x2);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5400c,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5400d,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5400e,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5400f,0x100);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54010,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54011,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54012,0x310);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54013,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54014,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54015,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54016,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54017,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54018,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54019,0x2d54);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5401a,0x33);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5401b,0x4d64);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5401c,0x4f28);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5401d,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5401e,0x4);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5401f,0x2d54);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54020,0x33);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54021,0x4d64);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54022,0x4f28);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54023,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54024,0x4);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54025,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54026,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54027,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54028,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54029,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5402a,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5402b,0x1000);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5402c,0x3);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5402d,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5402e,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5402f,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54030,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54031,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54032,0x5400);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54033,0x332d);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54034,0x6400);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54035,0x284d);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54036,0x4f);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54037,0x400);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54038,0x5400);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x54039,0x332d);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5403a,0x6400);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5403b,0x284d);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5403c,0x4f);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5403d,0x400);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5403e,0x0);
	dwc_ddrphy_phyinit_userCustom_io_write16(0x5403f,0x0);
}


void dwc_ddrphy_phyinit_F_loadDMEM (int pstate, int Train2D) 
{
    
    char *printf_header;
    if (Train2D)
        printf_header = "// [phyinit_F_loadDMEM, 2D]";
    else
        printf_header = "// [phyinit_F_loadDMEM, 1D]";

    //dwc_ddrphy_phyinit_cmnt ("%s Start of dwc_ddrphy_phyinit_F_loadDMEM (pstate=%d, Train2D=%d)\n", printf_header, pstate, Train2D);

    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("//\n");
    //dwc_ddrphy_phyinit_cmnt ("// (F) Load the %dD DMEM image and write the %dD Message Block parameters for the training firmware \n", Train2D+1, Train2D+1);
    //dwc_ddrphy_phyinit_cmnt ("// \n");
    //dwc_ddrphy_phyinit_cmnt ("// See PhyInit App Note for detailed description and function usage\n");
    //dwc_ddrphy_phyinit_cmnt ("// \n");
    //dwc_ddrphy_phyinit_cmnt ("//##############################################################\n");
    //dwc_ddrphy_phyinit_cmnt ("\n");

    // set a pointer to the message block.
    PMU_SMB_LPDDR4_1D_t *msgBlkPtr;
    if (Train2D == 1) {
      msgBlkPtr = (PMU_SMB_LPDDR4_1D_t *)&mb_LPDDR4_2D[pstate];
    } else {
      msgBlkPtr = &mb_LPDDR4_1D[pstate];
    }   
    // Some basic checks on MessgeBlock
    if ( msgBlkPtr->EnabledDQsChA %16 != 0 || msgBlkPtr->EnabledDQsChB %16 != 0 )
      {
      dwc_ddrphy_phyinit_assert(0,"%s Lp3/Lp4 - Number of Dq's Enabled per Channel much be multipe of 16\n",printf_header);
      }
    if (( msgBlkPtr->EnabledDQsChA > 8*(userInputBasic.NumActiveDbyteDfi0)) || 
        ( msgBlkPtr->EnabledDQsChB > 8*(userInputBasic.NumActiveDbyteDfi1)) || 
        ( msgBlkPtr->EnabledDQsChA == 0 && msgBlkPtr->EnabledDQsChB == 0) 
       )
      {
      dwc_ddrphy_phyinit_assert(0,"%s EnabledDqsChA/B are not set correctly./1\n",printf_header);
      }
  #if 0
  int addr, mem_offset=0, sizeOfMsgBlk;
  int mem[DMEM_SIZE];
  return_offset_lastaddr_t return_type = return_lastaddr;    

  // initialize the dmem structure
  for (addr=0;addr<DMEM_SIZE;addr++) mem[addr]=0;

  if (Train2D == 1) { 	// 2-D
    mem_offset = dwc_ddrphy_phyinit_storeIncvFile (DMEM_INCV_FILENAME_2D, mem, return_type);
    sizeOfMsgBlk = sizeof(mb_LPDDR4_2D[pstate]);
    dwc_ddrphy_phyinit_storeMsgBlk(&(mb_LPDDR4_2D[pstate]), sizeOfMsgBlk, mem);
    }
  else
    { 								// 1-D
    mem_offset = dwc_ddrphy_phyinit_storeIncvFile (DMEM_INCV_FILENAME, mem, return_type);
    sizeOfMsgBlk = sizeof(mb_LPDDR4_1D[pstate]);
    dwc_ddrphy_phyinit_storeMsgBlk(&(mb_LPDDR4_1D[pstate]), sizeOfMsgBlk, mem);
    }

  // Write local dmem array
  if(0==(mem_offset%1)){//Always write an even number of words so no 32bit quantity is uninitialized                                               
    mem_offset++;
  }
  dwc_ddrphy_phyinit_WriteOutMem(mem, DMEM_ST_ADDR, (mem_offset-DMEM_ST_ADDR));
  #else
  dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x0);
  dwc_ddrphy_phyinit_F_loadDMEM_of_SP(pstate, Train2D); 
  dwc_ddrphy_phyinit_Write_DMEM();
  dwc_ddrphy_phyinit_userCustom_io_write16((tAPBONLY | csr_MicroContMuxSel_ADDR), 0x1);
  #endif 
  //dwc_ddrphy_phyinit_cmnt("%s End of dwc_ddrphy_phyinit_F_loadDMEM()\n", printf_header);
  
}
/** @} */
