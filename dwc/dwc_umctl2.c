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
#ifdef DRAM_TYPE_LPDDR4
#ifdef SDRAM_SPEED_1600
#ifdef PLATFORM_SP7350
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
	ctl_apb_wr(0x1a8, 0x80000000);
	ctl_apb_wr(0x1b0, 0x00000041);
	ctl_apb_wr(0x1b4, 0x0000170a);
	ctl_apb_wr(0x1b8, 0x00000008);
	ctl_apb_wr(0x1c0, 0x00000000);
	ctl_apb_wr(0x1c4, 0x00000000);

	ctl_apb_wr(0x200, 0x00000016);
	ctl_apb_wr(0x204, 0x00080808);
	ctl_apb_wr(0x208, 0x00000000);
	ctl_apb_wr(0x20c, 0x00000000);
	ctl_apb_wr(0x210, 0x00001F1F);
	ctl_apb_wr(0x214, 0x070F0707);
	ctl_apb_wr(0x218, 0x08070707);
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

	ctl_apb_wr(0x540, 0x00000001);
	ctl_apb_wr(0x5f0, 0x00000001);
	ctl_apb_wr(0x6a0, 0x00000001);
#endif//#ifdef PLATFORM_SP7350

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
	ctl_apb_wr(0x050, 0x00210004);
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
	ctl_apb_wr(0x1a8, 0x80000000);
	ctl_apb_wr(0x1b0, 0x00000041);
	ctl_apb_wr(0x1b4, 0x0000170a);
	ctl_apb_wr(0x1b8, 0x00000008);
	ctl_apb_wr(0x1c0, 0x00000000);
	ctl_apb_wr(0x1c4, 0x00000000);
#ifdef MT53E1G32D2_A
	ctl_apb_wr(0x200, 0x00000018);
#else
	ctl_apb_wr(0x200, 0x00000017);
#endif
	ctl_apb_wr(0x204, 0x00080808);
	ctl_apb_wr(0x208, 0x00000000);
	ctl_apb_wr(0x20c, 0x00000000);
	ctl_apb_wr(0x210, 0x00001F1F);
	ctl_apb_wr(0x214, 0x070F0707);
	ctl_apb_wr(0x218, 0x07070707);
#ifdef MT53E1G32D2_A
	ctl_apb_wr(0x21c, 0x00000F07);
#else
	ctl_apb_wr(0x21c, 0x00000F08);
#endif
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
#endif//#ifdef PLATFORM_Q645
#endif//#ifdef SDRAM_SPEED_1600

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
	ctl_apb_wr(0x1a8, 0x80000000);
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
#endif//#ifdef SDRAM_SPEED_1200

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
#if defined(NT6AN1024F32AV) || defined(MT53E1G32D2_A) || defined(MT53E1G32D2_B)
	ctl_apb_wr(0x0f4, 0x0000032f);
#elif defined(MT53D1024M32D4)
	ctl_apb_wr(0x0f4, 0x0000053f);
#endif
	ctl_apb_wr(0x100, 0x11100d11);
	ctl_apb_wr(0x104, 0x00030418);
#if defined(NT6AN1024F32AV) || defined(MT53E1G32D2_A) || defined(MT53E1G32D2_B)
	ctl_apb_wr(0x108, 0x04070a0d);
#elif defined(MT53D1024M32D4)
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
	ctl_apb_wr(0x1a8, 0x80000000);
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
#endif//#ifdef SDRAM_SPEED_800

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
#if defined(NT6AN1024F32AV) || defined(MT53E1G32D2_A) || defined(MT53E1G32D2_B)
	ctl_apb_wr(0x0f4, 0x0000033f);
#elif defined(MT53D1024M32D4)
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
	ctl_apb_wr(0x1a8, 0x80000000);
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
#endif//#ifdef SDRAM_SPEED_666
#endif//#ifdef DRAM_TYPE_LPDDR4

