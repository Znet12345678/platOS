#ifndef __LIBMEM_H
#define __LIBMEM_H
#include <stdint.h>
extern void *code_begin;
extern void *code_end;
void init_page(uint32_t *pg);
#define EBDA_BASE 0x000A0000
#define EBDA_END 0x000Bffff
#define BIOS_BASE 0x000E0000
#define BIOS_END 0x000FFFFF
#define KERNEL_BASE code_begin
#define MALLOC_BASE code_end
struct __malloc_mem{
	uint8_t alloc;
	uint32_t size;
}__attribute__((packed));
void *malloc(unsigned long n);
void free(void *mem);
uint32_t page_floor(uint32_t addr);
void libmem_init();
#endif