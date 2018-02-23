#include <types.h>
#include <config.h>
#include <common.h>

#define CMDLINE "MON >"
#define GROUP 	0
#define REG		1

#define DRAM_MAX_SIZE		0x20000000

u8 check_align(u32 addr)
{
	if (addr & 3) {
		prn_string("Address should be 4-bytes alignment!\n");
		return 0;
	}
	return 1;
}

u8 check_addr(volatile u32 addr)
{
	if (addr > DRAM_MAX_SIZE) {
		prn_string("Dump range should not bigger than ");
		prn_dword(DRAM_MAX_SIZE);
		prn_string("\n");
		return 0;
	}

	return 1;
}

u8 check_value(u32 value, u8 flag)
{
	if (flag == 0) {	// check group
		if (value < 512)
			return 1;
	} else if (flag == 1){ // check reg
		if (value < 32)
			return 1;
	} else {
		prn_string("unknown flag!\n");
		return 0;
	}
	return 0;
}

#define UART_LSR_RX             (1 << 1)
#define UART0_RX_READY()        ((UART0_REG->lsr) & UART_LSR_RX)
#define UART0_GET_ERROR()       (((UART0_REG->lsr) << 3) & 0xE0)
u8 uart0_getc(void)
{
        u8 uart0_data; 

        while (!(UART0_RX_READY()));

        uart0_data = UART0_REG->dr;
        if (UART0_GET_ERROR()) {
                uart0_data = 'E';
        }

        return uart0_data;
}

u8 uart0_getc_show_char(void)
{
	u8 byte;

	byte = uart0_getc();

	if (byte == 0xD) {
		uart0_putc('\n');
		uart0_putc('\r');
	} else {
		uart0_putc(byte);
	}

	return byte;
}

void show_result(volatile u32 addr)
{
	prn_string("value at ");
	prn_dword(addr);
	prn_string(" is ");
	prn_dword(*(volatile u32 *)addr);
	prn_string("\n");
}

void show_result_reg(u32 group, u32 reg)
{
	prn_string("value at group[");
	prn_decimal(group);
	prn_string("] register[");
	prn_decimal(reg);
	prn_string("] is ");
	prn_dword(RF_GRP(group, reg));
	prn_dword(*(volatile u32 *)(RF_GRP(group, reg)));
	prn_string("\n");
}


u8 get_word(u32 *addr)
{
	u8 count = 0;
	u8 byte;
	u32 result = 0;
	u8 flag = 0;

	while (1) {
		byte = uart0_getc_show_char();
		if((byte == 0xD) || (byte == ' ')) {
			if ((count == 0) && (flag == 0)) {
				prn_string(CMDLINE);
				continue;
			}
			*addr = result;
			return 1;
		}

		if (('0' <= byte) && (byte <= '9')) {
			if ((count == 0) && (byte == '0')) {
				flag = 1;
				continue;
			}
			byte -= '0';
		} else if (('A' <= byte) && (byte <= 'F')) {
			byte -= '7';
		} else if (('a' <= byte) && (byte <= 'f')) {
			byte -= 'W';
		} else if (byte == 0x8) {
			count--;
			result = (result >> 4);
			continue;
		} else {
			prn_string("\nInvalid value..., plz tryagain..\n");
			return 0;
		}

		result = (result << 4) + byte;
		count++;
		if (count > 8) {
			prn_string("\ninvalid value..,plz try again\n");
			return 0;
		}
	}
	prn_dword(result);
	*addr = result;
	return 1;
}

u8 get_word_dec(u32 *addr)
{
	u8 count = 0;
	u8 byte;
	u32 result = 0;
	u8 flag = 0;

	while (1) {
		byte = uart0_getc_show_char();
		if((byte == 0xD) || (byte == ' ')) {
			if ((count == 0) && (flag == 0)) {
				prn_string(CMDLINE);
				continue;
			}
			*addr = result;
			return 1;
		}

		if (('0' <= byte) && (byte <= '9')) {
			if ((count == 0) && (byte == '0')) {
				flag = 1;
				continue;
			}
			byte -= '0';
		} else if (byte == 0x8) {
			count--;
			result = (result >> 4);
			continue;
		} else {
			prn_string("\nInvalid value..., plz tryagain..\n");
			return 0;
		}

		result = (result * 10) + byte;
		count++;
		if (count > 4) {
			prn_string("\ninvalid value..,plz try again\n");
			return 0;
		}
	}
	prn_dword(result);
	*addr = result;
	return 1;
}

/*
void set_RX_intr(u8 onOff)
{
	if (onOff) {
		HWREG_W(uart0_isc, HWREG_R(uart0_isc) | (1 << 5));
	} else {
		HWREG_W(uart0_isc, HWREG_R(uart0_isc) & ~(1 << 5));
	}
}
*/

void registerRW(void)
{
	u8 byte;
	u32 address;
	u32 value;
	u8 flag = 0;
	u32 group;
	u32 reg;

	//set_RX_intr(0);

	prn_string("format:(\"->\" means \"enter\" \"=>\" means \"space\")\n");
	prn_string("[ r ] read address: r => address ->\n");
	prn_string("[ w ] write address: w => address => value ->\n");
	prn_string("[ R ] read register: R => group => reg ->\n");
	prn_string("[ W ] write register: W => group => reg => value ->\n");
	prn_string("[ g ] read group: g => group ->\n");
	prn_string("[ d ] dump memory content: d => start address => size ->\n");
	prn_string("[ e ] end of MON: e =>\n");

	prn_string(CMDLINE);

	while (1) {
		byte = uart0_getc_show_char();
		if (flag == 1) {
			if (byte == 0xD) {
				flag = 0;
				prn_string("Invalid command!\n");
				prn_string(CMDLINE);
				continue;
			}
			continue;
		} else {
			if (byte == 0xD) {
				prn_string(CMDLINE);
				flag = 0;
				continue;
			}
		}

		switch (byte) {
			case 'r':
				prn_string(" [ Read address ] address = 0x");
				if (get_word(&address)) {
					if (check_align(address)) {
						show_result(address);
					}
				}
				flag = 0;
				break;
			case 'w':
				prn_string(" [ Write address ] address = 0x");
				if (get_word(&address)) {
					if (check_align(address)) {
						prn_string(" value = 0x");
						if (get_word(&value)) {
							*(volatile u32 *)address = (u32)value;
							show_result(address);
						}
					}
				}
				flag = 0;
				break;
			case 'g':
				prn_string(" [ Read group ] group = ");
				if (get_word_dec(&address)) {
					if (check_value(address, GROUP)) {
						int i;
						prn_string("\n[ value list at group ");
						prn_dword(address);
						prn_string(" ]\n");
						for (i = 0; i < 32; i++) {
							prn_string("[ ");
							prn_dword(i);
							prn_string(" ] ");
							show_result(REG_BASE + (address * 128) + (i * 4));
						}
					}
				}
				flag = 0;
				break;
			case 'R':
				prn_string(" [Read register ] group = ");
				if (get_word_dec(&group)) {
					if (check_value(group, GROUP)) {
						prn_string(" reg = ");
						if (get_word_dec(&reg)) {
							if (check_value(reg, REG)) {
								show_result_reg(group, reg);
							}
						}
					}
				}
				flag = 0;
				break;
			case 'W':
				prn_string(" [ Write register ] group = ");
				if (get_word_dec(&group)) {
					if (check_value(group, GROUP)) {
						prn_string(" reg = ");
						if (get_word_dec(&reg)) {
							if (check_value(reg, REG)) {
								prn_string(" value = 0x");
								if (get_word(&value)) {
									*(volatile u32 *)(RF_GRP(group, reg)) = value;
									show_result_reg(group, reg);
								}
							}
						}
					}
				}
				flag = 0;
				break;
			case 'd':
			case 'D':
				prn_string(" [ Dump ] start address = 0x");
				if (get_word(&address)) {
					if (check_addr(address)) {
						prn_string(" size = 0x");
						if (get_word(&value)) {
							if (check_addr(address + value))
								prn_dump_buffer((u8 *)address, value);
						}
					}
				}
				flag = 0;
				break;
			case 'e':
			case 'E':
				//set_RX_intr(1);
				prn_string("\n End of Reg R/W\n");
				return;
			default:
				flag = 1;
				continue;
		}
		prn_string(CMDLINE);
	}
}

void regRW_isr(void)
{
	u8 uart0_data;
	static int count = 0;

	uart0_data = uart0_getc();

	switch (count) {
		case 0:
			 if ((uart0_data == 'M') || (uart0_data == 'm')) {
				count++;
				break;
			} else {
				return;
			}
		case 1:
			if ((uart0_data == 'O') || (uart0_data == 'o'))
				count++;
			else
				count--;
			return;
		case 2:
			if ((uart0_data == 'N') || (uart0_data == 'n'))
				registerRW();
			count = 0;
			return;
		default:
			break;
	}
}


