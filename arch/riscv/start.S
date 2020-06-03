#include <config.h>
#include <asm/common.h>

#define SREG    sd
#define LREG     ld
#define REGBYTES 8
#define NONSMP_HART 0


	.section .text,"ax",@progbits
	.globl __start
/* Startup code */
__start:

   // la	sp, _sp_start

init_bss:
    /* init bss section */
    la	a0, __bss_start
    la	a1, __bss_end__-REGBYTES /* section end is actually the start of the next section */
    li	a2, 0x0
    jal	fill_block
init_stack:
    /* set stack pointer */
	call dram_init_main 
	ret
fill_block:
    sw		a2, 0(a0)
    bgeu	a0, a1, fb_end
    addi	a0, a0, REGBYTES
    j		fill_block
fb_end:
    ret	