#include <stdio.h>
#include <stdlib.h>
#include <libmem.h>
#include <inter.h>
#include <libio.h>
#include <stdint.h>
int errflg = 0;
int chkerr(){
	return errflg;
}
void exception(int i,uint32_t eip){
	panic(eip);
}
asm("c:");
void _exception(uint32_t err,uint32_t eip,uint32_t cs,uint32_t eflags){
	asm("add $4,%ebp");
asm("a:");
	puts("[Exception]Divide by zero\n");
	exception(0,eip+err);
	asm("iret");
asm("b:");
	puts("[Exception]Debug\n");
	exception(1,eip+cs);
	asm("iret");
	puts("[Exception]Non-maskable Interrupt\n");
	exception(2,eip+err);
	asm("iret");
	puts("[Exception]Breakpoint\n");
	exception(3,eip+err);
	asm("iret");
	puts("[Exception]Overflow\n");
	exception(4,eip+err);
	asm("iret");
	puts("[Exception]Bound Range Exceeded\n");
	exception(5,eip+err);
	asm("iret");
	puts("[Exception]Invalid Opcode\n");
	exception(6,eip+err);
	asm("iret");
	puts("[Exception]Device Not Available\n");
	exception(7,eip+err);
	asm("iret");
	puts("[Exception]Double fault\n");
	exception(8,eip+err);
	asm("iret");
	puts("[Exception]Coprocessor Segment Overrun\n");
	exception(9,eip+cs);
	asm("iret");
	puts("[Exception]Invalid TSS\n");
	exception(0xa,eip+cs);
	asm("iret");
	puts("[Exception]Segment Not Present\n");
	exception(0xb,eip+cs);
	asm("iret");
	puts("[Exception]Stack-Segment Fault\n");
	exception(0xc,eip+cs);
	asm("iret");
	puts("[Exception]General Protection Fault\n");
	exception(0xd,eip+cs);
	asm("iret");
	puts("[Exception]Page fault\n");
	exception(0xe,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0xf,eip+cs);
	asm("iret");
	puts("[Exception]x87 Floating-Point Exception\n");
	exception(0x10,eip+cs);
	asm("iret");
	puts("[Exception]Alignment Check\n");
	exception(0x11,eip+err);
	asm("iret");
	puts("[Exception]Machine Check\n");
	exception(0x12,eip+err);
	asm("iret");
	puts("[Exception]SIMD Floating-Point Exception\n");
	exception(0x13,eip+cs);
	asm("iret");
	puts("[Exception]Virtualization Exception\n");
	exception(0x14,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x15,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x16,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x17,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x18,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x19,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x1a,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x1b,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x1c,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x1d,eip+cs);
	asm("iret");
	puts("[Exception]Security\n");
	exception(0x1e,eip+cs);
	asm("iret");
	puts("[Exception]Reserved\n");
	exception(0x1f,eip+cs);
	asm("iret");
	puts("[Exception]Exception Handler\n");
	exception(0x20,eip+cs);
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
		arr[i].offset_1= (uint16_t)(((uint32_t)_exception + size*i + off) & 0xffff);
		arr[i].selector = 8;
		arr[i].zero = 0;
		arr[i].type_attr = 0b10001110;
		arr[i].offset_2 = (uint16_t)((((uint32_t)_exception +size*i + off) >> 16) & 0xffff);
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