#ifdef DRAM_TYPE_DDR4
	ctl_apb_wr(0x304,0x00000001);
	ctl_apb_wr(0x030,0x00000001);
	ctl_apb_rd(0x004);
	ctl_apb_wr(0x000,0x83040210);//M
	ctl_apb_wr(0x010,0x40003030); //M
	ctl_apb_wr(0x014,0x00024f12);
	ctl_apb_wr(0x01c,0xd0e98cb2);
	ctl_apb_wr(0x020,0x00001204);
	ctl_apb_wr(0x024,0xfeeb6e88);
	ctl_apb_wr(0x02c,0x00000000);
	ctl_apb_wr(0x030,0x00000020);
	ctl_apb_wr(0x034,0x00404004);
	ctl_apb_wr(0x038,0x00350002);
	ctl_apb_wr(0x050,0x48210000);
	ctl_apb_wr(0x054,0x00300086);
	ctl_apb_wr(0x060,0x00000001);
	ctl_apb_wr(0x064,0x00a2816f);
	ctl_apb_wr(0x068,0x005b0000);
	ctl_apb_wr(0x0c0,0x00000000);
	ctl_apb_wr(0x0c4,0x00000000);
	ctl_apb_wr(0x0d0,0xc0020002);
	ctl_apb_wr(0x0d4,0x00010002);
	ctl_apb_wr(0x0d8,0x0000ef00);
	ctl_apb_wr(0x0dc,0x0e410001);
	ctl_apb_wr(0x0e0,0x00280200);
	ctl_apb_wr(0x0e4,0x00110300);
	ctl_apb_wr(0x0e8,0x180005c0);
	ctl_apb_wr(0x0ec,0x00000c59);
	ctl_apb_wr(0x0f0,0x00000000);
	ctl_apb_wr(0x0f4,0x0000033f);
	ctl_apb_wr(0x100,0x15142d15);
	ctl_apb_wr(0x104,0x0004051f);
	ctl_apb_wr(0x108,0x08090510);
	ctl_apb_wr(0x10c,0x0000400c);
	ctl_apb_wr(0x110,0x0904050a);
	ctl_apb_wr(0x114,0x07070404);
	ctl_apb_wr(0x118,0x00000000);
	ctl_apb_wr(0x11c,0x00000707);
	ctl_apb_wr(0x120,0x0707120d);
	ctl_apb_wr(0x124,0x4002040d);
	ctl_apb_wr(0x128,0x000e0c05);
	ctl_apb_wr(0x12c,0x1d09011e);
	ctl_apb_wr(0x130,0x0c00000e);
	ctl_apb_wr(0x134,0x00000000);
	ctl_apb_wr(0x138,0x00000000);
	ctl_apb_wr(0x13c,0x00000000);
	ctl_apb_wr(0x180,0xe1000040);
	ctl_apb_wr(0x184,0x0003206e);
	ctl_apb_wr(0x188,0x00000000);
	ctl_apb_wr(0x190,0x038d820b);
	ctl_apb_wr(0x194,0x01070202);
	ctl_apb_wr(0x198,0x07a17110);
	ctl_apb_wr(0x19c,0x00000071);
	ctl_apb_wr(0x1a0,0x80400018);
	ctl_apb_wr(0x1a4,0x008f00d5);
	ctl_apb_wr(0x1a8,0x00000000);
	ctl_apb_wr(0x1b0,0x000000d1);
	ctl_apb_wr(0x1b4,0x00000d0b);
	ctl_apb_wr(0x1b8,0x00000008);
	ctl_apb_wr(0x1c0,0x00000001);
	ctl_apb_wr(0x1c4,0xd0000000);
	ctl_apb_wr(0x200,0x00000018);  /// W 9->b
	ctl_apb_wr(0x204,0x003f0909);
	ctl_apb_wr(0x208,0x01010100);
	ctl_apb_wr(0x20c,0x01010101);
	ctl_apb_wr(0x210,0x00001f1f);
	ctl_apb_wr(0x214,0x070f0707);
	ctl_apb_wr(0x218,0x07070707);
	ctl_apb_wr(0x21c,0x00000f07);
	ctl_apb_wr(0x220,0x00003f01);
	ctl_apb_wr(0x224,0x07070707);
	ctl_apb_wr(0x228,0x07070707);
	ctl_apb_wr(0x22c,0x00000007);
	ctl_apb_wr(0x240,0x07000708);
	ctl_apb_wr(0x244,0x00002223);
	ctl_apb_wr(0x250,0x00dd3f00);
	ctl_apb_wr(0x254,0x00000000);
	ctl_apb_wr(0x25c,0x0f000001);
	ctl_apb_wr(0x264,0x0f00007f);  /// r 16->1a  w d->b
	ctl_apb_wr(0x26c,0x0f00007f);
	ctl_apb_wr(0x300,0x00000000);
	ctl_apb_wr(0x304,0x00000000);
	ctl_apb_wr(0x30c,0x00000000);
	ctl_apb_wr(0x320,0x00000001);
	ctl_apb_wr(0x328,0x00000000);
	ctl_apb_wr(0x36c,0x00010000);
	ctl_apb_wr(0x490,0x00000001);
	ctl_apb_wr(0x540,0x00000001);
	ctl_apb_wr(0x5f0,0x00000001);  //DBI
	ctl_apb_wr(0x6a0,0x00000001);
	ctl_apb_rd(0x060);
	ctl_apb_wr(0x400,0x00000000);
	ctl_apb_wr(0x404,0x0000000f);
	ctl_apb_wr(0x4b4,0x0000000f);
	ctl_apb_wr(0x564,0x0000000f);
	ctl_apb_wr(0x614,0x0000000f);
	ctl_apb_wr(0x404,0x0000100f);
	ctl_apb_wr(0x4b4,0x0000100f);
	ctl_apb_wr(0x564,0x0000100f);
	ctl_apb_wr(0x614,0x0000100f);
	ctl_apb_wr(0x404,0x0000100f);
	ctl_apb_wr(0x4b4,0x0000100f);
	ctl_apb_wr(0x564,0x0000100f);
	ctl_apb_wr(0x614,0x0000100f);
	ctl_apb_wr(0x404,0x0000100f);
	ctl_apb_wr(0x4b4,0x0000100f);
	ctl_apb_wr(0x564,0x0000100f);
	ctl_apb_wr(0x614,0x0000100f);
	ctl_apb_wr(0x408,0x0000400f);
	ctl_apb_wr(0x4b8,0x0000400f);
	ctl_apb_wr(0x568,0x0000400f);
	ctl_apb_wr(0x618,0x0000400f);
	ctl_apb_wr(0x408,0x0000500f);
	ctl_apb_wr(0x4b8,0x0000500f);
	ctl_apb_wr(0x568,0x0000500f);
	ctl_apb_wr(0x618,0x0000500f);
	ctl_apb_wr(0x408,0x0000500f);
	ctl_apb_wr(0x4b8,0x0000500f);
	ctl_apb_wr(0x568,0x0000500f);
	ctl_apb_wr(0x618,0x0000500f);
	ctl_apb_wr(0x408,0x0000100f);
	ctl_apb_wr(0x4b8,0x0000100f);
	ctl_apb_wr(0x568,0x0000100f);
	ctl_apb_rd(0x618);
	ctl_apb_rd(0x030);
	ctl_apb_wr(0x030,0x00000020);
