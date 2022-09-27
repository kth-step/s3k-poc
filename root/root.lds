OUTPUT_ARCH("riscv")

ENTRY(_start)

SECTIONS {
    PROVIDE(__init_start__ = .);
    .text : {
        KEEP(*(.text.init))
    }

    .data : {
        KEEP(*(.data.init))
        . = ALIGN(4096);
    }
    PROVIDE(__init_end__ = .);

    .text : {
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

    ASSERT(__init_start__ + 4096 == __init_end__, "Error: Init has wrong size")
}
