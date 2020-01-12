ENTRY(_start)

SECTIONS{
	. = 0xC0001000;
	code_begin = .;
	.start : AT(0x1000) { *(.start) }
	.text : { 
		. = ALIGN(8);
		*(.multiboot )
		*(.text ) 
	}
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	kernel32_dev_first_node = .;
	code_end = .;
}
