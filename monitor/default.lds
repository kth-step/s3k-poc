OUTPUT_ARCH("riscv")

ENTRY(_start)

SECTIONS {
    .text : {
        KEEP(*(.text.init))
        *(.text)
        *(.text.*)
    }
    .data : {
        PROVIDE(__global_pointer$ = . + 0x7bc);
        *(.data)
        *(.data.*)
        *(.sdata)
        *(.srodata)
    }
}
