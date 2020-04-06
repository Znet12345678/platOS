#ifndef __STDLIB_H
#define __STDLIB_H
#define NULL 0
#include <libmem.h>
#include <stdint.h>
void panic(uint32_t eip);
void *malloc(unsigned long n);
int page_mapped(void *addr);
void identp(void *addr);
void *realloc(void *pntr,unsigned long size);
void init_int();
void panic();
void _panic(const char *msg);
#endif
