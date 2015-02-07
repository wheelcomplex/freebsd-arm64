
#include <machine/asm.h>

.globl _start
_start:
	mov	r4, r2

	adr	r0, stack_end
	mov	sp, r0

	adr	r0, .Lbase
	ldr	r1, [r0]
	sub	r5, r0, r1

	ldr	r0, .Limagebase
	add	r0, r0, r5
	ldr	r1, .Ldynamic
	add	r1, r1, r5

	bl	_C_LABEL(_reloc)

	/* Zero the BSS, _reloc fixed the values for us */
	ldr	r0, .Lbss
	ldr	r1, .Lbssend
	mov	r2, #0

1:	cmp	r0, r1
	bgt	2f
	str	r2, [r0], #4
	b	1b

2:
	mov	r0, r4
	bl	_C_LABEL(main)

.globl _exit
_exit:
1:	WFI
	b	1b

.Lbase:
	.word	.
.Limagebase:
	.word	ImageBase
.Ldynamic:
	.word	_DYNAMIC
.Lbss:
	.word	__bss_start
.Lbssend:
	.word	_end

.align	3
stack:
	.space 512
stack_end:

