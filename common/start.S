.globl _start

.section .init
_start:
.option push
.option norelax
        lla     gp,__global_pointer$
.option pop
        lla     sp,_sp
        call    _reloc
        call    _zero_bss
        
        call    main
1:      j       1b

_zero_bss:
        lla     t0,_bss
        lla     t1,_ebss
        j       2f
1:      sd      x0,(t0)
        addi    t0,t0,8
2:      bne     t0,t1,1b
        ret

_reloc:
        lla     t0,_got
        lla     t1,_egot
        lla     t2,_start
        j       2f
1:      ld      t3,(t0)
        add     t3,t3,t2
        sd      t3,(t0)
        addi    t0,t0,8
2:      bne     t0,t1,1b
        ret
