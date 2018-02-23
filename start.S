#include <config.h>
#include <asm/link.h>
#include <asm/common.h>

.global _start
_start:
	push {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}

#if 0
	/* print DRAM */
        ldr r10, ='D'
        STREG(RF_GRP(18, 0), r10);
        ldr r10, ='R'
        STREG(RF_GRP(18, 0), r10);
        ldr r10, ='A'
        STREG(RF_GRP(18, 0), r10);
        ldr r10, ='M'
        STREG(RF_GRP(18, 0), r10);
        ldr r10, ='\n'
        STREG(RF_GRP(18, 0), r10);
        ldr r10, ='\r'
        STREG(RF_GRP(18, 0), r10);
#endif

#if 0 // Set dram size (8700)
    LDREG(RF_GRP(1, 7 ), r5);
    and r5, r5, #~(0xF << 21)
    orr r5, r5, #(0xA << 21);
    STREG(RF_GRP(1, 7 ), r5)
#endif

    // sp has been setup by xboot

/*
 * Zero BSS
 */
    ldr r0, =__bss_start
    ldr r1, =__bss_end__
    mov r2, #0x00000000
clbss_l:
    cmp r0, r1          /* clear loop... */
    bhs clbss_e         /* if reached end of bss, exit */
    str r2, [r0]
    add r0, r0, #4
    b   clbss_l
clbss_e:
	blx dram_init_main

	//bl pwr_ctrl

	pop {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, lr}
	bx	lr
	nop
	nop
	nop
	nop
	nop
