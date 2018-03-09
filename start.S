#include <config.h>
#include <asm/common.h>

#ifdef DRAMSCAN
/* skip 64 bytes */
.rept		16
.long		0
.endr

/*
 * Magic number for SPI baremetal
 */
.word 0x6D622B52
#endif

.global _start
_start:
#ifdef DRAM_INIT
	/* Use stack setup by xboot */
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
#elif (defined(DRAMSCAN) || defined(SISCOPE))
	/* setup stack */
	ldr	sp, =_sp_start
	bic	sp, sp, #7	/* for 8-byte alignment */
#endif

/*
 * Zero BSS
 */
	ldr	r0, =__bss_start
	ldr	r1, =__bss_end__
	mov	r2, #0x00000000
clbss_l:
	cmp	r0, r1          /* clear loop... */
	bhs	clbss_e         /* if reached end of bss, exit */
	str	r2, [r0]
	add	r0, r0, #4
	b	clbss_l
clbss_e:
	blx	dram_init_main

#ifdef DRAM_INIT
	/* return back to xboot */
	pop	{r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
	bx	lr
	nop
	nop
	nop
	nop
	nop
#elif (defined(DRAMSCAN) || defined(SISCOPE))
die_here:
	b	die_here
#endif
