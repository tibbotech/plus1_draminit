OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)

MEMORY
{
	header : ORIGIN = 0x9e800000, LENGTH = 64
	public : ORIGIN = 0x9e800040, LENGTH = 18K - 64
	sram   : ORIGIN = 0x9e804800, LENGTH = 2K
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
}
