#ifndef __FS_H
#define __FS_H
#include <types.h>
#include <vfs.h>
#ifdef __cplusplus
class FSInstance{
	private:;
	dev_t *dev;
	fs_t *fs;
	public:;
	FSInstance(dev_t *dev,fs_t *fs);
	int open(const char *path,int flags);
	int read(int fd,void *buf,size_t n);
	int write(int fd,void *buf,size_t n);
	int lseek(int,size_t,int);
};
extern "C"{
	struct __file{
	 	int flags;
		size_t posoff;
		size_t poslba;
		size_t abspos;
		size_t size;
	};
};
#endif
#define FS_NONE 0 
#define FAT12 1
#define FAT16 2 
#define FAT32 3 
#define EXT2 4 //Not supported yet
#define EXT3 5 //Not supported yet
#define EXT4 6 //Not supported yet
#endif
