#ifndef __STRING_H
#define __STRING_H
#include <stdlib.h>
#include <types.h>
char *strchrnul (const char *s, int c_in);
unsigned long strlen(const char *str);
void strcpy(char *dest,const char *src);
size_t strspn(const char *s, const char *accept);
int strncmp(const char *s1,const char *s2,size_t n);
size_t strcspn(const char *s, const char *reject);
void* memset(void *pntr,char c,size_t n);
int memcmp(const void *a,const void *b,unsigned long n);
void memcpy(void *dest,const void *src,unsigned long n);
void bzero(void *buf,unsigned long n);
int strcmp(const char *str,const char *s);
//int memcpy(void *d,const void *s,unsigned long n);
#endif
