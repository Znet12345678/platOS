/*
 * (c) 2019 Zachary James Schlotman
 * platOS Virtual Files System driver
 */
#ifndef __VFS_H
#define __VFS_H
#define MAX_PATH_LEN 0x200
#define STDIN 0
#define STDOUT 1
#include <stdint.h>
#include <libata.h>
typedef struct dev{
	uint8_t name[0x20];
	uint32_t begin;//Begining cluster of data. Useful for partitions
	uint32_t clustersize;//Size of cluster e.g. 512 bytes for sector
	int *read;//read count bytes at offset into device to buf
	int *write;//Write count bytes to offset of buf
	void *dataPntr;
	struct dev *nxt;
}__attribute__((packed)) dev_t;
typedef struct dev_list{
	dev_t *dev;
	struct dev_list *nxt;
}__attribute__((packed)) dev_list_t;
typedef struct mountpoint{
	int fd;//File descriptor for mount point
	dev_t *dev;//Device file to provide read and write functions
	char path[MAX_PATH_LEN];
}mount_t;
typedef struct FileDescriptor{
	uint8_t alloc;
	dev_t *device;
	unsigned long offset;
}__attribute__((packed)) FileDescriptor;
typedef struct fsDriver{
	int *read;
	int *write;
	int *open;
	int *lseek;
}fs_t;//These are all pointers to functions
void __mount(int fd,int offset,dev_t *dev,fs_t *fs);
void register_dev(dev_t *pntr);
void map_devs(ata_dev_t **pntr);
int close(int fd);
#endif