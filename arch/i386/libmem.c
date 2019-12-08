#include <string.h>
#include <libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libmem.h>
void *malloc(unsigned long n){
	struct __malloc_mem *pntr = (struct __malloc_mem *)MALLOC_BASE;
	while(pntr->alloc == 1 || pntr->size < n){
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
uint32_t paged[1024] __attribute__((aligned(4096)));
void libmem_init(){
	debug("libmem","zeroing lowmem");
	bzero(0,0xffff);
	for(int i = 0; i < 1024;i++)
		paged[i] = 2;
	uint32_t paget[1024] __attribute__((aligned(4096)));;
	for(int i = 0;i < 1024;i++)
		paget[i] = (i*4096) | 3;
	paged[0] = (uint32_t) paget | 3;
	uint32_t paget2[1024] __attribute__((aligned(4096)));
	for(int i = 0; i < 1024;i++)
		paget2[i] = (i * 4096 + 4096*1024*1) | 3;
	paged[1] = (uint32_t) paget2 | 3;
	uint32_t addr = 0x3FC00000;
	uint32_t page255[1024] __attribute__((aligned(4096)));
	for(int i = 0; i < 1024;i++)
		page255[i] = (addr + i * 4096) | 3;
	paged[255] = (uint32_t)page255 | 3;
	paged[1023] = (uint32_t)&paged[1023] | 3;
	debug("libmem","Loading Page Directory...");
	init_page(paged);
}
int page_mapped(void *addr){
	return paged[(uint32_t)addr/4096/1024] != 2;
}
void identp(void *_addr){
	uint32_t paget[1024]__attribute__((aligned(4096)));
	uint32_t addr = (uint32_t)_addr/4096/1024*4096*1024;
	for(int i = 0; i < 1024;i++)
		paget[i] = (i * 4096 + (uint32_t)addr) | 3;

	paged[(uint32_t)addr/4096/1024] = (uint32_t)paget | 3;
}
uint32_t page_floor(uint32_t addr){
	return (addr/4096/1024)*4096*1024;//Integer division. Get rid of those pesky intermediate bits
}
void *realloc(void *pntr,unsigned long n){
	void *newpntr = malloc(n);
	struct __malloc_mem *m = (struct __malloc_mem *)((uint8_t*)pntr-sizeof(struct __malloc_mem));
	memcpy(newpntr,pntr,n > m->size ? m->size : n);
	free(pntr);
	return newpntr;
}
void init_page(uint32_t *pg){
	asm("mov %0,%%eax" : : "m"(pg));
        asm("mov %eax,%cr3");
        asm("mov %cr0,%eax");
        asm("or $0x80000001,%eax");
        asm("mov %eax,%cr0");
}

