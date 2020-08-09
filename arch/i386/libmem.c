#include <KMEM.h>
#include <string.h>
#include <libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libmem.h>
uint32_t paddrArray[1024] = {1,1,1,[3 ... 1023]0};
uint32_t allocFree();
void *ptabAlloc();
int map_page(void *pntr,void * n);
struct __malloc_mem *head = (struct __malloc_mem *)MALLOC_BASE;
void *malloc(size_t n){
	if(n == 0)
		return 0;
	void *base = (void*)head;
	if(!page_mapped(base)){
		puts("MALLOC INIT\n");
		map_page((void*)allocFree(),(void*)MALLOC_BASE);
	}void *pntr = base;
	struct __malloc_mem *mem = (struct __malloc_mem *)base;
	if(mem->head == 0){
		mem->head = base+sizeof(*mem);
		mem->size = 4096*1024;
		mem->nxt = 0;
	}
	while(mem->nxt != 0 && mem->size < n+5){
		if(mem->nxt->size == 0)
			mem->nxt = mem->nxt->nxt;
		mem = mem->nxt;

	}
	void *ret = mem->head;
	*(uint8_t*)ret = 1;
	*(uint32_t*)((uint8_t*)ret + 1) = n;
	mem->size-=n+5;
	mem->head+=n+5;
	if(!page_mapped(ret+n)){
		puts("ALLOCATED PAGE FOR MALLOC\n");
		map_page((void*)allocFree(),ret+n);
	}
	bzero(ret+5,n);
	return ret+5;
}
void free(void *pntr){
	struct __malloc_mem *new = malloc(sizeof(*new));
	memcpy(new,head,sizeof(*new));
	if(*(uint8_t*)(pntr - 5) != 1){
		_panic("DOUBLE FREE");
	}
	head->head = pntr;
	head->size = *(uint32_t*)((uint8_t*)pntr-4);
	head->nxt = new;
	*(uint8_t*)(pntr - 5) = 0;
}

void bzero(void *addr,unsigned long n){
	for(unsigned long i = 0; i < n;i++)
		*(uint8_t*)(addr + i) = 0;
}
//uint32_t paged[1024] __attribute__ __attribute__((aligned(4096)));
#define __PRE_CALL(a,b) (a-0xc0000000)(b)
void  libmem_init(){
	uint32_t *paged __attribute__((aligned(4096))) = 0x800000;
	for(int i = 0; i < 1024;i++)
		paged[i] = 2;
	uint32_t *paget __attribute__((aligned(4096))) = 0x801000;
	for(int i = 0; i < 3072;i++)
		paget[i] = i*4096 | 3;
	paged[0] = 0x801000 | 3;
	paged[1] = 0x802000 | 3;
	paged[2] = 0x803000 | 3;
	paged[0xc0400000/4096/1024] = 0x802000 | 3;
	paged[1023] = (uint32_t)paged | 3;
	init_page(paged);
}
void map_kernel(){
	libmem_init();	
}
void __pre_init(void *addr){
	libmem_init();
	map_page((void*)(2*1024*4096),addr);
}
int abs(int i){
	return i < 0 ? i * -1 : i;
}
int page_mapped(void *addr){
	unsigned long *pd = (unsigned long *)0xfffff000;
 	uint32_t pt = pd[(uint32_t)addr>>22];
	uint32_t *pat = ((unsigned long *)0xFFC00000) + (0x400*((uint32_t)addr>>22));
	if(pt == 2)
		return 0;
	if(pat[(unsigned int)addr >> 12 & 0x3ff] & 3)
		return 1;
	else
		return 0;
}
void identp(void *_addr){
	map_page((uint32_t)_addr & ~(4096*1024-1),(uint32_t)_addr & ~(4096*1024-1));
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
int page_free_p(uint32_t p);

void *realloc(void *pntr,unsigned long n){
	void *newpntr = malloc(n);
	size_t size = *(uint32_t*)((uint8_t*)pntr-4);
	if(size > n)
		memcpy(newpntr,pntr,n);
	else
		memcpy(newpntr,pntr,size);
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
extern void *code_end;
void *ptabAlloc(){
	void *pntr = (void*)0x800000;
	while((uint32_t)pntr % 4096)pntr++;
	while(*(uint8_t*)pntr)
		pntr+=4096;
	return pntr;
}
int map_page(void* paddr,void* vaddr){
	unsigned int pdindex = (unsigned int)vaddr >> 22;
	unsigned int ptindex = (unsigned int)vaddr >> 12 & 0x3ff;
	unsigned long *pd = (unsigned long *)0xfffff000;
	if(pd[pdindex] == 2){
		uint32_t *pt = ptabAlloc();
		for(int i = 0; i < 1024;i++)
			pt[i] = ((uint32_t)paddr) + i*4096 | 3;
		paddrArray[(uint32_t)paddr/1024/4096] = 1;
		pd[pdindex] = (uint32_t)pt | 3;
		return 1;
	}
	unsigned long * pt = ((unsigned long *)0xFFC00000) + (0x400 * pdindex);
	if(pt[ptindex] != 2)
		return 0;
	pt[ptindex] = (uint32_t)paddr | 3;
	return 1;
}
int inTab(int a){
	for(int i = 0; i < 1024;i++)
		if(a == paddrArray[i])
			return 1;
	return 0;
}
uint32_t allocFree(){
	for(int i = 3; i < 1024;i++)
		if(!paddrArray[i])
		{
			puts("allocFree():");
			putx(i*4096*1024);
			return i * 4096 * 1024;
		}
}
void unmap(uint32_t pdindex){
	((unsigned long *)0xfffff000)[pdindex] = 2;
	paddrArray[pdindex] = 0;
}
