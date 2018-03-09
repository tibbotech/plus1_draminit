#ifndef __INC_REGMAP_GEMINI_H
#define __INC_REGMAP_GEMINI_H

struct moon0_regs {
	unsigned int stamp;            // 0.0
	unsigned int g0_reserved_0[3]; // 0.1
	unsigned int clk_sel[2];       // 0.4
	unsigned int pllsys_cfg_sel;   // 0.6
	unsigned int clken[5];         // 0.7
	unsigned int gclken[5];        // 0.12
	unsigned int reset[5];         // 0.17
	unsigned int g0_reserved_2[6]; // 0.22
	unsigned int hw_cfg;           // 0.28
	unsigned int sfg_cfg_func;     // 0.29
	unsigned int g0_reserved_3[2]; // 0.30
};
#define MOON0_REG ((volatile struct moon0_regs *)RF_GRP(0, 0))

struct moon1_regs {
	unsigned int sft_cfg[32];
};
#define MOON1_REG ((volatile struct moon1_regs *)RF_GRP(1, 0))

struct moon2_regs {
	unsigned int sft_cfg[32];
};
#define MOON2_REG ((volatile struct moon2_regs *)RF_GRP(2, 0))

struct hb_gp_regs {
	unsigned int hb_otp_data0;
	unsigned int hb_otp_data1;
	unsigned int hb_otp_data2;
	unsigned int hb_otp_data3;
	unsigned int hb_otp_data4;
	unsigned int hb_otp_data5;
	unsigned int hb_otp_data6;
	unsigned int hb_otp_data7;
	unsigned int hb_otp_ctl;
	unsigned int hb_otp_data;
	unsigned int g7_reserved[22];
};
#define HB_GP_REG ((volatile struct hb_gp_regs *)RF_GRP(350, 0))

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

#endif /* __INC_REGMAP_GEMINI_H */
