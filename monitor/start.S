.globl _start

.extern payloads

.globl free_space

.section .init
_start:
        la      sp,_sp
        la      t0,_bss
        la      t1,_ebss
        j       2f
1:      sd      x0,(t0)
        addi    t0,t0,8
2:      bne     t0,t1,1b

# Fix relocations
        la      a0,_start
        call    payload_relocation
2:
        la      ra,loop
        tail    setup

