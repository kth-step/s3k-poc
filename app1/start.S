.globl _start

.section .init
_start:
        la      sp,_sp

        la      t0,_bss
        la      t1,_ebss
        j       2f
1:      sw      x0,(t0)
        addi    t0,t0,4
2:      bltu    t0,t1,1b
        la      ra,loop
        tail    setup
        
