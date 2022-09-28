.globl payload_uart
.section .data.payload.uart
.balign 4096
payload_uart:
    .incbin "../uart/build/uart.bin"
