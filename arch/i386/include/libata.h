#ifndef __LIBATA_H
#define __LIBATA_H
#include <stdint.h>
#define LASTVAL 0x1f000
#include <libio.h>
ata_dev_t ** libata_init();
uint8_t ident(uint16_t port,uint8_t slave);
int ata_read48(uint16_t port,uint8_t slave,void *buf,unsigned long lba,unsigned short sc);
int ata_write48(uint16_t port,uint8_t slave,void *buf,unsigned long lba,unsigned short sc);
#define READ_OFFSET 0
#define ERROR_OFFSET 1
#define FEATURES_OFFSET 2
#define SECTORCOUNT_OFFSET 3
#define LBALO_OFFSET 4
#define LBAMID_OFFSET 5
#define LBAHI_OFFSET 6
#define STATUS_OFFSET 7
#define COMMAND_OFFSET 7
#endif
