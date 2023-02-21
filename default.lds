OUTPUT_ARCH("riscv")

ENTRY(_start)

SECTIONS {
        .text : {
                _text = .;
                *(.init)
                *(.text .text.*)
                _etext = .;
        }

        .data : {
                _data = .;
                *(.data .data.*)
                *(.rodata .rodata.*)
                _sdata = .;
                *(.sdata .sdata.*)
                *(.srodata .srodata*)
                _edata = .;
        }

        .bss : ALIGN(8) {
                _bss = .;
                *(.sbss .sbss.*)
                *(.bss .bss.*)
                _ebss = ALIGN(8);
        }

	.stack : ALIGN(8) {
		*(.stack .stack.*)
	}

	.noinit : {
		_noinit = .;
                *(.noinit .noinit.*)
                _enoinit = .;
	}
	_end = .;

        __global_pointer$ = MIN(_sdata + 0x800, MAX(_data + 0x800, _ebss - 0x800));

}
