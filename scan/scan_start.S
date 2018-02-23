#include <config.h>
#include <asm/link.h>
#include <asm/common.h>

/* skip 64 bytes */
.rept   16
.long   0
.endr

/*
 * Magic number for SPI baremetal
 */
.word 0x6D622B52


.global _scan_start
_scan_start:
	/* setup stack */
	ldr	sp, =_sp_start
	bic	sp, sp, #7	/* for 8-byte alignment */

	/* zero BSS */
	ldr	r0, =__bss_start
	ldr	r1, =__bss_end__
	mov	r2, #0
clbss_l:
	cmp	r0, r1 
	bhs	clbss_e
	str	r2, [r0]
	add	r0, r0, #4
	b	clbss_l
clbss_e:
	bl	dram_scan_main

