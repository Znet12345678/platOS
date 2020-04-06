ENTRY(_start)

SECTIONS{
	. = 0xC0040000;
	code_begin = .;
	.start : AT(0x40000) { *(.start) }
	.multiboot : { 
		. = ALIGN(8);
		*(.multiboot )
	}
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }

	code_end = .;
}