#endif//#ifdef DRAM_TYPE_DDR4

#ifdef DRAM_TYPE_DDR3
	ctl_apb_wr(0x304, 0x00000001);
	ctl_apb_wr(0x030, 0x00000001);
	//ctl_apb_rd(0x004, 0x00000000);
	ctl_apb_rd(0x004);

	ctl_apb_wr(0x000, 0x83040001);
	ctl_apb_wr(0x010, 0x4000e030);
	ctl_apb_wr(0x014, 0x0001b7aa);
	ctl_apb_wr(0x01c, 0xd569f1c4);
	ctl_apb_wr(0x020, 0x00000702);
	ctl_apb_wr(0x024, 0x6ab4bde9);
	ctl_apb_wr(0x02c, 0x00000001);
	ctl_apb_wr(0x030, 0x0000002a);
	ctl_apb_wr(0x034, 0x001bc006);
	ctl_apb_wr(0x038, 0x000f0003);
	ctl_apb_wr(0x050, 0xb8f1f010);
	ctl_apb_wr(0x054, 0x005a0049);
	ctl_apb_wr(0x060, 0x00000000);
	ctl_apb_wr(0x064, 0x0071807a);
	ctl_apb_wr(0x068, 0x00600000);
	ctl_apb_wr(0x0c0, 0x00000000);
	ctl_apb_wr(0x0c4, 0x00000000);
	ctl_apb_wr(0x0d0, 0xc0030002);
	ctl_apb_wr(0x0d4, 0x0001000d);
	ctl_apb_wr(0x0d8, 0x0000ad00);
	ctl_apb_wr(0x0dc, 0x1f140010);
	ctl_apb_wr(0x0e0, 0x00200000);
	ctl_apb_wr(0x0e4, 0x000b0276);
	ctl_apb_wr(0x0e8, 0x00000000);
	ctl_apb_wr(0x0ec, 0x00000000);
	ctl_apb_wr(0x0f0, 0x00000020);
	ctl_apb_wr(0x0f4, 0x0000e2ca);
	ctl_apb_wr(0x100, 0x13111f10);
	ctl_apb_wr(0x104, 0x00030917);
	ctl_apb_wr(0x108, 0x0a0c050a);
	ctl_apb_wr(0x10c, 0x00002007);
	//ctl_apb_wr(0x110, 0x01020307);
	ctl_apb_wr(0x110, 0x06020307);
	ctl_apb_wr(0x114, 0x090a0303);
	ctl_apb_wr(0x118, 0x00070002);
	ctl_apb_wr(0x11c, 0x00000a09);
	ctl_apb_wr(0x120, 0x04040a09);
	ctl_apb_wr(0x124, 0x40000012);
	ctl_apb_wr(0x128, 0x00040608);
	ctl_apb_wr(0x12c, 0x0101000e);
	ctl_apb_wr(0x130, 0x00000000);
	ctl_apb_wr(0x134, 0x00000000);
	ctl_apb_wr(0x138, 0x00000000);
	ctl_apb_wr(0x13c, 0x00000000);
	ctl_apb_wr(0x180, 0x00960026);
	ctl_apb_wr(0x184, 0x00000070);
	ctl_apb_wr(0x188, 0x00000000);
	//ctl_apb_wr(0x190, 0x0393820f);
	ctl_apb_wr(0x190, 0x03878204);
	ctl_apb_wr(0x194, 0x00070202);
	ctl_apb_wr(0x198, 0x07500020);
	ctl_apb_wr(0x19c, 0x00000011);
	ctl_apb_wr(0x1a0, 0x80400018);
	ctl_apb_wr(0x1a4, 0x003400e0);
	ctl_apb_wr(0x1a8, 0x00000000);
	ctl_apb_wr(0x1b0, 0x000000c1);
	ctl_apb_wr(0x1b4, 0x0000130f);
	ctl_apb_wr(0x1b8, 0x00000008);
	//ctl_apb_wr(0x1c0, 0x00000001);
	ctl_apb_wr(0x1c0, 0x00000000);
	ctl_apb_wr(0x1c4, 0xff000000);
	ctl_apb_wr(0x200, 0x00000012);
	ctl_apb_wr(0x204, 0x00011702);
	ctl_apb_wr(0x208, 0x01030000);
	ctl_apb_wr(0x20c, 0x02050202);
	ctl_apb_wr(0x210, 0x00001f1f);
	ctl_apb_wr(0x214, 0x0b060504);
	ctl_apb_wr(0x218, 0x0f070407);
	ctl_apb_wr(0x21c, 0x00000f0f);
	ctl_apb_wr(0x220, 0x00000000);
	ctl_apb_wr(0x224, 0x09060907);
	ctl_apb_wr(0x228, 0x090a0903);
	ctl_apb_wr(0x22c, 0x0000000a);
	ctl_apb_wr(0x240, 0x031e0d3c);
	ctl_apb_wr(0x244, 0x00003023);
	ctl_apb_wr(0x250, 0x14521f06);
	ctl_apb_wr(0x254, 0x0000002c);
	ctl_apb_wr(0x25c, 0x00001339);
	ctl_apb_wr(0x264, 0x6500716e);
	ctl_apb_wr(0x26c, 0x3f00d144);
	ctl_apb_wr(0x300, 0x00000050);
	ctl_apb_wr(0x304, 0x00000000);
	ctl_apb_wr(0x30c, 0x00000000);
	ctl_apb_wr(0x320, 0x00000001);
	ctl_apb_wr(0x328, 0x00000000);
	ctl_apb_wr(0x36c, 0x00010001);
	ctl_apb_wr(0x400, 0x00000101);
	ctl_apb_wr(0x404, 0x00001303);
	ctl_apb_wr(0x408, 0x000053c7);
	ctl_apb_wr(0x490, 0x00000001);
	ctl_apb_wr(0x494, 0x0011000e);
	ctl_apb_wr(0x498, 0x06190712);
	ctl_apb_wr(0x49c, 0x00100402);
	ctl_apb_wr(0x4a0, 0x00340698);
	ctl_apb_wr(0x4b4, 0x0000238c);
	ctl_apb_wr(0x4b8, 0x000003e4);
	ctl_apb_wr(0x540, 0x00000001);
	ctl_apb_wr(0x544, 0x00100000);
	ctl_apb_wr(0x548, 0x0421001a);
	ctl_apb_wr(0x54c, 0x01100b0a);
	ctl_apb_wr(0x550, 0x0051067c);
	ctl_apb_wr(0x564, 0x00006092);
	ctl_apb_wr(0x568, 0x0000206e);
	ctl_apb_wr(0x5f0, 0x00000001);
	ctl_apb_wr(0x5f4, 0x0010000c);
	ctl_apb_wr(0x5f8, 0x052f0764);
	ctl_apb_wr(0x5fc, 0x01000603);
	ctl_apb_wr(0x600, 0x044406cd);
	ctl_apb_wr(0x614, 0x000003cf);
	ctl_apb_wr(0x618, 0x000052b2);
	//ctl_apb_wr(0x61c, 0x00000000);
	ctl_apb_wr(0x6a0, 0x00000001);
	//ctl_apb_rd(0x060, 0x00000000);
	ctl_apb_rd(0x060);

	//ctl_apb_rd(0x030, 0x0000002a);
	ctl_apb_rd(0x030);
#endif//#ifdef DRAM_TYPE_DDR3

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
 	UINT32 string_index;
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
