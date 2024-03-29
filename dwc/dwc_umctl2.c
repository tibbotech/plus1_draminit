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
	UINT16 value;
	value = UMCTL2_REG(adr);
	return value;
}

int dwc_umctl2_init_before_ctl_rst(void)
{
#ifdef SDRAM_SPEED_1600
#ifdef PLATFORM_SP7350
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
	ctl_apb_wr(0x00d0,0x00030002);  //UMCTL2_D0);//INIT0
	ctl_apb_wr(0x00d4,0x00010002);  //UMCTL2_D4);//INIT1
	ctl_apb_wr(0x00d8,0x00001a00);  //UMCTL2_D8);//INIT2
	ctl_apb_wr(0x00dc,0x0054002d);  //UMCTL2_DC);//INIT3
	ctl_apb_wr(0x00e0,0x00310000);  //UMCTL2_E0);//INIT4
	ctl_apb_wr(0x00e4,0x00040009);  //UMCTL2_E4);//INIT5
	ctl_apb_wr(0x00e8,0x0000004d);  //UMCTL2_E8);//INIT6
	ctl_apb_wr(0x00ec,0x0000004d);  //UMCTL2_EC);//INIT7
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

//	ctl_apb_wr(0x0200, 0x00000007);//00000018);//00000016); //UMCTL2_200);//ADDRMAP1
//	ctl_apb_wr(0x0204, 0x00080202);//00020202);//00080808); //UMCTL2_204);//ADDRMAP1
//	ctl_apb_wr(0x0208, 0x02020000);//03030000);//00000000); //UMCTL2_208);//ADDRMAP2
//	ctl_apb_wr(0x020c, 0x02020202);//03030303);//00000000); //UMCTL2_20C);//ADDRMAP3
//	ctl_apb_wr(0x0210, 0x00001f1f);//00001F1F);//00001f1f); //UMCTL2_210);//ADDRMAP4
//	ctl_apb_wr(0x0214, 0x080f0808);//070F0707);//070f0707); //UMCTL2_214);//ADDRMAP5
//	ctl_apb_wr(0x0218, 0x08080808);//07070707);//09070707); //UMCTL2_218);//ADDRMAP6
//	ctl_apb_wr(0x021c, 0x00000f08);//00000F07);//00000f07); //UMCTL2_21C);//ADDRMAP7
//	ctl_apb_wr(0x0220, 0x00003f3f);//00003F3F);//00003f3f); //UMCTL2_220);//ADDRMAP8
//	ctl_apb_wr(0x0224, 0x08080808);//07070707);//07070707); //UMCTL2_224);//ADDRMAP9
//	ctl_apb_wr(0x0228, 0x08080808);//07070707);//07070707); //UMCTL2_228);//ADDRMAP10
//	ctl_apb_wr(0x022c, 0x00000008);//001F1F07);//00000007); //UMCTL2_22C);//ADDRMAP11

	ctl_apb_wr(0x0200, 0x00000016);//00000017);//UMCTL2_200);//ADDRMAP1
	ctl_apb_wr(0x0204, 0x00080202);//00080202);//UMCTL2_204);//ADDRMAP1
	ctl_apb_wr(0x0208, 0x02020000);//02020000);//UMCTL2_208);//ADDRMAP2
	ctl_apb_wr(0x020c, 0x02020202);//02020202);//UMCTL2_20C);//ADDRMAP3
	ctl_apb_wr(0x0210, 0x00001F1F);//00001F1F);//UMCTL2_210);//ADDRMAP4
	ctl_apb_wr(0x0214, 0x070F0707);//070F0707);//UMCTL2_214);//ADDRMAP5
	ctl_apb_wr(0x0218, 0x08070707);//07070707);//UMCTL2_218);//ADDRMAP6
	ctl_apb_wr(0x021c, 0x00000F08);//00000F08);//UMCTL2_21C);//ADDRMAP7
	ctl_apb_wr(0x0220, 0x00003F3F);//00003F3F);//UMCTL2_220);//ADDRMAP8
	ctl_apb_wr(0x0224, 0x07070707);//07070707);//UMCTL2_224);//ADDRMAP9
	ctl_apb_wr(0x0228, 0x07070707);//07070707);//UMCTL2_228);//ADDRMAP10
	ctl_apb_wr(0x022c, 0x001F1F07);//001F1F07);//UMCTL2_22C);//ADDRMAP11

	ctl_apb_wr(0x0240,UMCTL2_240);//ODTCFG
	ctl_apb_wr(0x0244,UMCTL2_244);//ODTMAP
	//ctl_apb_wr(0x0250,UMCTL2_250);//SCHED    //mask for test by dz.he 0628
	ctl_apb_wr(0x0250,0x000b1784);//SCHED
	ctl_apb_wr(0x0254,0x000000f0); //UMCTL2_254);//SCHED1
	//ctl_apb_wr(0x025c,UMCTL2_25C);//PERFHPR1  //mask for test by dz.he 0628
	ctl_apb_wr(0x025c,0x610001ff);//PERFHPR1
	//ctl_apb_wr(0x0264,UMCTL2_264);//PERFLPR1  //mask for test by dz.he 0628
	ctl_apb_wr(0x0264,0x420001ff);//PERFLPR1
	ctl_apb_wr(0x026c,UMCTL2_26C);//PERFWR1
	ctl_apb_wr(0x0300,0x00000000);//UMCTL2_300);//DBG0
	ctl_apb_wr(0x0304,UMCTL2_304(UMCTL2_304_3));//DBG1
	ctl_apb_wr(0x030c,UMCTL2_30C);//DBGCMD
	ctl_apb_wr(0x0320,UMCTL2_320(UMCTL2_320_1));//SWCTL
	ctl_apb_wr(0x0328,UMCTL2_328);//SWCTLSTATIC
	ctl_apb_wr(0x036c,UMCTL2_36C);//POISONCFG
	ctl_apb_wr(0x0400,0x00000110);//UMCTL2_400);//PCCFG
	ctl_apb_wr(0x0404,UMCTL2_404);//PCFGR_0
	ctl_apb_wr(0x0408,UMCTL2_408(UMCTL2_408_4));//PCFGW_0
	ctl_apb_wr(0x0490,UMCTL2_490);//PCTRL_0
	ctl_apb_wr(0x540, 0x00000001);
	ctl_apb_wr(0x5f0, 0x00000001);
	ctl_apb_wr(0x6a0, 0x00000001);
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

#ifdef PLATFORM_Q645
	ctl_apb_wr(0x304, 0x00000001);
	ctl_apb_wr(0x030, 0x00000001);
	ctl_apb_rd(0x004);
	ctl_apb_wr(0x000, 0x83080020);
	ctl_apb_wr(0x010, 0x40003030);
	ctl_apb_wr(0x014, 0x00008043);
	ctl_apb_wr(0x01c, 0x040df7d4);
	ctl_apb_wr(0x020, 0x00001202);
	ctl_apb_wr(0x024, 0x0c3a97b5);
	ctl_apb_wr(0x02c, 0x00000000);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_wr(0x034, 0x0040b904);
	ctl_apb_wr(0x038, 0x004e0000);
	ctl_apb_wr(0x050, 0x00210000);
	ctl_apb_wr(0x054, 0x00100042);
	ctl_apb_wr(0x060, 0x00000000);
	ctl_apb_wr(0x064, 0x00618130);
	ctl_apb_wr(0x068, 0x00480000);
	ctl_apb_wr(0x0c0, 0x00000000);
	ctl_apb_wr(0x0c4, 0x00000000);
	ctl_apb_wr(0x0d0, 0xc0020002);
	ctl_apb_wr(0x0d4, 0x00010002);
	ctl_apb_wr(0x0d8, 0x0000e205);
	ctl_apb_wr(0x0dc, 0x0054002d);
	ctl_apb_wr(0x0e0, 0x00310008);
	ctl_apb_wr(0x0e4, 0x00040009);
	ctl_apb_wr(0x0e8, 0x0000004d);
	ctl_apb_wr(0x0ec, 0x0000004d);
	ctl_apb_wr(0x0f0, 0x00000000);
	ctl_apb_wr(0x0f4, 0x0000054f);
	ctl_apb_wr(0x100, 0x1b203622);
	ctl_apb_wr(0x104, 0x00060630);
	ctl_apb_wr(0x108, 0x070e0f14);
	ctl_apb_wr(0x10c, 0x00b0c006);
	ctl_apb_wr(0x110, 0x0f04080f);
	ctl_apb_wr(0x114, 0x02040c0c);
	ctl_apb_wr(0x118, 0x01010007);
	ctl_apb_wr(0x11c, 0x00000402);
	ctl_apb_wr(0x120, 0x01010101);
	ctl_apb_wr(0x124, 0x00000008);
	ctl_apb_wr(0x128, 0x00020a00);
	ctl_apb_wr(0x12c, 0x0101001a);
	ctl_apb_wr(0x130, 0x00020000);
	ctl_apb_wr(0x134, 0x0c100002);
	ctl_apb_wr(0x138, 0x00000136);
	ctl_apb_wr(0x13c, 0x80000000);
	ctl_apb_wr(0x180, 0xd3200018);
	ctl_apb_wr(0x184, 0x028b282b);
	ctl_apb_wr(0x188, 0x00000000);
	ctl_apb_wr(0x190, 0x0397820a);
	ctl_apb_wr(0x194, 0x00090202);
	ctl_apb_wr(0x198, 0x07513011);
	ctl_apb_wr(0x19c, 0x00000071);
	ctl_apb_wr(0x1a0, 0xe0400018);
	ctl_apb_wr(0x1a4, 0x0042009c);
	ctl_apb_wr(0x1a8, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000041);
	ctl_apb_wr(0x1b4, 0x0000170a);
	ctl_apb_wr(0x1b8, 0x00000008);
	ctl_apb_wr(0x1c0, 0x00000000);
	ctl_apb_wr(0x1c4, 0x00000000);
	ctl_apb_wr(0x200, 0x00000017);
	ctl_apb_wr(0x204, 0x00080202);
	ctl_apb_wr(0x208, 0x02020000);
	ctl_apb_wr(0x20c, 0x02020202);
	ctl_apb_wr(0x210, 0x00001F1F);
	ctl_apb_wr(0x214, 0x070F0707);
	ctl_apb_wr(0x218, 0x07070707);
	ctl_apb_wr(0x21c, 0x00000F08);
	ctl_apb_wr(0x220, 0x00003F3F);
	ctl_apb_wr(0x224, 0x07070707);
	ctl_apb_wr(0x228, 0x07070707);
	ctl_apb_wr(0x22c, 0x001F1F07);
	ctl_apb_wr(0x240, 0x06080a48);
	ctl_apb_wr(0x244, 0x00000000);
	ctl_apb_wr(0x250, 0x00dd3f00);
	ctl_apb_wr(0x254, 0x00000000);
	ctl_apb_wr(0x25c, 0x0f000001);
	ctl_apb_wr(0x264, 0x0f00007f);
	ctl_apb_wr(0x26c, 0x0f00007f);
	ctl_apb_wr(0x300, 0x00000000);
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_wr(0x30c, 0x00000000);
	ctl_apb_wr(0x320, 0x00000001);
	ctl_apb_wr(0x328, 0x00000000);
	ctl_apb_wr(0x36c, 0x00100000);
	ctl_apb_wr(0x490, 0x00000001);
	ctl_apb_rd(0x060);
	ctl_apb_wr(0x400, 0x00000000);
	ctl_apb_wr(0x404, 0x0000000f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x408, 0x0000400f);
	ctl_apb_wr(0x408, 0x0000500f);
	ctl_apb_wr(0x408, 0x0000500f);
	ctl_apb_wr(0x408, 0x0000100f);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
#endif
#endif

#ifdef SDRAM_SPEED_1200
	ctl_apb_wr(0x304, 0x00000001);
	ctl_apb_wr(0x030, 0x00000001);
	ctl_apb_rd(0x004);
	ctl_apb_wr(0x000, 0x83080020);
	ctl_apb_wr(0x010, 0x40003030);
	ctl_apb_wr(0x014, 0x00008043);
	ctl_apb_wr(0x01c, 0x040df7d4);
	ctl_apb_wr(0x020, 0x00001202);
	ctl_apb_wr(0x024, 0x0c3a97b5);
	ctl_apb_wr(0x02c, 0x00000000);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_wr(0x034, 0x0040b904);
	ctl_apb_wr(0x038, 0x004e0000);
	ctl_apb_wr(0x050, 0x00210000);
	ctl_apb_wr(0x054, 0x00100042);
	ctl_apb_wr(0x060, 0x00000000);
	ctl_apb_wr(0x064, 0x00618130);
	ctl_apb_wr(0x068, 0x00480000);
	ctl_apb_wr(0x0c0, 0x00000000);
	ctl_apb_wr(0x0c4, 0x00000000);
	ctl_apb_wr(0x0d0, 0xc0020002);
	ctl_apb_wr(0x0d4, 0x00010002);
	ctl_apb_wr(0x0d8, 0x0000e205);
	ctl_apb_wr(0x0dc, 0x0054002d);
	ctl_apb_wr(0x0e0, 0x00310008);
	ctl_apb_wr(0x0e4, 0x00040009);
	ctl_apb_wr(0x0e8, 0x0000004d);
	ctl_apb_wr(0x0ec, 0x0000004d);
	ctl_apb_wr(0x0f0, 0x00000000);
	ctl_apb_wr(0x0f4, 0x0000053f);
	ctl_apb_wr(0x100, 0x1b203622);
	ctl_apb_wr(0x104, 0x00060630);
	ctl_apb_wr(0x108, 0x070e0f14);
	ctl_apb_wr(0x10c, 0x00b0c006);
	ctl_apb_wr(0x110, 0x0f04080f);
	ctl_apb_wr(0x114, 0x02040c0c);
	ctl_apb_wr(0x118, 0x01010007);
	ctl_apb_wr(0x11c, 0x00000402);
	ctl_apb_wr(0x120, 0x01010101);
	ctl_apb_wr(0x124, 0x00000008);
	ctl_apb_wr(0x128, 0x00020a00);
	ctl_apb_wr(0x12c, 0x0101001a);
	ctl_apb_wr(0x130, 0x00020000);
	ctl_apb_wr(0x134, 0x0c100002);
	ctl_apb_wr(0x138, 0x00000136);
	ctl_apb_wr(0x13c, 0x80000000);
	ctl_apb_wr(0x180, 0xd3200018);
	ctl_apb_wr(0x184, 0x028b282b);
	ctl_apb_wr(0x188, 0x00000000);
	ctl_apb_wr(0x190, 0x03938208);
	ctl_apb_wr(0x194, 0x00090202);
	ctl_apb_wr(0x198, 0x07513011);
	ctl_apb_wr(0x19c, 0x00000071);
	ctl_apb_wr(0x1a0, 0xe0400018);
	ctl_apb_wr(0x1a4, 0x0042009c);
	ctl_apb_wr(0x1a8, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000041);
	ctl_apb_wr(0x1b4, 0x00001308);
	ctl_apb_wr(0x1b8, 0x00000008);
	ctl_apb_wr(0x1c0, 0x00000000);
	ctl_apb_wr(0x1c4, 0x00000000);
	ctl_apb_wr(0x200, 0x00000017);
	ctl_apb_wr(0x204, 0x00080808);
	ctl_apb_wr(0x208, 0x00000000);
	ctl_apb_wr(0x20c, 0x00000000);
	ctl_apb_wr(0x210, 0x00001f1f);
	ctl_apb_wr(0x214, 0x070f0707);
	ctl_apb_wr(0x218, 0x07070707);
	ctl_apb_wr(0x21c, 0x00000f08);
	ctl_apb_wr(0x220, 0x00000000);
	ctl_apb_wr(0x224, 0x07070707);
	ctl_apb_wr(0x228, 0x07070707);
	ctl_apb_wr(0x22c, 0x00000007);
	ctl_apb_wr(0x240, 0x06080a48);
	ctl_apb_wr(0x244, 0x00000000);
	ctl_apb_wr(0x250, 0x00dd3f00);
	ctl_apb_wr(0x254, 0x00000000);
	ctl_apb_wr(0x25c, 0x0f000001);
	ctl_apb_wr(0x264, 0x0f00007f);
	ctl_apb_wr(0x26c, 0x0f00007f);
	ctl_apb_wr(0x300, 0x00000000);
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_wr(0x30c, 0x00000000);
	ctl_apb_wr(0x320, 0x00000001);
	ctl_apb_wr(0x328, 0x00000000);
	ctl_apb_wr(0x36c, 0x00100000);
	ctl_apb_wr(0x490, 0x00000001);
#ifdef PLATFORM_SP7350
	ctl_apb_wr(0x0540,0x00000001);//BUS1
	ctl_apb_wr(0x05f0,0x00000001);//BUS2
	ctl_apb_wr(0x06a0,0x00000001);//BUS3
#endif
	ctl_apb_rd(0x060);
	ctl_apb_wr(0x400, 0x00000000);
	ctl_apb_wr(0x404, 0x0000000f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x408, 0x0000400f);
	ctl_apb_wr(0x408, 0x0000500f);
	ctl_apb_wr(0x408, 0x0000500f);
	ctl_apb_wr(0x408, 0x0000100f);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
#endif

#ifdef SDRAM_SPEED_800
	ctl_apb_wr(0x304, 0x00000001);
	ctl_apb_wr(0x030, 0x00000001);
	ctl_apb_rd(0x004);
	ctl_apb_wr(0x000, 0x83080020);
	ctl_apb_wr(0x010, 0x40003030);
	ctl_apb_wr(0x014, 0x00008043);
	ctl_apb_wr(0x01c, 0x9a361bcc);
	ctl_apb_wr(0x020, 0x00000100);
	ctl_apb_wr(0x024, 0x0c3a97b5);
	ctl_apb_wr(0x02c, 0x00000000);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_wr(0x034, 0x00403904);
	ctl_apb_wr(0x038, 0x00330002);
	ctl_apb_wr(0x050, 0x90210001);
	ctl_apb_wr(0x054, 0x00150027);
	ctl_apb_wr(0x060, 0x00000000);
	ctl_apb_wr(0x064, 0x00300098);
	ctl_apb_wr(0x068, 0x00240000);
	ctl_apb_wr(0x0c0, 0x00000000);
	ctl_apb_wr(0x0c4, 0x00000000);
	ctl_apb_wr(0x0d0, 0xc0020002);
	ctl_apb_wr(0x0d4, 0x00010002);
	ctl_apb_wr(0x0d8, 0x00000d00);
	ctl_apb_wr(0x0dc, 0x00240012);
	ctl_apb_wr(0x0e0, 0x00310008);
	ctl_apb_wr(0x0e4, 0x00030005);
	ctl_apb_wr(0x0e8, 0x0000004d);
	ctl_apb_wr(0x0ec, 0x0000004d);
	ctl_apb_wr(0x0f0, 0x00000000);
#ifdef NANYA
	ctl_apb_wr(0x0f4, 0x0000032f);
#elif defined(MICRON)
	ctl_apb_wr(0x0f4, 0x0000053f);
#endif
	ctl_apb_wr(0x100, 0x11100d11);
	ctl_apb_wr(0x104, 0x00030418);
#ifdef NANYA
	ctl_apb_wr(0x108, 0x04070a0d);
#elif defined(MICRON)
	ctl_apb_wr(0x108, 0x060c0e12);
#endif
	ctl_apb_wr(0x10c, 0x00606006);
	ctl_apb_wr(0x110, 0x08040408);
	ctl_apb_wr(0x114, 0x02030606);
	ctl_apb_wr(0x118, 0x01010004);
	ctl_apb_wr(0x11c, 0x00000302);
	ctl_apb_wr(0x120, 0x01010101);
	ctl_apb_wr(0x124, 0x00000008);
	ctl_apb_wr(0x128, 0x00030400);
	ctl_apb_wr(0x12c, 0x0101001d);
	ctl_apb_wr(0x130, 0x00020000);
	ctl_apb_wr(0x134, 0x0a100002);
	ctl_apb_wr(0x138, 0x0000009b);
	ctl_apb_wr(0x13c, 0x80000000);
	ctl_apb_wr(0x180, 0xc190000c);
	ctl_apb_wr(0x184, 0x0143206e);
	ctl_apb_wr(0x188, 0x00000000);
	ctl_apb_wr(0x190, 0x03898204);
	ctl_apb_wr(0x194, 0x00090202);
	ctl_apb_wr(0x198, 0x0771a021);
	ctl_apb_wr(0x19c, 0x00000021);
	ctl_apb_wr(0x1a0, 0x80400018);
	ctl_apb_wr(0x1a4, 0x00b800d1);
	ctl_apb_wr(0x1a8, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000041);
	ctl_apb_wr(0x1b4, 0x00000904);
	ctl_apb_wr(0x1b8, 0x00000008);
	ctl_apb_wr(0x1c0, 0x00000000);
	ctl_apb_wr(0x1c4, 0xcf000000);
	ctl_apb_wr(0x200, 0x00000017);
	ctl_apb_wr(0x204, 0x00080808);
	ctl_apb_wr(0x208, 0x00000000);
	ctl_apb_wr(0x20c, 0x00000000);
	ctl_apb_wr(0x210, 0x00001f1f);
	ctl_apb_wr(0x214, 0x070f0707);
	ctl_apb_wr(0x218, 0x07070707);
	ctl_apb_wr(0x21c, 0x00000f08);
	ctl_apb_wr(0x220, 0x00000000);
	ctl_apb_wr(0x224, 0x07070707);
	ctl_apb_wr(0x228, 0x07070707);
	ctl_apb_wr(0x22c, 0x00000007);
	ctl_apb_wr(0x240, 0x06050714);
	ctl_apb_wr(0x244, 0x00000000);
	ctl_apb_wr(0x250, 0x00f53f00);
	ctl_apb_wr(0x254, 0x00000000);
	ctl_apb_wr(0x25c, 0x0f000001);
	ctl_apb_wr(0x264, 0x0f00007f);
	ctl_apb_wr(0x26c, 0x0f00007f);
	ctl_apb_wr(0x300, 0x00000000);
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_wr(0x30c, 0x00000000);
	ctl_apb_wr(0x320, 0x00000001);
	ctl_apb_wr(0x328, 0x00000000);
	ctl_apb_wr(0x36c, 0x00100000);
	ctl_apb_wr(0x490, 0x00000001);
#ifdef PLATFORM_SP7350
	ctl_apb_wr(0x0540,0x00000001);//BUS1
	ctl_apb_wr(0x05f0,0x00000001);//BUS2
	ctl_apb_wr(0x06a0,0x00000001);//BUS3
#endif
	ctl_apb_rd(0x060);
	ctl_apb_wr(0x400, 0x00000000);
	ctl_apb_wr(0x404, 0x0000000f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x408, 0x0000400f);
	ctl_apb_wr(0x408, 0x0000500f);
	ctl_apb_wr(0x408, 0x0000500f);
	ctl_apb_wr(0x408, 0x0000100f);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
#endif

#ifdef SDRAM_SPEED_666
	ctl_apb_wr(0x304, 0x00000001);
	ctl_apb_wr(0x030, 0x00000001);
	ctl_apb_rd(0x004);
	ctl_apb_wr(0x000, 0x83080020);
	ctl_apb_wr(0x010, 0x40003030);
	ctl_apb_wr(0x014, 0x00008043);
	ctl_apb_wr(0x01c, 0x040df7d4);
	ctl_apb_wr(0x020, 0x00001202);
	ctl_apb_wr(0x024, 0x0c3a97b5);
	ctl_apb_wr(0x02c, 0x00000000);
	ctl_apb_wr(0x030, 0x00000020);
	ctl_apb_wr(0x034, 0x0040b904);
	ctl_apb_wr(0x038, 0x004e0000);
	ctl_apb_wr(0x050, 0x00210000);
	ctl_apb_wr(0x054, 0x000e0037);
	ctl_apb_wr(0x060, 0x00000000);
	ctl_apb_wr(0x064, 0x005180fe);
	ctl_apb_wr(0x068, 0x003c0000);
	ctl_apb_wr(0x0c0, 0x00000000);
	ctl_apb_wr(0x0c4, 0x00000000);
	ctl_apb_wr(0x0d0, 0xc0020002);
	ctl_apb_wr(0x0d4, 0x00010002);
	ctl_apb_wr(0x0d8, 0x0000e205);
	ctl_apb_wr(0x0dc, 0x00440024);
	ctl_apb_wr(0x0e0, 0x00310008);
	ctl_apb_wr(0x0e4, 0x00040008);
	ctl_apb_wr(0x0e8, 0x0000004d);
	ctl_apb_wr(0x0ec, 0x0000004d);
	ctl_apb_wr(0x0f0, 0x00000000);
#ifdef NANYA
	ctl_apb_wr(0x0f4, 0x0000033f);
#elif defined(MICRON)
	ctl_apb_wr(0x0f4, 0x0000053f);
#endif
	ctl_apb_wr(0x100, 0x171b2d1c);
	ctl_apb_wr(0x104, 0x00050528);
	ctl_apb_wr(0x108, 0x060c0e12);
	ctl_apb_wr(0x10c, 0x0090a006);
	ctl_apb_wr(0x110, 0x0c04070c);
	ctl_apb_wr(0x114, 0x02040a0a);
	ctl_apb_wr(0x118, 0x01010006);
	ctl_apb_wr(0x11c, 0x00000402);
	ctl_apb_wr(0x120, 0x01010101);
	ctl_apb_wr(0x124, 0x00000008);
	ctl_apb_wr(0x128, 0x00020a00);
	ctl_apb_wr(0x12c, 0x0101001a);
	ctl_apb_wr(0x130, 0x00020000);
	ctl_apb_wr(0x134, 0x0b100002);
	ctl_apb_wr(0x138, 0x00000102);
	ctl_apb_wr(0x13c, 0x80000000);
	ctl_apb_wr(0x180, 0xd29b0014);
	ctl_apb_wr(0x184, 0x022b282b);
	ctl_apb_wr(0x188, 0x00000000);
	ctl_apb_wr(0x190, 0x03898204);
	ctl_apb_wr(0x194, 0x00090202);
	ctl_apb_wr(0x198, 0x07513011);
	ctl_apb_wr(0x19c, 0x00000071);
	ctl_apb_wr(0x1a0, 0xe0400018);
	ctl_apb_wr(0x1a4, 0x0042009c);
	ctl_apb_wr(0x1a8, 0x00000000);
	ctl_apb_wr(0x1b0, 0x00000041);
	ctl_apb_wr(0x1b4, 0x00000904);
	ctl_apb_wr(0x1b8, 0x00000008);
	ctl_apb_wr(0x1c0, 0x00000000);
	ctl_apb_wr(0x1c4, 0x00000000);
	ctl_apb_wr(0x200, 0x00000017);
	ctl_apb_wr(0x204, 0x00080808);
	ctl_apb_wr(0x208, 0x00000000);
	ctl_apb_wr(0x20c, 0x00000000);
	ctl_apb_wr(0x210, 0x00001f1f);
	ctl_apb_wr(0x214, 0x070f0707);
	ctl_apb_wr(0x218, 0x07070707);
	ctl_apb_wr(0x21c, 0x00000f08);
	ctl_apb_wr(0x220, 0x00000000);
	ctl_apb_wr(0x224, 0x07070707);
	ctl_apb_wr(0x228, 0x07070707);
	ctl_apb_wr(0x22c, 0x00000007);
	ctl_apb_wr(0x240, 0x06070944);
	ctl_apb_wr(0x244, 0x00000000);
	ctl_apb_wr(0x250, 0x00dd3f00);
	ctl_apb_wr(0x254, 0x00000000);
	ctl_apb_wr(0x25c, 0x0f000001);
	ctl_apb_wr(0x264, 0x0f00007f);
	ctl_apb_wr(0x26c, 0x0f00007f);
	ctl_apb_wr(0x300, 0x00000000);
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_wr(0x30c, 0x00000000);
	ctl_apb_wr(0x320, 0x00000001);
	ctl_apb_wr(0x328, 0x00000000);
	ctl_apb_wr(0x36c, 0x00100000);
	ctl_apb_wr(0x490, 0x00000001);
#ifdef PLATFORM_SP7350
	ctl_apb_wr(0x0540,0x00000001);//BUS1
	ctl_apb_wr(0x05f0,0x00000001);//BUS2
	ctl_apb_wr(0x06a0,0x00000001);//BUS3
#endif
	ctl_apb_rd(0x060);
	ctl_apb_wr(0x400, 0x00000000);
	ctl_apb_wr(0x404, 0x0000000f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x404, 0x0000100f);
	ctl_apb_wr(0x408, 0x0000400f);
	ctl_apb_wr(0x408, 0x0000500f);
	ctl_apb_wr(0x408, 0x0000500f);
	ctl_apb_wr(0x408, 0x0000100f);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030, 0x00000020);
