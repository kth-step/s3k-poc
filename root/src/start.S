.globl _start

.section .text.init
_start:
.option push
.option norelax
        la      gp,__global_pointer$
.option pop
        la      sp,stack

        la      t0,_bss
        la      t1,_ebss
        j       2f
1:      sw      x0,(t0)
        addi    t0,t0,4
2:      bltu    t0,t1,1b
      
        call    main
1:      j       1b
        

.section .bss
   .fill 4096 * 8
stack:
