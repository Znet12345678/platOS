ENTRY(_start)
SECTIONS{
	. = 0x10000;
	code_begin = .;
	.text : { 
		. = ALIGN(8);
		KEEP(*(.multiboot))
		*(.text) 
	}

	. = 0x15000;
	.data : { *(.data) }
	. = 0x16000;
	.rodata : { *(.rodata) }
	. = 0x1f000;
	kernel32_dev_first_node = .;
	.devnode32 : { *(.devnode32) }
	code_end = .;
}
