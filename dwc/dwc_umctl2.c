#include <types.h>
#include <common.h>
#include <config.h>
#include <dwc_dram_param.h>
#include <dwc_ddrphy_phyinit.h>

struct umctl2_regs {
	unsigned int umctl2_reg[1024];	/* change the size here, (area >> 2) */
};
static volatile struct umctl2_regs *umctl2_reg_ptr = (volatile struct umctl2_regs *)(UMCTL2_REG_Base);
#define UMCTL2_REG(OFFSET)		(umctl2_reg_ptr->umctl2_reg[OFFSET >> 2])

void ctl_apb_wr(UINT32 adr, UINT32 dat)
{
	UMCTL2_REG(adr) = dat;
}
int ctl_apb_rd(UINT32 adr)
{
	UINT32 value;
	value = UMCTL2_REG(adr);
	return value;
}

int dwc_umctl2_init_before_ctl_rst(void)
{
#ifdef DRAM_TYPE_LPDDR4
#ifdef SDRAM_SPEED_1600
#ifdef PLATFORM_SP7350
	prn_string("dwc_umctl2_lpddr4_1600_SP7350_for_zebu\n");
	#include <SP7350/dwc_umctl2_lpddr4_1600_SP7350_for_zebu.txt>
	//prn_string("dwc_umctl2_lpddr4_1600_SP7350_for_realchip\n");
	//#include <SP7350/dwc_umctl2_lpddr4_1600_SP7350_for_realchip.txt>
#endif
#ifdef PLATFORM_Q645
	prn_string("dwc_umctl2_lpddr4_1600_Q645\n");
	#include <Q645/dwc_umctl2_lpddr4_1600_Q645.txt>
#endif
#endif

#ifdef SDRAM_SPEED_1200
#ifdef PLATFORM_SP7350
	prn_string("dwc_umctl2_lpddr4_1200_SP7350\n");
	#include <SP7350/dwc_umctl2_lpddr4_1200_SP7350.txt>
#endif
#ifdef PLATFORM_Q645
	prn_string("dwc_umctl2_lpddr4_1200_Q645\n");
	#include <Q645/dwc_umctl2_lpddr4_1200_Q645.txt>
#endif
#endif

#ifdef SDRAM_SPEED_1066
#ifdef PLATFORM_SP7350
	prn_string("dwc_umctl2_lpddr4_1066_SP7350\n");
	#include <SP7350/dwc_umctl2_lpddr4_1066_SP7350.txt>
#endif
#endif

#ifdef SDRAM_SPEED_933
#ifdef PLATFORM_SP7350
	prn_string("dwc_umctl2_lpddr4_933_SP7350\n");
	#include <SP7350/dwc_umctl2_lpddr4_933_SP7350.txt>
#endif
#endif

#ifdef SDRAM_SPEED_800
#ifdef PLATFORM_SP7350
	prn_string("dwc_umctl2_lpddr4_800_SP7350\n");
	#include <SP7350/dwc_umctl2_lpddr4_800_SP7350.txt>
#endif
#ifdef PLATFORM_Q645
	prn_string("dwc_umctl2_lpddr4_800_Q645\n");
	#include <Q645/dwc_umctl2_lpddr4_800_Q645.txt>
#endif
#endif

#ifdef SDRAM_SPEED_733
#ifdef PLATFORM_SP7350
	prn_string("dwc_umctl2_lpddr4_733_SP7350\n");
	#include <SP7350/dwc_umctl2_lpddr4_733_SP7350.txt>
#endif
#endif

#ifdef SDRAM_SPEED_666
#ifdef PLATFORM_Q645
	prn_string("dwc_umctl2_lpddr4_666_Q645\n");
	#include <Q645/dwc_umctl2_lpddr4_666_Q645.txt>
#endif
#endif

#ifdef SDRAM_SPEED_533
#ifdef PLATFORM_SP7350
	prn_string("dwc_umctl2_lpddr4_533_SP7350\n");
	#include <SP7350/dwc_umctl2_lpddr4_533_SP7350.txt>
#endif
#endif
#endif

#ifdef DRAM_TYPE_DDR4
	prn_string("dwc_umctl2_ddr4_SP7350\n");
	#include <SP7350/dwc_umctl2_ddr4_SP7350.txt>
#endif

#ifdef DRAM_TYPE_DDR3
	prn_string("dwc_umctl2_ddr3_SP7350\n");
	#include <SP7350/dwc_umctl2_ddr3_SP7350.txt>
#endif

#ifdef NT6AN1024F32AV
	prn_string("NT6AN1024F32AV\n");
#elif defined(MT53D1024M32D4)
	prn_string("MT53D1024M32D4, 2rank, FBGA=D9WHT\n");
#elif defined(MT53E1G32D2_A)
	prn_string("MT53E1G32D2_A, 1rank, FBGA=D9ZQX\n");
#elif defined(MT53E1G32D2_B)
	prn_string("MT53E1G32D2_B, 2rank, FBGA=D8CJG\n");
#endif
	return 0;
}