#endif

#ifdef NANYA
	prn_string("NANYA");
	prn_string("\n");
#elif defined(MICRON)
	prn_string("MICRON");
	prn_string("\n");
#endif
	return 0;
}


int dwc_umctl2_init_after_ctl_rst(void)
{
#ifdef SDRAM_SPEED_1600
	prn_string("SDRAM_SPEED_1600");
	prn_string("\n");
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
	prn_string("SDRAM_SPEED_1200");
	prn_string("\n");
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
	prn_string("SDRAM_SPEED_800");
	prn_string("\n");
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
	prn_string("SDRAM_SPEED_666");
	prn_string("\n");
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
			prn_string("Register programming done!!!");
			prn_string("\n");
			break;
		}
	}
}

void dwc_ddrphy_phyinit_userCustom_G_waitFwDone_of_SP()
{
	#ifdef STREAM_MESSAGE
 	UINT32 string_index;
	#endif
	UINT16 rd_data, args, i, low_16bit, upper_16bit;
	UINT8 train_test = 0;
    mp = 0;
	prn_string("Start to wait for the training firmware to complete v.00 !!!");
	prn_string("\n");
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
				prn_string("End of initialization.");
				prn_string("\n");
				break;
			case 0x01:
				prn_string("End of fine write leveling.");
				prn_string("\n");
				break;
			case 0x02:
				prn_string("End of read enable training.");
				prn_string("\n");
				break;
			case 0x03:
				prn_string("End of read delay center optimization.");
				prn_string("\n");
				break;
			case 0x04:
				prn_string("End of write delay center optimization.");
				prn_string("\n");
				break;
			case 0x05:
				prn_string("End of 2D read delay/voltage center optimization.");
				prn_string("\n");
				break;
			case 0x06:
				prn_string("End of 2D write delay/voltage center optimization.");
				prn_string("\n");
				break;
			case 0x07:
				train_test = 1;
				prn_string("Training has run successfully.(firmware complete)");
				prn_string("\n");
				mp = 1;
				break;
			case 0x08:
				#ifdef STREAM_MESSAGE
				prn_string("Start streaming message mode.");
				prn_string("\n");
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
				string_index = (upper_16bit << 16) | low_16bit;
				prn_string("string_index=");prn_dword0(string_index);
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
					string_index = (upper_16bit << 16) | low_16bit;
					prn_string("args=");prn_dword0(low_16bit);
					prn_string("\n");
					#endif
				}
				break;
			case 0x09:
				prn_string("End of max read latency training.");
				prn_string("\n");
				break;
			case 0x0a:
				prn_string("End of read dq deskew training.");
				prn_string("\n");
				break;
			case 0x0b:
				prn_string("Reserved.");
				prn_string("\n");
				break;
			case 0x0c:
				prn_string("End of all DB training(MREP/DWL/MRD/MWD complete).");
				prn_string("\n");
				break;
			case 0x0d:
				prn_string("End of CA training.");
				prn_string("\n");
				break;
			case 0xfd:
				prn_string("End of MPR read delay center optimization.");
				prn_string("\n");
				break;
			case 0xfe:
				prn_string("End of Wrtie leveling coarse delay.");
				prn_string("\n");
				break;
			case 0xff:
				prn_string("Training has failed.(firmware complete)");
				prn_string("\n");
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
