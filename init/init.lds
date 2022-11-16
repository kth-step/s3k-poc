OUTPUT_ARCH("riscv")

ENTRY(_start)

SECTIONS {
    .text : {
        KEEP(*(.init))
        *(.text)
        *(.text.*)
    }

    _etext = .; PROVIDE(etext = .);
    .data : ALIGN(8) {
        __DATA_BEGIN__ = .;
        *(.rdata*)
        *(.rodata*)
        *(.data*)
    }
    .sdata : ALIGN(8) {
        __SDATA_BEGIN__ = .;
        *(.srdata*)
        *(.srodata*)
        *(.sdata*)
    }

    _edata = .; PROVIDE (edata = .);
    .sbss : ALIGN(8) {
        *(.sbss .sbss.*)
    } 

    .bss : ALIGN(8) {
        *(.bss .bss.*)
        . = ALIGN(8);
    }
    __BSS_END__ = .;
    _end = .; PROVIDE (end = .);

    __global_pointer$ = MIN(__SDATA_BEGIN__ + 0x800, MAX(__DATA_BEGIN__ + 0x800, __BSS_END__ - 0x800));

    .payload : {
        *(.payload*)
    }
}