int dwc_umctl2_init_after_ctl_rst(void)
{
#ifdef SDRAM_SPEED_1600
	prn_string("SDRAM_SPEED_1600\n");
	#ifdef PLATFORM_SP7350
	ctl_apb_wr(0x0304,UMCTL2_304(UMCTL2_304_3));
	ctl_apb_rd(0x0030);//PWRCTL
	ctl_apb_wr(0x0030,UMCTL2_30(UMCTL2_30_3));//PWRCTL
	ctl_apb_rd(0x0030);//PWRCTL
	ctl_apb_wr(0x0030,UMCTL2_30(UMCTL2_30_3)); //PWRCTL
	ctl_apb_rd(0x01c4);//DFIPHYMSTR
	ctl_apb_wr(0x01c4,UMCTL2_1C4(UMCTL2_1C4_2));//DFIPHYMSTR
	ctl_apb_wr(0x0320,UMCTL2_320(UMCTL2_304_2));//SWCTL
	ctl_apb_wr(0x01b0,UMCTL2_1B0(UMCTL2_1B0_2));//DFIMISC
	ctl_apb_wr(0x01b0,UMCTL2_1B0(UMCTL2_1B0_2));//DFIMISC
	ctl_apb_wr(0x0304,UMCTL2_304(UMCTL2_304_4));
	#else
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_rd(0x1c4);
	ctl_apb_wr(0x1c4, 0x00000000);
	ctl_apb_wr(0x320, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000040);
	ctl_apb_wr(0x1b0, 0x00000040);
	ctl_apb_wr(0x304, 0x00000002);
	#endif
	ctl_apb_rd(0x00d0);//INIT0
	ctl_apb_rd(0x01c0);//DBICTL 'h00000006
	ctl_apb_rd(0x0000);//MSTR 'h80080020
	ctl_apb_rd(0x0000);//MSTR 'h80080020
	ctl_apb_rd(0x00dc);//INIT3 'h0054002d
	ctl_apb_rd(0x00dc);//INIT3 'h0054002d
	ctl_apb_rd(0x00e0);//INIT4 'h00f10028
	ctl_apb_rd(0x00e8);//INIT6 'h0004004d
	ctl_apb_rd(0x00e8);//INIT6 'h0004004d
	ctl_apb_rd(0x00e0);//INIT4 'h00f10028
	ctl_apb_rd(0x00ec);//INIT7 'h0000004d
	ctl_apb_rd(0x00ec);//INIT7 'h0000004d
	ctl_apb_rd(0x00d0);//INIT0 'hc0020003
	ctl_apb_rd(0x01c0);//DBICTL 'h00000006
	ctl_apb_rd(0x0000);//MSTR 'h80080020
	ctl_apb_rd(0x0000);//MSTR 'h80080020
	ctl_apb_rd(0x00dc);//INIT3 'h0054002d
	ctl_apb_rd(0x00dc);//INIT3 'h0054002d
	ctl_apb_rd(0x00e0);//INIT4 'h00f10028
	ctl_apb_rd(0x00e8);//INIT6 'h0004004d
	ctl_apb_rd(0x00e8);//INIT6 'h0004004d
	ctl_apb_rd(0x00e0);//INIT4 'h00f10028
	ctl_apb_rd(0x00ec);//INIT7 'h0000004d
	ctl_apb_rd(0x00ec);//INIT7 'h0000004d
	ctl_apb_rd(0x00d0);//INIT0 'hc0020003
#endif

#ifdef SDRAM_SPEED_1200
	prn_string("SDRAM_SPEED_1200\n");
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_rd(0x1c4);
	ctl_apb_wr(0x1c4, 0xcf000000);
	ctl_apb_wr(0x320, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000040);
	ctl_apb_wr(0x1b0, 0x00000040);
	ctl_apb_wr(0x304, 0x00000002);
	ctl_apb_rd(0x0d0);
	ctl_apb_rd(0x1c0);
	ctl_apb_rd(0x000);
	ctl_apb_rd(0x0dc);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0e8);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0ec);
	ctl_apb_rd(0x0d0);
	ctl_apb_rd(0x1c0);
	ctl_apb_rd(0x000);
	ctl_apb_rd(0x0dc);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0e8);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0ec);
	ctl_apb_rd(0x0d0);
#endif

