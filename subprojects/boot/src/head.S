.globl _start

.section .text.init

_start:
	.option push
	.option norelax
	la	gp,__global_pointer$
	.option pop
	la	sp,_stack_top
	call	main
_loop:	j	_loop

.section .bss.stack

.skip 256
_stack_top:
