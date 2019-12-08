#include <stdio.h>
#include <stdlib.h>
#include <libmem.h>
#include <inter.h>
#include <libio.h>
#include <stdint.h>
void exception(int i){
	if(i == 0xe){
		uint32_t addr;
		asm("mov %cr2,%eax");
		asm("mov %%eax,%0" : "=m"(addr));
		puts("Page fault at address 0x");
		putx(addr);
		puts("\n");

	}
	if(i != 0xe)
		panic("Exception Handler Not Implemented");
}
asm("c:");
void _exception(){
asm("a:");
	exception(0);
	asm("iret");
asm("b:");
	exception(1);
	asm("iret");
	exception(2);
	asm("iret");
	exception(3);
	asm("iret");
	exception(4);
	asm("iret");
	exception(5);
	asm("iret");
	exception(6);
	asm("iret");
	exception(7);
	asm("iret");
	exception(8);
	asm("iret");
	exception(9);
	asm("iret");
	exception(0xa);
	asm("iret");
	exception(0xb);
	asm("iret");
	exception(0xc);
	asm("iret");
	exception(0xd);
	asm("iret");
	exception(0xe);
	asm("iret");
	exception(0xf);
	asm("iret");
	exception(0x10);
	asm("iret");
	exception(0x11);
	asm("iret");
	exception(0x12);
	asm("iret");
	exception(0x13);
	asm("iret");
	exception(0x14);
	asm("iret");
	exception(0x15);
	asm("iret");
	exception(0x16);
	asm("iret");
	exception(0x17);
	asm("iret");
	exception(0x18);
	asm("iret");
	exception(0x19);
	asm("iret");
	exception(0x1a);
	asm("iret");
	exception(0x1b);
	asm("iret");
	exception(0x1c);
	asm("iret");
	exception(0x1d);
	asm("iret");
	exception(0x1e);
	asm("iret");
	exception(0x1f);
	asm("iret");
	exception(0x20);
	asm("iret");
}
void init_int(){
	extern unsigned char exception_end[],exception_start[];
	int size;
	asm("movl $(b-a),%0":"=m"(size));
	int off;
	asm("movl $(a-c),%0":"=m"(off));
//	putx(size);
	struct IDTDescr *arr = malloc(sizeof(*arr)*256);
//	size-=size2;
	for(int i = 0; i < 0x20;i++){
		arr[i].offset_1= (uint16_t)(((uint32_t)_exception + off + size*i) & 0xffff);
		arr[i].selector = 8;
		arr[i].zero = 0;
		arr[i].type_attr = 0b10001110;
		arr[i].offset_2 = (uint16_t)((((uint32_t)_exception +off + size*i) >> 16) & 0xffff);
	}
	for(int i = 0x20;i < 0x100;i++){
		arr[i].selector = 8;
		arr[i].zero = 0;
		arr[i].type_attr = 0b10001110;
	}
	struct idtr idtd;
//	putx(idtd);
//	puts("\n");
//	putx(arr);
	idtd.limit = 0x100*sizeof(struct IDTDescr)-1;
	idtd.base = (uint32_t)arr;
	asm("lidt %0" : :"m"(idtd));
}
