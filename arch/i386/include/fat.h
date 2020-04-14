/*
 * FAT File System Driver
 */
#ifndef __FAT_H
#define __FAT_H
#include <vfs.h>
#include <stdint.h>
typedef struct __fat_bpb{
	uint8_t jmp[3];
	uint8_t oem[8];
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
}__attribute__((packed)) BPB;
struct __fat_extBS_16{
	uint8_t bios_drive_num;
	uint8_t resv1;
	uint8_t sig;
	uint32_t vid;
	uint8_t v_lab[11];
	uint8_t ftl[8];
}__attribute__((packed));
struct __fat_extBS_32{
	uint32_t tabsz32;
	uint16_t extFlags;
	uint16_t ver;
	uint32_t rootCluster;
	uint16_t fat_info;
	uint16_t backupBS;
	uint8_t reserved_0[12];
	uint8_t bios_drive_num;
	uint8_t resv1;
	uint8_t sig;
	uint32_t vid;
	uint8_t v_lab[11];
	uint8_t vtl[8];
}__attribute__((packed));
fs_t *fat_init(dev_t*fat_part);
	
int __fat12_read(int fd,void *buf, unsigned long n);
int __fat16_read(int fd,void *buf, unsigned long n);
int __fat32_read(int fd,void *buf, unsigned long n);
int __fat12_write(int fd,void *buf,unsigned long n);
int __fat16_write(int fd,void *buf,unsigned long n);
int __fat32_write(int fd,void *buf,unsigned long n);
int __fat12_open(const char *path,unsigned int flags);
int __fat16_open(const char *path,unsigned int flags);
int __fat32_open(const char *path,unsigned int flags);
int __fat12_lseek(int fd,unsigned long pos,unsigned int flags);
int __fat16_lseek(int fd,unsigned long pos,unsigned int flags);
int __fat32_lseek(int fd,unsigned long pos,unsigned int flags);
#endif
