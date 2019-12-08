/*
 * FAT File System Driver
 */
#ifndef __FAT_H
#define __FAT_H
#include <stdint.h>
typedef struct __fat_bpb{
	uint8_t jmp[3];
	uint64_t oem;
	uint16_t bps;
	uint8_t spc;
	uint16_t numResv;
	uint8_t tblcnt;
	uint16_t dirent;
	uint16_t tsect;
	uint8_t mdt;
	uint16_t spf16;
	uint16_t spt;
	uint16_t nheads;
	uint32_t nhsect;
	uint32_t lsect;
}BPB;
int __fat12_read(const char *path,void *buf, int n);
int __fat16_read(const char *path,void *buf, int n);
int __fat32_read(const char *path,void *buf, int n);
#endif
