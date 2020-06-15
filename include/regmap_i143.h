#ifndef __INC_REGMAP_I143_H
#define __INC_REGMAP_I143_H
#if 0
struct moon0_regs {
	unsigned int tbd[32];
};
#define MOON0_REG ((volatile struct moon0_regs *)RF_GRP(0, 0))

struct moon1_regs {
	unsigned int tbd[32];
};
#define MOON1_REG ((volatile struct moon1_regs *)RF_GRP(1, 0))

struct moon2_regs {
	unsigned int tbd[32];
};
#define MOON2_REG ((volatile struct moon2_regs *)RF_GRP(2, 0))
#endif

struct uart_regs {
	unsigned int dr;  /* data register */
	unsigned int lsr; /* line status register */
	unsigned int msr; /* modem status register */
	unsigned int lcr; /* line control register */
	unsigned int mcr; /* modem control register */
	unsigned int div_l;
	unsigned int div_h;
	unsigned int isc; /* interrupt status/control */
	unsigned int txr; /* tx residue */
	unsigned int rxr; /* rx residue */
	unsigned int thr; /* rx threshold */
};
#define UART0_REG    ((volatile struct uart_regs *)RF_GRP(18, 0))
#endif /* __INC_REGMAP_PENTAGRAM_H */
