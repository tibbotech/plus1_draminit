OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
#ifdef DRAM_INIT
	header : ORIGIN = 0x9e807000, LENGTH = 64
	public : ORIGIN = 0x9e807040, LENGTH = 7K - 64
	sram   : ORIGIN = 0x9e808c00, LENGTH = 1K + 512    /* borrow xboot sect_buf */
#elif defined DRAMSCAN
	public : ORIGIN = 0x98000000, LENGTH = 1M
	sram   : ORIGIN = 0x9e800000, LENGTH = 38K
#elif defined SISCOPE
	public : ORIGIN = 0x9e800000, LENGTH = 38K
	sram   : ORIGIN = 0x9e809800, LENGTH = 2K
#endif
}

SECTIONS
{
	.text :
	{
		start.o (.text)
		*(.rodata)
		*(.text)
		*(.text.*)
	} > public

	.data :
	{
		*(.data)
		*(.data.*)
	} > sram

	.bss :
	{
		__bss_start = .;
		*(.bss)
		*(.bss.*)
		*(.sbss)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > sram

#if defined(DRAMSCAN) || defined(SISCOPE)
	_sp_start = 0x9e80a000 ;
#endif
}
