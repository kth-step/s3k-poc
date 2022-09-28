OUTPUT_ARCH("riscv")

ENTRY(_start)

SECTIONS {
    .text : {
        KEEP(*(.text.init))
        *(.text)
        *(.text.*)
    }
    
    .rodata : ALIGN(4) {
        PROVIDE(__global_pointer$ = . + 0x7bc);
        *(.srdata)
        *(.srdata.*)
        *(.srodata)
        *(.srodata.*)
        *(.rdata)
        *(.rdata.*)
        *(.rodata)
        *(.rodata.*)
        *(.sdata)
        *(.sdata.*)
        *(.data)
        *(.data.*)
    }

    .bss : ALIGN(4) {
        PROVIDE(_bss = .);
        *(.sbss)
        *(.sbss.*)
        *(.bss)
        *(.bss.*)
        *(COMMON)
        . = ALIGN(4);
        PROVIDE(_ebss = .);
    }

    .data : {
        *(.data.payload.*)
    }
}
