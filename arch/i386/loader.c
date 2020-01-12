#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libelf.h>
void debug(const char *msg,const char *msg2){
	//Stub
}
void panic(void *msg){
	while(1);
}
void memcpy(void *dest,const void *src,unsigned long n){
	for(unsigned long i = 0; i < n; i++)
		((uint8_t*)dest)[i] = ((uint8_t*)src)[i];
}
int min(int a, int b){
	return a > b ? b:a;
}
int max(int a,int b){
	return a > b ? a:b;
}
extern int __pre_init(void *pntr);
int main(void *address){
	Elf32_Ehdr *hdr = (Elf32_Ehdr*)address;
	Elf32_Phdr *phdr = (Elf32_Phdr*)((uint8_t*)address + hdr->e_phoff);
	uint32_t base;
	for(int i = 0; i < hdr->e_phnum;i++){
		base = min(phdr[i].p_vaddr,base);
		if(phdr[i].p_type == PT_LOAD)
			memcpy((void*)phdr[i].p_vaddr,address + phdr[i].p_offset,max(phdr[i].p_filesz,phdr[i].p_memsz));
	}
	void *basepntr = (void*)base;
	uint32_t entry = hdr->e_entry;
	__pre_init(basepntr);
	((void(*)())entry)();
	while(1);
}
