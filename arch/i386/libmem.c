#include <string.h>
#include <libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libmem.h>
uint32_t allocFree();
void map_page(void *pntr,void * n);
void *malloc(unsigned long n){
	struct __malloc_mem *pntr = (struct __malloc_mem *)((uint32_t)4096*1024*20);
	if(!page_mapped(pntr)){
		map_page(pntr,pntr);
	}
	while(pntr->alloc == 1 || (pntr->size < n && pntr->size != 0)){
		if(!page_mapped(pntr)){
			map_page((void*)allocFree(),pntr);
		}	
		pntr=(struct __malloc_mem *)((uint8_t*)pntr+sizeof(*pntr)+pntr->size);
		continue;
		
	}
	pntr->size = n;
	pntr->alloc = 1;
	bzero(pntr + sizeof(*pntr),n);
	return (uint8_t*)pntr + sizeof(*pntr);
}
void free(void *pntr){
	struct __malloc_mem *_pntr = (struct __malloc_mem *)((uint8_t*)pntr-sizeof(struct __malloc_mem));
	if(_pntr->alloc != 1){
		panic("DOUBLE FREE EXCEPTION!\n");
	
	}
	_pntr->alloc = 0;
}
void bzero(void *addr,unsigned long n){
	for(unsigned long i = 0; i < n;i++)
		*(uint8_t*)(addr + n) = 0;
}
//uint32_t paged[1024] __attribute__ __attribute__((aligned(4096)));
#define __PRE_CALL(a,b) (a-0xc0000000)(b)
void  libmem_init(){
	uint32_t paged[1024] __attribute__((aligned(4096)));
	for(int i = 0; i < 1024;i++)
		paged[i] = 2;
	uint32_t paget[1024] __attribute__((aligned(4096)));;
	uint32_t base = 0x00000000;
	for(int i = 0;i < 1024;i++)
		paget[i] = (i*4096) | 3;
	base = 0x400000;
	uint32_t kpaget[1024] __attribute__((aligned(4096)));
	for(int i = 0; i < 1024;i++)
		kpaget[i] = base+(i*4096) | 3;
	paged[1] = (uint32_t)kpaget | 3;
	paged[0xC0400000/4096/1024] = (uint32_t) (kpaget) | 3;
	paged[0] = (uint32_t) (paget) | 3;
	uint32_t paget2[1024] __attribute__((aligned(4096)));
//	for(int i = 0; i < 1024;i++)
//		paget2[i] = (i * 4096 + 4096*1024*1) | 3;
//	paged[1] = (uint32_t) paget2 | 3;
	uint32_t addr = 0x3FC00000;
	uint32_t page255[1024] __attribute__((aligned(4096)));
	for(int i = 0; i < 1024;i++)
		page255[i] = (addr + i * 4096) | 3;
	paged[255] = (uint32_t)page255 | 3;
	paged[1023] = (uint32_t)paged | 3;
//	debug("libmem","Loading Page Directory...");
	init_page(paged);
//	map_page(0,(void*)0xC0000000);
};
void map_kernel(){
	libmem_init();	
}
void __pre_init(void *addr){
	libmem_init();
	map_page((void*)(2*1024*4096),addr);
}
int page_mapped(void *addr){
	unsigned long *pd = (unsigned long *)0xfffff000;
	return pd[(uint32_t)addr>>22] != 2;
}
void identp(void *_addr){
	map_page(_addr,_addr);
	return;
	uint32_t paget[1024]__attribute__((aligned(4096)));
	uint32_t addr = (uint32_t)_addr/4096/1024*4096*1024;
	for(int i = 0; i < 1024;i++)
		paget[i] = (i * 4096 + (uint32_t)addr) | 3;

//	paged[(uint32_t)addr/4096/1024] = (uint32_t)paget | 3;
}
uint32_t page_floor(uint32_t addr){
	return (addr/4096/1024)*4096*1024;//Integer division. Get rid of those pesky intermediate bits
}
uint32_t allocFree(){
	int i = 0;
	while(page_mapped((void*)i))
		i+=4096*1024;
	return i;
}
void *realloc(void *pntr,unsigned long n){
	void *newpntr = malloc(n);
	struct __malloc_mem *m = (struct __malloc_mem *)((uint8_t*)pntr-sizeof(struct __malloc_mem));
	memcpy(newpntr,pntr,n > m->size ? m->size : n);
	free(pntr);
	return newpntr;
}
void  init_page(uint32_t *pg){
	asm("mov %0,%%eax" : : "m"(pg));
        asm("mov %eax,%cr3");
        asm("mov %cr0,%eax");
        asm("or $0x80000001,%eax");
        asm("mov %eax,%cr0");
}
void map_page(void *paddr,void *vaddr){
	unsigned int pdindex = (unsigned int)vaddr >> 22;
	unsigned int ptindex = (unsigned int)vaddr >> 12 & 0x3ff;
	unsigned long *pd = (unsigned long *)0xfffff000;
	if(pd[pdindex] == 2){
		uint32_t paget[1024]__attribute__((aligned(4096)));
		for(int i = 0; i < 1024;i++)
			paget[i] = ((uint32_t)paddr/1024/4096*1024*4096 + i*4096) | 3;

		pd[pdindex] = (uint32_t)paget | 3;

	}
}
void unmap(uint32_t pdindex){
	((unsigned long *)0xfffff000)[pdindex] = 2;
}
