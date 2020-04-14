#include <libelf.h>
asm("jmp _start");
#include <stdint.h>
void memcpy(void *dest,const void *src,unsigned long n){
	for(int i = 0; i < n;i++)
		*((char*)dest + i) = *((char*)src+i);
}
void bzero(void *pntr,unsigned long n){
	for(unsigned long i = 0; i < n;i++)
		*((char*)pntr +i) = 0;
}
void put(char c){
	*(uint16_t*)0xb8000 = c;
}
int _start(){
	Elf32_Ehdr *hdr = (Elf32_Ehdr *)0x21400;
	Elf32_Phdr *phdr = (Elf32_Phdr *)(0x21400 + hdr->e_phoff);
	for(int i = 0; i < hdr->e_phnum;i++){
		if(phdr[i].p_type == PT_LOAD){
			bzero((void*)phdr[i].p_paddr,phdr[i].p_memsz);
			memcpy((void*)phdr[i].p_paddr,(uint8_t*)hdr +  phdr[i].p_offset,phdr[i].p_filesz);
		}
	}
	char *argv[] = {"/KERNEL32.BIN"};
	int (*main)(int argc,char *argv[]) = (int (*)(int argc,char *argv[]))hdr->e_entry-0xc0000000;
	return main(1,argv);
}
