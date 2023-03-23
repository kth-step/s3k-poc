.globl uartppp_bin
.globl uartppp_bin_len

.globl monitor_bin
.globl monitor_bin_len

.globl crypto_bin
.globl crypto_bin_len

.section .rodata.payloads

crypto_bin:
	.incbin "build/crypto.bin"
crypto_bin_len:
.quad . - crypto_bin

monitor_bin:
	.incbin "build/monitor.bin"
monitor_bin_len:
.quad . - monitor_bin

uartppp_bin:
	.incbin "build/uartppp.bin"
uartppp_bin_len:
.quad . - uartppp_bin
