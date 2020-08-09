#include <stdio.h>
#include <stdlib.h>
#include <libmem.h>
#include <inter.h>
#include <libio.h>
#include <stdint.h>
extern void breakpoint();
extern void page();
int errflg = 0;
int chkerr(){
	return errflg;
}
void exception(uint32_t err,uint32_t eip,uint32_t cs,uint32_t cflags){
	puts("\n");
	asm("add $4,%esp");
	for(int i = 0; i < 80;i++)
		putc('#');
	panic(eip);
}
void _exception(uint32_t eip,uint32_t cs,uint32_t cflags){
	puts("\n");
	asm("add $4,%esp");
	for(int i = 0; i < 80;i++)
		putc('#');
	panic(eip);
}
void stube(){
	asm("add $8,%esp");
	asm("jmp exception");
}
void divz(){
	puts("divz:");
	stube();
}
void debuge(){
	puts("debuge:");
	stube();
}
void nonmask(){
	puts("nonmask:");
	stube();
}void overflow(){
	puts("overflow:");
	stube();
}
void bre(){
	puts("bre:");
	stube();
}
void invalidopcode(){
	puts("invalid opcode:");
	stube();
}
void devnotavailable(){
	puts("Device Access Error\n");
	stube();
}
void cso(){
	puts("cso:");
	stube();
}
void itss(){
	puts("TSS Error\n");
	asm("jmp exception");
}
void doubleFault(){
	puts("!!!DOUBLE FAULT!!!\n");
	asm("jmp exception");
}
void segnotpres(){
	puts("!!!Segmentation Fault!!!\n");
	asm("jmp exception");
}
void stacksegfault(){
	puts("!!!Segmentation Fault!!!\n");
	asm("jmp exception");
}
void gpf(){
	puts("~~~GENERAL PROTECTION FAULT~~~\n");
	asm("jmp exception");
}
void floatingpoint(){
	puts("!!!Floating point exception!!!");
	asm("jmp exception");
}
uint32_t jmp_arr[] = {
	divz,
	debuge,
	nonmask,
	breakpoint,
	overflow,
	bre,
	invalidopcode,
	devnotavailable,
	doubleFault,
	cso,
	itss,
	segnotpres,
	stacksegfault,
	gpf,
	page,
	exception,
	floatingpoint,
	exception,
	exception,
	exception,
	exception
};
void init_int(){
	extern unsigned char exception_end[],exception_start[];
	int size;
//	putx(size);
	struct IDTDescr *arr = malloc(sizeof(*arr)*256);
//	size-=size2;
	for(int i = 0; i < 0x20;i++){
		arr[i].offset_1= (uint16_t)(jmp_arr[i] & 0xffff);
		arr[i].selector = 8;
		arr[i].zero = 0;
		arr[i].type_attr = 0b10001110;
		arr[i].offset_2 = (uint16_t)(jmp_arr[i] >> 16) & 0xffff;
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
