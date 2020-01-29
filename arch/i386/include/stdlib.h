#ifndef __STDLIB_H
#define __STDLIB_H
#define NULL 0
void *malloc(unsigned long n);
int page_mapped(void *addr);
void identp(void *addr);
void *realloc(void *pntr,unsigned long size);
void init_int();
void panic(void *msg);
#endif
