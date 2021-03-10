#include <types.h>
#include <common.h>
#include <config.h>
#include <dwc_dram_param.h>
struct umctl2_regs {
	unsigned int umctl2_reg[1024];	/* change the size here, (area >> 2) */
};
static volatile struct umctl2_regs *umctl2_reg_ptr = (volatile struct umctl2_regs *)(UMCTL2_REG_Base);
#define UMCTL2_REG(OFFSET)		(umctl2_reg_ptr->umctl2_reg[OFFSET >> 2])

void ctl_apb_wr(UINT32 adr, UINT32 dat) {
    //dwc_ddrphy_phyinit_print ("ctl_apb_wr(12'h%x, 32'h%x);\n", adr, dat);
	UMCTL2_REG(adr) = dat;
}
int ctl_apb_rd(UINT32 adr) {
	UINT16 value;
    //dwc_ddrphy_phyinit_print ("ctl_apb_rd(12'h%x, rd_data);\n", adr);
	value = UMCTL2_REG(adr);
	return value;
}

int dwc_umctl2_init_before_ctl_rst(unsigned int dram_id)
{
	//dwc_ddrphy_phyinit_print ("//Start of dwc_umctl2_init_before_ctl_rst\n");
	prn_string ("//Start of dwc_umctl2_init_before_ctl_rst\n");
	//RESET:<aresetn> for Port 0 ASSERTED (ACTIVE LOW)
	//RESET:<core_ddrc_rstn> ASSERTED (ACTIVE LOW)
	//RESET:<presetn> ASSERTED (ACTIVE LOW)
	//RESET:<presetn> DEASSERTED
#if 1
	ctl_apb_wr(0x0304,UMCTL2_304(UMCTL2_304_5));
	ctl_apb_wr(0x0030,UMCTL2_30(UMCTL2_30_1));//PWRCTL
	ctl_apb_rd(0x0004);//STAT
	ctl_apb_wr(0x0000,UMCTL2_0);//MSTR
	ctl_apb_wr(0x0010,UMCTL2_10);//MRCTRL0
	ctl_apb_wr(0x0014,UMCTL2_14);//MRCTRL1
	ctl_apb_wr(0x001c,UMCTL2_1C);//MRCTRL2
	ctl_apb_wr(0x0020,UMCTL2_20);//DERATEEN
	ctl_apb_wr(0x0024,UMCTL2_24);//DERATEINT
	ctl_apb_wr(0x002c,UMCTL2_2C);//DERATECTL
	ctl_apb_wr(0x0030,UMCTL2_30(UMCTL2_30_2));//PWRCTL
	ctl_apb_wr(0x0034,UMCTL2_34);//PWRTMG
	ctl_apb_wr(0x0038,UMCTL2_38);//HWLPCTL
	ctl_apb_wr(0x0050,UMCTL2_50);//RFSHCTL0
	ctl_apb_wr(0x0054,UMCTL2_54);//RFSHCTL0
	ctl_apb_wr(0x0060,UMCTL2_60);//RFSHCTL3
	ctl_apb_wr(0x0064,UMCTL2_64);//RFSHTMG
	ctl_apb_wr(0x0068,UMCTL2_68);//RFSHTMG1
	ctl_apb_wr(0x00c0,UMCTL2_C0);//CRCPARCTL0
	ctl_apb_wr(0x00c4,UMCTL2_C4(UMCTL2_C4_1));//CRCPARCTL1
	ctl_apb_wr(0x00d0,UMCTL2_D0);//INIT0
	ctl_apb_wr(0x00d4,UMCTL2_D4);//INIT1
	ctl_apb_wr(0x00d8,UMCTL2_D8);//INIT2
	ctl_apb_wr(0x00dc,UMCTL2_DC);//INIT3
	ctl_apb_wr(0x00e0,UMCTL2_E0);//INIT4
	ctl_apb_wr(0x00e4,UMCTL2_E4);//INIT5
	ctl_apb_wr(0x00e8,UMCTL2_E8);//INIT6
	ctl_apb_wr(0x00ec,UMCTL2_EC);//INIT7
	ctl_apb_wr(0x00f0,UMCTL2_F0);//DIMMCTL
	ctl_apb_wr(0x00f4,UMCTL2_F4);//DIMMCTL
	ctl_apb_wr(0x0100,UMCTL2_100);//DRAMTMG0
	ctl_apb_wr(0x0104,UMCTL2_104);//DRAMTMG1
	ctl_apb_wr(0x0108,UMCTL2_108);//DRAMTMG2
	ctl_apb_wr(0x010c,UMCTL2_10C);//DRAMTMG3
	ctl_apb_wr(0x0110,UMCTL2_110);//DRAMTMG4
	ctl_apb_wr(0x0114,UMCTL2_114);//DRAMTMG5
	ctl_apb_wr(0x0118,UMCTL2_118);//DRAMTMG6
	ctl_apb_wr(0x011c,UMCTL2_11C);//DRAMTMG7
	ctl_apb_wr(0x0120,UMCTL2_120);//DRAMTMG8
	ctl_apb_wr(0x0124,UMCTL2_124);//DRAMTMG9
	ctl_apb_wr(0x0128,UMCTL2_128);//DRAMTMG10
	ctl_apb_wr(0x012c,UMCTL2_12C);//DRAMTMG11
	ctl_apb_wr(0x0130,UMCTL2_130);//DRAMTMG12
	ctl_apb_wr(0x0134,UMCTL2_134);//DRAMTMG13
	ctl_apb_wr(0x0138,UMCTL2_138);//DRAMTMG14
	ctl_apb_wr(0x013c,UMCTL2_13C);//DRAMTMG15
	ctl_apb_wr(0x0180,UMCTL2_180);//ZQCTL0
	ctl_apb_wr(0x0184,UMCTL2_184);//ZQCTL1
	ctl_apb_wr(0x0188,UMCTL2_188);//ZQCTL2
	ctl_apb_wr(0x0190,UMCTL2_190);//DFITMG0
	ctl_apb_wr(0x0194,UMCTL2_194);//DFITMG1
	ctl_apb_wr(0x0198,UMCTL2_198);//DFILPCFG0
	ctl_apb_wr(0x019c,UMCTL2_19C);//DFILPCFG1
	ctl_apb_wr(0x01a0,UMCTL2_1A0);//DFIUPD0
	ctl_apb_wr(0x01a4,UMCTL2_1A4);//DFIUPD1
	ctl_apb_wr(0x01a8,UMCTL2_1A8);//DFIUPD2
	ctl_apb_wr(0x01b0,UMCTL2_1B0(UMCTL2_1B0_1));//DFIMISC
	ctl_apb_wr(0x01b4,UMCTL2_1B4);//DFITMG2
	ctl_apb_wr(0x01b8,UMCTL2_1B8);//DFITMG3
	ctl_apb_wr(0x01c0,UMCTL2_1C0);//DBICTL
	ctl_apb_wr(0x01c4,UMCTL2_1C4(UMCTL2_1C4_1));//DFIPHYMSTR
	ctl_apb_wr(0x0200,UMCTL2_200);//ADDRMAP1
	ctl_apb_wr(0x0204,UMCTL2_204);//ADDRMAP1
	ctl_apb_wr(0x0208,UMCTL2_208);//ADDRMAP2
	ctl_apb_wr(0x020c,UMCTL2_20C);//ADDRMAP3
	ctl_apb_wr(0x0210,UMCTL2_210);//ADDRMAP4
	ctl_apb_wr(0x0214,UMCTL2_214);//ADDRMAP5
	ctl_apb_wr(0x0218,UMCTL2_218);//ADDRMAP6
	ctl_apb_wr(0x021c,UMCTL2_21C);//ADDRMAP7
	ctl_apb_wr(0x0220,UMCTL2_220);//ADDRMAP8
	ctl_apb_wr(0x0224,UMCTL2_224);//ADDRMAP9
	ctl_apb_wr(0x0228,UMCTL2_228);//ADDRMAP10
	ctl_apb_wr(0x022c,UMCTL2_22C);//ADDRMAP11
	ctl_apb_wr(0x0240,UMCTL2_240);//ODTCFG
	ctl_apb_wr(0x0244,UMCTL2_244);//ODTMAP
	ctl_apb_wr(0x0250,UMCTL2_250);//SCHED
	ctl_apb_wr(0x0254,UMCTL2_254);//SCHED1
	ctl_apb_wr(0x025c,UMCTL2_25C);//PERFHPR1
	ctl_apb_wr(0x0264,UMCTL2_264);//PERFLPR1
	ctl_apb_wr(0x026c,UMCTL2_26C);//PERFWR1
	ctl_apb_wr(0x0300,UMCTL2_300);//DBG0
	ctl_apb_wr(0x0304,UMCTL2_304(UMCTL2_304_3));//DBG1
	ctl_apb_wr(0x030c,UMCTL2_30C);//DBGCMD
	ctl_apb_wr(0x0320,UMCTL2_320(UMCTL2_320_1));//SWCTL
	ctl_apb_wr(0x0328,UMCTL2_328);//SWCTLSTATIC
	ctl_apb_wr(0x036c,UMCTL2_36C);//POISONCFG
	ctl_apb_wr(0x0400,UMCTL2_400);//PCCFG
	ctl_apb_wr(0x0404,UMCTL2_404);//PCFGR_0
	ctl_apb_wr(0x0408,UMCTL2_408(UMCTL2_408_4));//PCFGW_0
	ctl_apb_wr(0x0490,UMCTL2_490);//PCTRL_0
	ctl_apb_wr(0x0494,UMCTL2_494);//PCFGQOS0_0
	ctl_apb_wr(0x0498,UMCTL2_498);//PCFGQOS1_0
	ctl_apb_wr(0x049c,UMCTL2_49C);//PCFGWQOS0_0
	ctl_apb_wr(0x04a0,UMCTL2_4A0);//PCFGWQOS1_0
	ctl_apb_rd(0x0060);
	ctl_apb_rd(0x0030);//PWRCTL
	ctl_apb_wr(0x0030,UMCTL2_30(UMCTL2_30_2));//PWRCTL
	//RESET:<aresetn> for Port 0 DEASSERTED
	//RESET:<core_ddrc_rstn> DEASSERTED
#endif

#if 0
	prn_string("RESET:<aresetn> for Port 0 ASSERTED (ACTIVE LOW)");
	prn_string("\n");
	prn_string("RESET:<core_ddrc_rstn> ASSERTED (ACTIVE LOW)");
	prn_string("\n");
	prn_string("RESET:<presetn> ASSERTED (ACTIVE LOW)");
	prn_string("\n");
	prn_string("RESET:<presetn> DEASSERTED");
	prn_string("\n");
	prn_string("0x0304=");prn_dword0(UMCTL2_304(UMCTL2_304_1));
    prn_string("\n");
	prn_string("0x0030=");prn_dword0(UMCTL2_30(UMCTL2_30_1));
    prn_string("\n");
	//UMCTL2_REG(0x0004);
	prn_string("0x0000=");prn_dword0(UMCTL2_0);
    prn_string("\n");
	prn_string("0x0010=");prn_dword0(UMCTL2_10);
    prn_string("\n");
	prn_string("0x0014=");prn_dword0(UMCTL2_14);
    prn_string("\n");
	prn_string("0x001c=");prn_dword0(UMCTL2_1C);
    prn_string("\n");
	prn_string("0x0020=");prn_dword0(UMCTL2_20);
    prn_string("\n");
	prn_string("0x0024=");prn_dword0(UMCTL2_24);
    prn_string("\n");
	prn_string("0x002C=");prn_dword0(UMCTL2_2C);
    prn_string("\n");
	prn_string("0x0030=");prn_dword0(UMCTL2_30(UMCTL2_30_2));
    prn_string("\n");
	prn_string("0x0034=");prn_dword0(UMCTL2_34);
    prn_string("\n");
	prn_string("0x0038=");prn_dword0(UMCTL2_38);
    prn_string("\n");
	prn_string("0x0050=");prn_dword0(UMCTL2_50);
    prn_string("\n");
	prn_string("0x0054=");prn_dword0(UMCTL2_54);
    prn_string("\n");
	prn_string("0x0060=");prn_dword0(UMCTL2_60);
    prn_string("\n");
	prn_string("0x0064=");prn_dword0(UMCTL2_64);
    prn_string("\n");
	prn_string("0x0068=");prn_dword0(UMCTL2_68);
    prn_string("\n");
	prn_string("0x00C0=");prn_dword0(UMCTL2_C0);
    prn_string("\n");
	prn_string("0x00C4=");prn_dword0(UMCTL2_C4(UMCTL2_C4_1));
    prn_string("\n");
	prn_string("0x00D0=");prn_dword0(UMCTL2_D0);
    prn_string("\n");
	prn_string("0x00D4=");prn_dword0(UMCTL2_D4);
    prn_string("\n");
	prn_string("0x00D8=");prn_dword0(UMCTL2_D8);
    prn_string("\n");
	prn_string("0x00DC=");prn_dword0(UMCTL2_DC);
    prn_string("\n");
	prn_string("0x00E0=");prn_dword0(UMCTL2_E0);
    prn_string("\n");
	prn_string("0x00E4=");prn_dword0(UMCTL2_E4);
    prn_string("\n");
	prn_string("0x00E8=");prn_dword0(UMCTL2_E8);
    prn_string("\n");
	prn_string("0x00EC=");prn_dword0(UMCTL2_EC);
    prn_string("\n");
	prn_string("0x00F0=");prn_dword0(UMCTL2_F0);
    prn_string("\n");
	prn_string("0x00F4=");prn_dword0(UMCTL2_F4);
    prn_string("\n");
	prn_string("0x0100=");prn_dword0(UMCTL2_100);
    prn_string("\n");
	prn_string("0x0104=");prn_dword0(UMCTL2_104);
    prn_string("\n");
	prn_string("0x0108=");prn_dword0(UMCTL2_108);
    prn_string("\n");
	prn_string("0x010C=");prn_dword0(UMCTL2_10C);
    prn_string("\n");
	prn_string("0x0110=");prn_dword0(UMCTL2_110);
    prn_string("\n");
	prn_string("0x0114=");prn_dword0(UMCTL2_114);
    prn_string("\n");
	prn_string("0x0118=");prn_dword0(UMCTL2_118);
    prn_string("\n");
	prn_string("0x011C=");prn_dword0(UMCTL2_11C);
    prn_string("\n");
	prn_string("0x0120=");prn_dword0(UMCTL2_120);
    prn_string("\n");
	prn_string("0x0124=");prn_dword0(UMCTL2_124);
    prn_string("\n");
	prn_string("0x0128=");prn_dword0(UMCTL2_128);
    prn_string("\n");
	prn_string("0x012C=");prn_dword0(UMCTL2_12C);
    prn_string("\n");
	prn_string("0x0130=");prn_dword0(UMCTL2_130);
    prn_string("\n");
	prn_string("0x0134=");prn_dword0(UMCTL2_134);
    prn_string("\n");
	prn_string("0x0138=");prn_dword0(UMCTL2_138);
    prn_string("\n");
	prn_string("0x013C=");prn_dword0(UMCTL2_13C);
    prn_string("\n");
	prn_string("0x0180=");prn_dword0(UMCTL2_180);
    prn_string("\n");
	prn_string("0x0184=");prn_dword0(UMCTL2_184);
    prn_string("\n");
	prn_string("0x0188=");prn_dword0(UMCTL2_188);
    prn_string("\n");
	prn_string("0x0190=");prn_dword0(UMCTL2_190);
    prn_string("\n");
	prn_string("0x0194=");prn_dword0(UMCTL2_194);
    prn_string("\n");
	prn_string("0x0198=");prn_dword0(UMCTL2_198);
    prn_string("\n");
	prn_string("0x019C=");prn_dword0(UMCTL2_19C);
    prn_string("\n");
	prn_string("0x01A0=");prn_dword0(UMCTL2_1A0);
    prn_string("\n");
	prn_string("0x01A4=");prn_dword0(UMCTL2_1A4);
    prn_string("\n");
	prn_string("0x01A8=");prn_dword0(UMCTL2_1A8);
    prn_string("\n");
	prn_string("0x01B0=");prn_dword0(UMCTL2_1B0(UMCTL2_1B0_1));
    prn_string("\n");
	prn_string("0x01B4=");prn_dword0(UMCTL2_1B4);
    prn_string("\n");
	prn_string("0x01B8=");prn_dword0(UMCTL2_1B8);
    prn_string("\n");
	prn_string("0x01C0=");prn_dword0(UMCTL2_1C0);
    prn_string("\n");
	prn_string("0x01C4=");prn_dword0(UMCTL2_1C4(UMCTL2_1C4_1));
    prn_string("\n");
	prn_string("0x0200=");prn_dword0(UMCTL2_200);
    prn_string("\n");
	prn_string("0x0204=");prn_dword0(UMCTL2_204);
    prn_string("\n");
	prn_string("0x0208=");prn_dword0(UMCTL2_208);
    prn_string("\n");
	prn_string("0x020C=");prn_dword0(UMCTL2_20C);
    prn_string("\n");
	prn_string("0x0210=");prn_dword0(UMCTL2_210);
    prn_string("\n");
	prn_string("0x0214=");prn_dword0(UMCTL2_214);
    prn_string("\n");
	prn_string("0x0218=");prn_dword0(UMCTL2_218);
    prn_string("\n");
	prn_string("0x021C=");prn_dword0(UMCTL2_21C);
    prn_string("\n");
	prn_string("0x0220=");prn_dword0(UMCTL2_220);
    prn_string("\n");
	prn_string("0x0224=");prn_dword0(UMCTL2_224);
    prn_string("\n");
	prn_string("0x0228=");prn_dword0(UMCTL2_228);
    prn_string("\n");
	prn_string("0x022C=");prn_dword0(UMCTL2_22C);
    prn_string("\n");
	prn_string("0x0240=");prn_dword0(UMCTL2_240);
    prn_string("\n");
	prn_string("0x0244=");prn_dword0(UMCTL2_244);
    prn_string("\n");
	prn_string("0x0250=");prn_dword0(UMCTL2_250);
    prn_string("\n");
	prn_string("0x0254=");prn_dword0(UMCTL2_254);
    prn_string("\n");
	prn_string("0x025C=");prn_dword0(UMCTL2_25C);
    prn_string("\n");
	prn_string("0x0264=");prn_dword0(UMCTL2_264);
    prn_string("\n");
	prn_string("0x026C=");prn_dword0(UMCTL2_26C);
    prn_string("\n");
	prn_string("0x0300=");prn_dword0(UMCTL2_300);
    prn_string("\n");
	prn_string("0x0304=");prn_dword0(UMCTL2_304(UMCTL2_304_2));
    prn_string("\n");
	prn_string("0x030C=");prn_dword0(UMCTL2_30C);
    prn_string("\n");
	prn_string("0x0320=");prn_dword0(UMCTL2_320(UMCTL2_320_1));
    prn_string("\n");
	prn_string("0x0328=");prn_dword0(UMCTL2_328);
    prn_string("\n");
	prn_string("0x036c=");prn_dword0(UMCTL2_36C);
	prn_string("\n");
	prn_string("0x0400=");prn_dword0(UMCTL2_400);
	prn_string("\n");
	prn_string("0x0404=");prn_dword0(UMCTL2_404);
    prn_string("\n");
	prn_string("0x0408=");prn_dword0(UMCTL2_408(UMCTL2_408_4));
    prn_string("\n");
	prn_string("0x0490=");prn_dword0(UMCTL2_490);
	prn_string("\n");
	prn_string("0x0494=");prn_dword0(UMCTL2_494);
	prn_string("\n");
	prn_string("0x0498=");prn_dword0(UMCTL2_498);
	prn_string("\n");
	prn_string("0x049c=");prn_dword0(UMCTL2_49C);
	prn_string("\n");
	prn_string("0x04a0=");prn_dword0(UMCTL2_4A0);
	prn_string("\n");
	prn_string("READ 0x0060");
	prn_string("\n");
	prn_string("READ 0x0030");
	prn_string("\n");
	prn_string("0x0030=");prn_dword0(UMCTL2_30(UMCTL2_30_2));
	prn_string("\n");
	prn_string("RESET:<aresetn> for Port 0 DEASSERTED");
	prn_string("\n");
	prn_string("RESET:<core_ddrc_rstn> DEASSERTED");
	prn_string("\n");
#endif
	//dwc_ddrphy_phyinit_print ("//End of dwc_umctl2_init_before_ctl_rst\n");
	prn_string ("//End of dwc_umctl2_init_before_ctl_rst\n");
	return 0;
}


