ENTRY(_start)

MEMORY {
	RAM (rwx) : ORIGIN = 0x80010000, LENGTH = 64M
}

SECTIONS {
	.text : {
		KEEP(*(.text.init))
		*(.text.*)
	} > RAM

	.data : ALIGN(4K) {
		*(.data)
		*(.data.*)
		__global_pointer$ = .;
	} > RAM

	.bss : ALIGN(4K) {
		*(.bss)
		*(.bss.*)
	} > RAM
}
