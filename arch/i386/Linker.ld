SECTIONS{
	. = 0x1000;
	.text : { *(.text) }
	. = 0x2000;
	.data : { *(.data) }
	. = 0x2200;
	.isr : { *(.isr) }
	. = 0x2300;
	.kern : {*(.kern) }
}
