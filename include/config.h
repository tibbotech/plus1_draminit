#ifndef __CONFIG_H
#define __CONFIG_H

#if defined(CONFIG_PLATFORM_I143)
#define PLATFORM_I143
#elif defined(CONFIG_PLATFORM_Q645)
#define PLATFORM_Q645
#else
#define PLATFORM_PENTAGRAM
#endif

#ifdef PLATFORM_Q645
#define REG_BASE		0xF8000000
#else
#define REG_BASE		0x9C000000
#endif
#define RF_GRP(_grp, _reg)	((((_grp) * 32 + (_reg)) * 4) + REG_BASE)
#define RF_MASK_V(_mask, _val)	(((_mask) << 16) | (_val))
#define RF_MASK_V_SET(_mask)	(((_mask) << 16) | (_mask))
#define RF_MASK_V_CLR(_mask)	(((_mask) << 16) | 0)

#ifdef PLATFORM_Q645
#define UMCTL2_REG_Base		0xF8107000
#else
#define UMCTL2_REG_Base		0x9C107000
#endif

#endif /* __CONFIG_H */
