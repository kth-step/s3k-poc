.globl _start

.section .text.init

_start:
	la	sp,_stack_top
	call	main
_loop:	j	_loop

.section .bss.stack

.skip 256
_stack_top:
