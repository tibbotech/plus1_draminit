/**************************************************************************
*                    I N C L U D E   F I L E S                            *
 **************************************************************************/
#include <types.h>
#include <regmap.h>


void uart0_wait(void)
{
        unsigned int lsr = 0;

        while (!lsr) {
                lsr = UART0_REG->lsr;
                lsr &= 1;
        }
}

void uart0_putc(unsigned char c)
{
        uart0_wait();
        UART0_REG->dr = c;
}


//----------------------------------------------------------------------------
int memcmp(UINT8 *s1, UINT8 *s2, int n)
{
	int ret = 0;
	UINT8 *s1_end = s1 + n;

	while(s1 != s1_end) {
		if (*s1++ != *s2++)
			ret = 1;
	}

	return ret;
}

//----------------------------------------------------------------------------
void *memset(UINT8 *s1, int c, int n)
{
	UINT8 *s1_end = s1 + n;

	while (s1 != s1_end)
		*s1++ = c;

	return s1;
}

//----------------------------------------------------------------------------
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
