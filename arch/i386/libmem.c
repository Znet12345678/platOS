#include <KMEM.h>
#include <string.h>
#include <libio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <libmem.h>
uint32_t allocFree();
int map_page(void *pntr,void * n);
void writeMTab(void *pntr,unsigned long size){
	struct __malloc_mem *_pntr = pntr;
	_pntr->size= size;
	_pntr->alloc = 0;
#ifdef MEM_DEBUG
	puts("memtab ");
	putx((uint32_t)pntr);
	puts(" ");
	putx(size);
	puts("\n");
#endif
}
void *malloc(unsigned long n){
#ifdef MEM_DEBUG
	puts("malloc(");
	puti(n);
	puts(")->");
#endif
	struct __malloc_mem *svpntr = (struct __malloc_mem *)MALLOC_BASE;
	struct __malloc_mem *pntr = (struct __malloc_mem *)MALLOC_BASE;
	if(!page_mapped(pntr)){
		puts("(0x");
		putx(allocFree());
		puts(",");
		putx((uint32_t)pntr);
		puts(")\n");
		int r = map_page((void*)allocFree(),pntr);
		if(r)
			bzero(pntr,4096*1024);
		else
			_panic("failed to allocate page\n");
	}
	if(n == 0)
		return 0;
	while(1){
		if(pntr->alloc != 0 && pntr->alloc != 1){
			puts("0x");
			putx((uint32_t)pntr);
			puts(":");
			putx(pntr->alloc);
			puts(":");
			putx(pntr->size);
			puts("\n");
			_panic("malloc validity check failed\n");
					
		}
		if(!page_mapped(pntr)){
			puts("(0x");
			putx(allocFree());
			puts(",0x");
			putx((uint32_t)pntr);
			puts(")\n");
			map_page((void*)allocFree(),pntr);
		}	
		if(pntr->alloc == 0 && (pntr->size >= n) || (pntr->alloc == 0 &&pntr->size == 0))
			break;
		svpntr = pntr;
		pntr=(struct __malloc_mem *)((uint8_t*)pntr+sizeof(*pntr)+pntr->size);
		continue;
		
	}
	int orig = pntr->size;
	pntr->size = n;
	pntr->alloc = 1;
	if(orig > 0){
		writeMTab((uint8_t*)pntr+n+sizeof(*pntr),orig-n-sizeof(*pntr));
	}
#ifdef MEM_DEBUG
	putx((uint32_t)((uint8_t*)pntr + sizeof(*pntr)));
	puts("\n");
#endif
	return (uint8_t*)pntr + sizeof(*pntr);
}
void free(void *pntr){
#ifdef MEM_DEBUG
	puts("free(0x");
	putx((uint32_t)pntr);
	puts(")\n");
#endif
	struct __malloc_mem *_pntr = (struct __malloc_mem *)((uint8_t*)pntr-sizeof(*_pntr));
	if(_pntr->alloc != 1){
		puts("\nBase address:0x");
		putx((uint32_t)pntr);
		puts("\nAlloc status:");
		puti(_pntr->alloc);
		_panic("\nDOUBLE FREE EXCEPTION!\n");
	
	}
	bzero((uint8_t*)pntr,*(uint32_t*)((uint8_t*)pntr-sizeof(*_pntr)+1));
	_pntr->alloc = 0;
}
void bzero(void *addr,unsigned long n){
	for(unsigned long i = 0; i < n;i++)
		*(uint8_t*)(addr + i) = 0;
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
	base = 0x000000;
	uint32_t kpaget[1024] __attribute__((aligned(4096)));
	for(int i = 0; i < 1024;i++)
		kpaget[i] = base+(i*4096) | 3;
	paged[0] = (uint32_t)kpaget | 3;
	paged[0xC0010000/4096/1024] = (uint32_t) (kpaget) | 3;
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
int page_free_p(uint32_t p);

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
static uint32_t paddrArr[1024] = { [0 ... 1023]0};
int map_page(void *paddr,void *vaddr){
	unsigned int pdindex = (unsigned int)vaddr >> 22;
	unsigned int ptindex = (unsigned int)vaddr >> 12 & 0x3ff;
	unsigned long *pd = (unsigned long *)0xfffff000;
	if(pd[pdindex] == 2){
		uint32_t paget[1024]__attribute__((aligned(4096)));
		for(int i = 0; i < 1024;i++)
			paget[i] = (uint32_t)paddr/1024/4096*1024*4096 + i *4096 | 3;
		pd[pdindex] = (uint32_t)paget | 3;
			int i = 0;
		while(paddrArr[i] != 0)
			i++;
		paddrArr[i] = (uint32_t)paddr;
		return 1;
	}else{
		uint32_t *paget = (uint32_t*)(0xffc00000) + (0x400*pdindex);
		paget[ptindex] = ((uint32_t)paddr) | 3;
		return 1;
	}

}
int inTab(int a){
	for(int i = 0; i < 1024;i++)
		if(a == paddrArr[i])
			return 1;
	return 0;
}
uint32_t allocFree(){
	uint32_t t = 5;
	uint32_t j = (t - 1) << 22;
	while(inTab(j)){
		j=(t-1) << 22;
		t++;
	}
	return j;
}
void unmap(uint32_t pdindex){
	((unsigned long *)0xfffff000)[pdindex] = 2;
}
