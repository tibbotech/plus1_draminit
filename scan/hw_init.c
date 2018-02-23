#include <types.h>
#include <config.h>
#include <common.h>

void hardware_init(void)
{
	u32 val;

	// speed up spi nor flash
	//SPI_CTRL_REG->spi_ctrl = (SPI_CTRL_REG->spi_ctrl & ~0x7) | 0x4; // 4: CLK_SPI/8

	// clken0 ~ 3 enable
	MOON0_REG->clken[0] = 0xffffffff;
	MOON0_REG->clken[1] = 0xffffffff;
	MOON0_REG->clken[2] = 0xffffffff;
	MOON0_REG->clken[3] = 0xffffffff;
#ifndef PLATFORM_3502
	MOON0_REG->clken[4] = 0xffffffff;
#endif

	// release module reset
	MOON0_REG->reset[0] = 0x00000000;
	MOON0_REG->reset[1] = 0x00000000;
	MOON0_REG->reset[2] = 0x00000000;
	MOON0_REG->reset[3] = 0x00000000;
#ifndef PLATFORM_3502
	MOON0_REG->reset[4] = 0x00000000;
#endif
}

