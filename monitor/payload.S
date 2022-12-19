.globl uart_bin_begin
.globl uart_bin_end

.globl app0_bin_begin
.globl app0_bin_end

.globl app1_bin_begin
.globl app1_bin_end

.section .text.rodata
uart_bin_begin:
.incbin "build/uart.bin"
uart_bin_end:

app0_bin_begin:
.incbin "build/app0.bin"
app0_bin_end:

app1_bin_begin:
.incbin "build/app1.bin"
app1_bin_end:
