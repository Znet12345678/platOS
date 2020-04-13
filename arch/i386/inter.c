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
void exception(uint32_t err,uint32_t eip,uint32_t cs,uint32_t cflags){
	puts("\n");
	asm("sub $4,%bp");
	for(int i = 0; i < 80;i++)
		putc('#');
	panic(eip);
}
uint32_t jmp_arr[] = {[0 ... 0x20](uint32_t)exception};
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