#ifdef SDRAM_SPEED_800
	prn_string("SDRAM_SPEED_800\n");
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_rd(0x1c4);
	ctl_apb_wr(0x1c4, 0xcf000000);
	ctl_apb_wr(0x320, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000040);
	ctl_apb_wr(0x1b0, 0x00000040);
	ctl_apb_wr(0x304, 0x00000002);
	ctl_apb_rd(0x0d0);
	ctl_apb_rd(0x1c0);
	ctl_apb_rd(0x000);
	ctl_apb_rd(0x0dc);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0e8);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0ec);
	ctl_apb_rd(0x0d0);
	ctl_apb_rd(0x1c0);
	ctl_apb_rd(0x000);
	ctl_apb_rd(0x0dc);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0e8);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0ec);
	ctl_apb_rd(0x0d0);
#endif

#ifdef SDRAM_SPEED_666
	prn_string("SDRAM_SPEED_666\n");
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_rd(0x1c4);
	ctl_apb_wr(0x1c4, 0x00000000);
	ctl_apb_wr(0x320, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000040);
	ctl_apb_wr(0x1b0, 0x00000040);
	ctl_apb_wr(0x304, 0x00000002);
	ctl_apb_rd(0x0d0);
	ctl_apb_rd(0x1c0);
	ctl_apb_rd(0x000);
	ctl_apb_rd(0x0dc);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0e8);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0ec);
	ctl_apb_rd(0x0d0);
	ctl_apb_rd(0x1c0);
	ctl_apb_rd(0x000);
	ctl_apb_rd(0x0dc);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0e8);
	ctl_apb_rd(0x0e0);
	ctl_apb_rd(0x0ec);
	ctl_apb_rd(0x0d0);
#endif
	return 0;
}

void polling_sw_cfg_done()
{
	UINT16 rd_data;

	while (1) {
		rd_data = ctl_apb_rd(0x324);
		if (rd_data == 1) {
			prn_string("Register programming done!!!\n");
			break;
		}
	}
}

void dwc_ddrphy_phyinit_userCustom_G_waitFwDone_of_SP()
{
	#ifdef STREAM_MESSAGE
 	UINT32 value;
	#endif
	UINT16 rd_data, args, i, low_16bit, upper_16bit;
	UINT8 train_test = 0;
    mp = 0;
	prn_string("Start to wait for the training firmware to complete v.00 !!!\n");
	while (train_test == 0) {
		while (1) {
			rd_data = dwc_ddrphy_apb_rd(0xd0004);/* When set to 0, the PMU has a message for the user */
			if ((rd_data & 0x01) == 0) {
				break;
			}
		}

		low_16bit = dwc_ddrphy_apb_rd(0xd0032);/* Used to pass the lower 16 bits for streaming messages. */
		upper_16bit = dwc_ddrphy_apb_rd(0xd0034);/* Used to pass the upper 16 bits for streaming messages. */

		dwc_ddrphy_apb_wr(0xd0031,0);
		while(1)
		{
		   rd_data = dwc_ddrphy_apb_rd(0xd0004);
		   if((rd_data & 0x01) == 1)
			   break;
		}
		dwc_ddrphy_apb_wr(0xd0031,1);

		rd_data = (upper_16bit << 16) | low_16bit;
		#ifdef STREAM_MESSAGE
		prn_string("major =");prn_dword0(rd_data);
		prn_string("\n");
		#endif
		switch (rd_data & 0xffff) {
			case 0x00:
				//train_test = 1;
				prn_string("End of initialization.\n");
				break;
			case 0x01:
				prn_string("End of fine write leveling.\n");
				break;
			case 0x02:
				prn_string("End of read enable training.\n");
				break;
			case 0x03:
				prn_string("End of read delay center optimization.\n");
				break;
			case 0x04:
				prn_string("End of write delay center optimization.\n");
				break;
			case 0x05:
				prn_string("End of 2D read delay/voltage center optimization.\n");
				break;
			case 0x06:
				prn_string("End of 2D write delay/voltage center optimization.\n");
				break;
			case 0x07:
				train_test = 1;
				prn_string("Training has run successfully.(firmware complete)\n");
				mp = 1;
				break;
			case 0x08:
				#ifdef STREAM_MESSAGE
				prn_string("Start streaming message mode.\n");
				#endif
				while(1)
                {
					rd_data = dwc_ddrphy_apb_rd(0xd0004);
					if((rd_data & 0x01) == 0)
						break;
				}

				low_16bit = dwc_ddrphy_apb_rd(0xd0032);/* Used to pass the lower 16 bits for streaming messages. */
				upper_16bit = dwc_ddrphy_apb_rd(0xd0034);/* Used to pass the upper 16 bits for streaming messages. */

				dwc_ddrphy_apb_wr(0xd0031,0);
				while(1)
				{
				   rd_data = dwc_ddrphy_apb_rd(0xd0004);
				   if((rd_data & 0x01) == 1)
					   break;
				}
				dwc_ddrphy_apb_wr(0xd0031,1);

				#ifdef STREAM_MESSAGE
				value = (upper_16bit << 16) | low_16bit;
				prn_string("string_index=");prn_dword0(value);
				prn_string("\n");
				#endif
				args = low_16bit & 0xffff;
				for(i = 0; i < args; i++) {
					while(1)
	                {
						rd_data = dwc_ddrphy_apb_rd(0xd0004);
						if((rd_data & 0x01) == 0)
							break;
					}
					low_16bit = dwc_ddrphy_apb_rd(0xd0032);/* Used to pass the lower 16 bits for streaming messages. */
					upper_16bit = dwc_ddrphy_apb_rd(0xd0034);/* Used to pass the upper 16 bits for streaming messages. */

					dwc_ddrphy_apb_wr(0xd0031,0);
					while(1)
					{
					   rd_data = dwc_ddrphy_apb_rd(0xd0004);
					   if((rd_data & 0x01) == 1)
						   break;
					}
					dwc_ddrphy_apb_wr(0xd0031,1);
					#ifdef STREAM_MESSAGE
					value = (upper_16bit << 16) | low_16bit;
					prn_string("args=");prn_dword0(value);
					prn_string("\n");
					#endif
				}
				break;
			case 0x09:
				prn_string("End of max read latency training.\n");
				break;
			case 0x0a:
				prn_string("End of read dq deskew training.\n");
				break;
			case 0x0b:
				prn_string("Reserved.\n");
				break;
			case 0x0c:
				prn_string("End of all DB training(MREP/DWL/MRD/MWD complete).\n");
				break;
			case 0x0d:
				prn_string("End of CA training.\n");
				break;
			case 0xfd:
				prn_string("End of MPR read delay center optimization.\n");
				break;
			case 0xfe:
				prn_string("End of Wrtie leveling coarse delay.\n");
				break;
			case 0xff:
				prn_string("Training has failed.(firmware complete)\n");
				while(1);
				return;
		}
	}
}

