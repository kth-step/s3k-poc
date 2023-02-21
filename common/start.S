.globl _start
.type _start, @function

.section .init
_start:

.option push
.option norelax
	lla	gp,__global_pointer$
.option pop
	lla	sp,_stack_top

	lla	t0,_edata
	lla	t1,_end
	j	2f
1:	sb	zero,(t0)
	addi	t0,t0,1
2:	bne	t0,t1,1b



	call	setup
	lla	ra,loop
	tail	loop

.section .stack
_stack:
.fill 2048
_stack_top:
