.globl _start

.section .text.init
_start:
        call    main
        j       _start
        
