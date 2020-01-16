ENTRY(_start)

SECTIONS{
	. = 0xC0400000;
	code_begin = .;
	.start : AT(0x400000) { *(.start) }
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
