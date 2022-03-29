#ifndef __CONFIG_H
#define __CONFIG_H

#if defined(CONFIG_PLATFORM_I143)
#define PLATFORM_I143
#elif defined(CONFIG_PLATFORM_Q645)
#define PLATFORM_Q645
#elif defined(CONFIG_PLATFORM_SP7350)
#define PLATFORM_SP7350
#else
#define PLATFORM_PENTAGRAM
#endif

#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define REG_BASE		0xF8000000
#else
#define REG_BASE		0x9C000000
#endif

#ifdef PLATFORM_SP7350
#define REG_BASE_AO		0xF8800000
#define RF_GRP_AO(_grp, _reg)	((((_grp) * 32 + (_reg)) * 4) + REG_BASE_AO)
#endif

#define RF_GRP(_grp, _reg)	((((_grp) * 32 + (_reg)) * 4) + REG_BASE)
#define RF_MASK_V(_mask, _val)	(((_mask) << 16) | (_val))
#define RF_MASK_V_SET(_mask)	(((_mask) << 16) | (_mask))
#define RF_MASK_V_CLR(_mask)	(((_mask) << 16) | 0)

#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define UMCTL2_REG_Base		0xF8107000
#else
#define UMCTL2_REG_Base		0x9C107000
#endif

#if defined(PLATFORM_Q645) || defined(PLATFORM_SP7350)
#define DWC_PHY_REG_Base	0xF9000000
#endif

#endif /* __CONFIG_H */
