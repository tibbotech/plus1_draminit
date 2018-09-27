#include <regmap.h>
#include <config.h>

#define dbg_stamp(value) // { *(volatile unsigned int *)RF_GRP(0, 0) = (unsigned int)(value); }

#define dbg()				\
	do {						\
		prn_string("[DBG] ");	\
		prn_string(__FILE__);	\
		prn_string(" :");		\
		prn_decimal(__LINE__);	\
		prn_string("\n");		\
	} while (0)

#define uart0_nl()			\
	uart0_putc(0x0D);		\
	uart0_putc(0x0A);

extern void uart0_wait(void);
extern void uart0_putc(unsigned char c);


char sp_getChar(void);
int  sp_getString(char *data);

#ifndef DRAM_INIT_DEBUG
#define prn_string(_x)      do {} while(0)
#define prn_byte(_x)        do {} while(0)
#define prn_char(_x)   do {} while(0)
#define prn_byten(_x)       do {} while(0)
#define prn_dword(_x)       do {} while(0)
#define prn_dword0(_x)       do {} while(0)
#define prn_decimal(_x)     do {} while(0)
#define prn_dump_buffer(buf,len) do {} while(0)
#else
void prn_string(char *);
void prn_byte(unsigned char);
void prn_char(unsigned char);
void prn_dword(unsigned int);
void prn_dword0(unsigned int);
void prn_decimal(unsigned int);
void prn_dump_buffer(unsigned char *buf, int len);
#endif