int dwc_umctl2_init_after_ctl_rst(unsigned int dram_id)
{
	//dwc_ddrphy_phyinit_print ("//Start of dwc_umctl2_init_after_ctl_rst\n");
	prn_string ("//Start of dwc_umctl2_init_after_ctl_rst\n");
#if 1
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

#if 0
	prn_string("0x0304=");prn_dword0(UMCTL2_304(UMCTL2_304_3));
	prn_string("\n");
	prn_string("READ 0x0030");
	prn_string("\n");
	prn_string("0x0030=");prn_dword0(UMCTL2_30(UMCTL2_30_3));
	prn_string("\n");
	prn_string("READ 0x0030");
	prn_string("\n");
	prn_string("0x0030=");prn_dword0(UMCTL2_30(UMCTL2_30_3));
	prn_string("\n");
	prn_string("READ 0x01c4");
	prn_string("\n");
	prn_string("0x01c4=");prn_dword0(UMCTL2_1C4(UMCTL2_1C4_2));
	prn_string("\n");
	prn_string("0x0320=");prn_dword0(UMCTL2_320(UMCTL2_320_2));
	prn_string("\n");
	prn_string("0x01B0=");prn_dword0(UMCTL2_1B0(UMCTL2_1B0_2));
	prn_string("\n");
	prn_string("0x01B0=");prn_dword0(UMCTL2_1B0(UMCTL2_1B0_2));
	prn_string("\n");
	prn_string("0x0304=");prn_dword0(UMCTL2_304(UMCTL2_304_4));
	prn_string("\n");
#endif
	//dwc_ddrphy_phyinit_print ("//End of dwc_umctl2_init_after_ctl_rst\n");
	prn_string ("//End of dwc_umctl2_init_after_ctl_rst\n");
	return 0;
}

