.globl uart_bin
.globl uart_bin_len
.globl app0_bin
.globl app0_bin_len
.globl app1_bin
.globl app1_bin_len

.section .rodata.payloads


uart_bin:
	.incbin "../uart/uart.bin"
uart_bin_len: 
	.long . - uart_bin

app0_bin:
	.incbin "../app0/app0.bin"
app0_bin_len:
	.word . - app0_bin


app1_bin:
	.incbin "../app1/app1.bin"
app1_bin_len:
	.word . - app1_bin

