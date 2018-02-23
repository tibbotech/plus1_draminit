
#define REG_BASE                0x9c000000
#define RGST_BASE               REG_BASE

#define RF_GRP(_grp, _reg) ((((_grp) * 32 + (_reg)) * 4) + REG_BASE)

//FIXME
//#define PLATFORM_8388  // enable to build 8388-compatible version
