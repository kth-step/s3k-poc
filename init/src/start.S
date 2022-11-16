.globl _start

.section .init
_start:
.option push
.option norelax
        lla     gp,__global_pointer$
.option pop
        lla     sp,stack

        lla     t0,edata
        lla     t1,end
        j       2f
1:      sw      x0,(t0)
        addi    t0,t0,4
2:      bltu    t0,t1,1b
      
        call    main
1:      j       1b
        

.section .bss
   .fill 4096
stack:
