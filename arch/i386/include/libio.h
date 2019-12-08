#ifndef __LIBIO_H
#define __LIBIO_H
#include <stdint.h>
#include <libio.h>
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
  
}
static inline void outw(uint16_t port, uint16_t val)
{
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}
static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile ( "inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}
unsigned long strlen(const char *str);
void memcpy(void *dest,const void *src,unsigned long n);
void strcpy(char *dest,const char *src);
void puts(const char *str);
void putc(uint8_t c);
void puti(int i);
void debug(const char *task,const char *msg);
typedef struct ata_dev{
	uint16_t ioaddr;
	uint8_t slavebyte;	
}ata_dev_t;
#include <libata.h>
#endif
