#ifndef __STRING_H
#define __STRING_H
int memcmp(const void *a,const void *b,unsigned long n);
void memcpy(void *dest,const void *src,unsigned long n);
void bzero(void *buf,unsigned long n);
int strcmp(const char *str,const char *s);
//int memcpy(void *d,const void *s,unsigned long n);
#endif
