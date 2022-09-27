OUTPUT_ARCH("riscv")

ENTRY(_start)

SECTIONS {
    PROVIDE(_init_start = .);

    .text : {
        KEEP(*(.text.init))
    }

    .data : {
        KEEP(*(.data.init))
    }

    PROVIDE(_init_end = ALIGN(4096));
    . = ALIGN(4096);

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
}
