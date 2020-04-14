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
#include <ll.h>
typedef struct dev{
	uint8_t name[0x20];
	uint32_t begin;//Begining cluster of data. Useful for partitions
	uint32_t clustersize;//Size of cluster e.g. 512 bytes for sector
	int *read;//read count bytes at offset into device to buf
	int *write;//Write count bytes to offset of buf
	short fs_type;
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
	struct mountpoint *nxt;
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
	int verify;
	LinkedList *data;
}fs_t;//These are all pointers to functions
typedef struct kfd{
	void *data;
	unsigned long pos;
}kfd_t;
dev_t *getATA();
int vfs_init();
int kopen(int dlld,const char *name);
int kread(int llfd,void *buf,unsigned long n);
int kiter(int llfd,void *buf,unsigned long n);
int klseek(int llfd,int pos, int flags);
int  __mount(int llfd,const char *mountpoint,dev_t *dev,fs_t *fs);
void register_dev(dev_t *pntr);
int map_devs(ata_dev_t **pntr);
int close(int fd);
int getdevs();
#endif
