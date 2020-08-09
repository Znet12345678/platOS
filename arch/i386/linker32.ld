ENTRY(_start)

SECTIONS{
	. = 0xC0400000;
	code_begin = .;
	.start : AT(0x400000) { *(.start) }
	.multiboot : { 
		. = ALIGN(8);
		*(.multiboot )
	}
	.text : { *(.text) }
	.rodata : { *(.rodata) }
	.data : { *(.data) }
	.ptab : {
		ptBegin = .;
		*(.ptab)
			
	}
	code_end = .;
	
}