void run_customPostTrain()
{
	UINT16 rd_data;

	dwc_ddrphy_apb_wr(0xd0000,0);
	rd_data = dwc_ddrphy_apb_rd(0x20010);
	prn_string("32'h20010 rd_data = ");prn_dword0(rd_data);
	prn_string("\n");
	dwc_ddrphy_apb_wr(0x20010,0x6a);
	dwc_ddrphy_apb_wr(0x20010,0x6a);
	rd_data = dwc_ddrphy_apb_rd(0x2001d);
	prn_string("32'h2001d rd_data = ");prn_dword0(rd_data);
	prn_string("\n");
	dwc_ddrphy_apb_wr(0x2001d,0x01);
	rd_data = dwc_ddrphy_apb_rd(0x20097);
	prn_string("32'h20097 rd_data = ");prn_dword0(rd_data);
	prn_string("\n");
}

int ctl_trigger_init_and_wait_normal()
{
	UINT16 rd_data;

	ctl_apb_wr(0x1b0,0x00000070);   //trigger dfi_init_start
	//prn_string("Start to wait for dfi_init_complete !!!");
	//prn_string("\n");
	while (1) {
		rd_data = ctl_apb_rd(0x1bc);
		if (rd_data == 1) {
			//prn_string("Wait for dfi_init_complete, done!!!");
			//prn_string("\n");
			break;
		}
	}
	//->TRIG_VIP_REFRESH_CHECK;

	ctl_apb_wr(0x1b0,0x00000050);
	ctl_apb_wr(0x1b0,0x00000051); //trigger SDRAM initilaztion.
	ctl_apb_wr(0x030,0x00000000); //PWRCTL
	ctl_apb_wr(0x030,0x00000000);
	ctl_apb_wr(0x320,0x00000001); //SWCTL
	//// add w.zhou
	//ctl_apb_wr(12'h014,32'h000002ce);
	////////
	polling_sw_cfg_done();
	//prn_string("12'h324 rd_data = 'h%8h",rd_data);

	//prn_string("Start to wait for ctl into normal mode !!!");
	//prn_string("\n");
	while (1) {
		rd_data = ctl_apb_rd(0x004);
		if (rd_data == 1) {
			//prn_string("Wait for ctl into normal mode, done!!!");
			//prn_string("\n");
			break;
		}
	}

	ctl_apb_wr(0x1c4,0x00000001); //enable the PHY master interface.
	ctl_apb_wr(0x320,0x00000000); //SWCTL
	ctl_apb_wr(0x0d0,0x00030003); //set skip_dram_init to 0, for what???(lj.guo)
	ctl_apb_wr(0x320,0x00000001); //SWCTL
	polling_sw_cfg_done();
	ctl_apb_wr(0x304,0x00000000); //enable dq and hif.
	ctl_apb_wr(0x030,0x00000000);
	ctl_apb_wr(0x030,0x00000000);
	ctl_apb_wr(0x490,0x00000001); //axi port 0 enable.
	return 1;
}
