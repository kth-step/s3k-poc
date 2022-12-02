OUTPUT_ARCH("riscv")

ENTRY(_start)

SECTIONS {
    .text : ALIGN(4K) {
        _text = .;
        *(.init)
        *(.text)
        *(.text.*)
        _etext = .;
    }

    _etext = .; PROVIDE(etext = .);
    .data : ALIGN(4K) {
        _data = .;
        *(.data*)
        *(.rodata .rodata.*)
        *(COMMON)
        _sdata = .;
        *(.sdata .sdata.*)
        *(.srodata .srodata*)
        . = ALIGN(8);
        _edata = .;
    }

    .bss : ALIGN(8) {
        _bss = .;
        _sbss = .;
        *(.sbss) 
        *(.sbss.*)
        *(.bss) 
        *(.bss.*)
        . = ALIGN(8);
        _ebss = .;
    }

    . += 0x800;
    _sp = .;

    free_space = ALIGN(16K);

    __global_pointer$ = MIN(_sdata + 0x800, MAX(_data + 0x800, _ebss - 0x800));
}
