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
typedef struct dev{
	uint32_t begin;//Begining cluster of data. Useful for partitions
	uint32_t clustersize;//Size of cluster e.g. 512 bytes for sector
	int read(void *buf,int offset,int count);//read count bytes at offset into device to buf
	int write(void *buf,int offset,int count);//Write count bytes to offset of buf
}dev_t __attribute__((packed));
typedef struct dev_list{
	dev_t *dev;
	dev_list_t *nxt;
}dev_list_t __attribute__((packed));
typedef struct mountpoint{
	int fd;//File descriptor for mount point
	dev_t *dev;//Device file to provide read and write functions
	char path[MAX_PATH_LEN];
}mount_t;
typedef struct FileDescriptor{
	uint8_t alloc;
	dev_t *device;
	unsigned long offset;
}FileDescriptor __attribute__((packed));
typedef struct fsDriver{
	int read(int file,void *buf,int n);
	int write(int file,void *buf,int n);
	int open(const char * path,unsigned int flags);
	int lseek(int file,int offset,int mac);
}fs_t;
void __mount(int fd,int offset,dev_t *dev,fs_t *fs);
void register_dev(dev_t *pntr);

void map_devs();
int close(int fd);
#endif
