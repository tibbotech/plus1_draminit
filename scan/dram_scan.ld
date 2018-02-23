OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_scan_start)

/*
 * dramscan
 */
MEMORY
{
	public : ORIGIN = 0x98000000, LENGTH =1M
	sram   : ORIGIN = 0x9e800000, LENGTH = 1M
}

SECTIONS
{
	.text :
	{
		scan/scan_start.o (.text)
		*(.rodata)
		*(.text)
		*(.text.*)
	} > public

	.data :
	{
		*(.data)
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

	_sp_start = 0x9e808000 ;
}
