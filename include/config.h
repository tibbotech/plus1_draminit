#define REG_BASE               0x9C000000
#define RF_GRP(_grp, _reg)     ((((_grp) * 32 + (_reg)) * 4) + REG_BASE)

#define PLATFORM_GEMINI
// #define PLATFORM_PENTAGRAM
