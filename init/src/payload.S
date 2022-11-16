.globl payload_uart
.section .payload, "a"
.balign 4096
payload_uart:
    .incbin "../uart/build/uart.bin"