void polling_sw_cfg_done(){
    UINT16 rd_data;
	while(1){
		rd_data = ctl_apb_rd(0x324);
		if(rd_data == 1){
			prn_string("Register programming done!!!");
			prn_string("\n");
			break;
		}
	}
}

void run_waitFwDone (){
    UINT16 rd_data;
    UINT8 train_test = 0;
    prn_string("Start to wait for the training firmware to complete!!!");
	prn_string("\n");
    while(train_test==0){
        while(1) {
            rd_data = dwc_ddrphy_apb_rd(0xd0004);
            if(rd_data == 0){
                prn_string("Wait mailbox send message done!!!");
				prn_string("\n");
                break;
            }
        }
        rd_data = dwc_ddrphy_apb_rd(0xd0032);
        if(rd_data == 0x07) {
            train_test = 1;
            prn_string("GET mailbox send 7 ,FW training done!!!!");
			prn_string("\n");
        }
        prn_string("GET mailbox message = ");prn_dword0(rd_data);
		prn_string("\n");
        if(rd_data == 0xff) {
            prn_string("GET mailbox send 16'hff ,FW training Fail!!!!");
			prn_string("\n");
            return;
        }
        rd_data = dwc_ddrphy_apb_rd(0xd0034);
        //dwc_ddrphy_apb_rd(32'hd0031,rd_data);
        dwc_ddrphy_apb_wr(0xd0031,0);
        while(1) {
            rd_data = dwc_ddrphy_apb_rd(0xd0004);
            if(rd_data == 1) {
                break;
            }
        }
        dwc_ddrphy_apb_wr(0xd0031,1);
    }
}

void run_customPostTrain(){
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


void ctl_trigger_init_and_wait_normal(){
	UINT16 rd_data;
    ctl_apb_wr(0x1b0,0x00000070);   //trigger dfi_init_start
    prn_string("Start to wait for dfi_init_complete !!!");
	prn_string("\n");
    while(1){
       rd_data = ctl_apb_rd(0x1bc);
       if(rd_data == 1){
          prn_string("Wait for dfi_init_complete, done!!!");
		  prn_string("\n");
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

    prn_string("Start to wait for ctl into normal mode !!!");
	prn_string("\n");
    while(1){
       rd_data = ctl_apb_rd(0x004);
       if(rd_data == 1){
          prn_string("Wait for ctl into normal mode, done!!!");
		  prn_string("\n");
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
}
