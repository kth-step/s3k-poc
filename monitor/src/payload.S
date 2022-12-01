.globl payloads
.globl payload_load
.globl payload_relocation

.section .text.payload

# payload_load(src, end, dest)
payload_load:
1:      ld      t0,0(a0)
        sd      t0,0(a2)
        addi    a0,a0,8
        addi    a2,a2,8
        bne     a0,a1,1b
        ret
        
payload_relocation:
        la      t0,payloads
        la      t1,payloads_end
1:      ld      t2,(t0)
        ld      t3,8(t0)
        add     t2,t2,a0
        add     t3,t3,a0
        sd      t2,(t0)
        sd      t3,8(t0)
        addi    t0,t0,16
        bne     t0,t1,1b
2:      ret


.section .data.payload
payloads:
.quad _uart_in, _uart_out
.quad _uart_out, _app0
.quad _app0, _app1
.quad _app1, _end
payloads_end:

.balign 8
_uart_in:
.incbin "../uart_in/uart_in.bin"
.balign 8
_uart_out:
.incbin "../uart_out/uart_out.bin"
.balign 8
_app0:
.incbin "../app0/app0.bin"
.balign 8
_app1:
.incbin "../app1/app1.bin"
.balign 8
_end:
