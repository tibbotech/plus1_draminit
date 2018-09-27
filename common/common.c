/**************************************************************************
*                    I N C L U D E   F I L E S                            *
 **************************************************************************/
#include <types.h>
#include <regmap.h>
#include <common.h>


void uart0_wait(void)
{
	unsigned int lsr = 0;

	while (!lsr) {
		lsr = UART0_REG->lsr;
		lsr &= 1;
	}
}

void uart0_rx_wait(void)
{
	unsigned int lsr = 0;

	while (!lsr) {
		lsr = UART0_REG->lsr;
		lsr = (lsr >> 1) & (0x01);
	}
}

void uart0_putc(unsigned char c)
{
	uart0_wait();
	UART0_REG->dr = c;
}

char sp_getChar(void)
{
	uart0_rx_wait();
	char rx = UART0_REG->dr;
	// if( 0x0D == rx )
	// else
	// prn_byte(rx);
	prn_char(rx);
	prn_string("\n");
	// prn_string("\n");
	return rx;
}


int sp_getString(char *data)
{
	int len = 0;
	do {
		uart0_rx_wait();
		data[len] = UART0_REG->dr;
		if (0x0D == data[len]) {
			prn_string("\n");
			data[len] = '\0';
			break;
		} else {
			prn_char(data[len]);
			len++;
		}
	} while (len < 100);
	return len;
}

// ----------------------------------------------------------------------------
int memcmp(UINT8 *s1, UINT8 *s2, int n)
{
	int ret = 0;
	UINT8 *s1_end = s1 + n;

	while (s1 != s1_end) {
		if (*s1++ != *s2++)
			ret = 1;
	}

	return ret;
}

// ----------------------------------------------------------------------------
void *memset(UINT8 *s1, int c, int n)
{
	UINT8 *s1_end = s1 + n;

	while (s1 != s1_end)
		*s1++ = c;

	return s1;
}

// ----------------------------------------------------------------------------
void *memcpy(UINT8 *dest, UINT8 *src, int n)
{
	UINT8 *src_end = (UINT8 *)((unsigned long)(src) + n);
	UINT8 *to = (UINT8 *)(dest);

	while (src != src_end) {
		*to = *src;
		to++;
		src++;
	}

	return (void *)(dest);
}

// ----------------------------------------------------------------------------
void u32Tostring(unsigned int num, char *string)
{
	char *str = string;
	int i;
	char tmp;


	for (i = 7; i > -1; i--) {
		tmp = ((num << ((7 - i) << 2) >> 28));
		if (tmp < 10) {
			*str++ = tmp + 48;
		} else {
			*str++ = tmp + 55;
		}
	}
	*str = '\0';
}
