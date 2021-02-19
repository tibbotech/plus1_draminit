#define REG_BASE		0x9C000000
#define RF_GRP(_grp, _reg)	((((_grp) * 32 + (_reg)) * 4) + REG_BASE)
#define RF_MASK_V(_mask, _val)	(((_mask) << 16) | (_val))
#define RF_MASK_V_SET(_mask)	(((_mask) << 16) | (_mask))
#define RF_MASK_V_CLR(_mask)	(((_mask) << 16) | 0)

#define UMCTL2_REG_Base		0x9C107000

// #define PLATFORM_GEMINI
#ifdef CONFIG_PLATFORM_I143
#define PLATFORM_I143
#else
#define PLATFORM_PENTAGRAM
#endif
