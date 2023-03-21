.globl uart_bin
.globl uart_bin_len
.globl app0_bin
.globl app0_bin_len
.globl app1_bin
.globl app1_bin_len

.section .rodata.payloads

uart_bin:
	.incbin "build/uart.bin"
uart_bin_len:
.quad . - uart_bin

app0_bin:
	.incbin "build/app0.bin"
app0_bin_len:
.quad . - app0_bin

app1_bin:
	.incbin "build/app1.bin"
app1_bin_len:
.quad . - app1_bin

