//OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
//OUTPUT_ARCH(arm)
OUTPUT_FORMAT("elf64-littleriscv", "elf64-littleriscv",	      "elf64-littleriscv")
OUTPUT_ARCH( "riscv" )
ENTRY(__start)

MEMORY
{
#ifdef DRAM_INIT_DEBUG
	/* Following settings are just for passing linking when building draminit */
	/* For debug mode, draminit must be linked to xboot because its size is over 7KB */
	/* If (sizeof(XBOOT) + sizeof(draminit)) is still too large, some functions in XBOOT or draminit must be removed */
	header : ORIGIN = 0xFe800000, LENGTH = 64
	public : ORIGIN = 0xFe800040, LENGTH = 38K -64
	sram   : ORIGIN = 0xFe809800, LENGTH = 2K
#elif defined DRAM_INIT
	header : ORIGIN = 0xFe807010, LENGTH = 64
	public : ORIGIN = 0xFe807050, LENGTH = 7K - 64
	sram   : ORIGIN = 0xFe808c10, LENGTH = 1K + 512    /* borrow xboot sect_buf */
#elif defined DRAMSCAN
	public : ORIGIN = 0xF8000000, LENGTH = 1M
	sram   : ORIGIN = 0xFe800000, LENGTH = 38K
#elif defined SISCOPE
	public : ORIGIN = 0xFe800000, LENGTH = 38K
	sram   : ORIGIN = 0xFe809800, LENGTH = 2K
#endif
}

SECTIONS
{
	.text :
	{
		arch/riscv/start.o (.text)
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
		*(.sbss.*)
		*(.gnu.linkonce.b.*)
		*(COMMON)
		. = ALIGN(8);
		__bss_end__ = .;
	} > sram

#if defined(DRAMSCAN) || defined(SISCOPE)
	_sp_start = 0xFe80a000 ;
#endif
}
