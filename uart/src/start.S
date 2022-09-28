.globl _start

.section .text.init
_start:
        mv      s0,ra
        call    main
        jalr    s0
        
