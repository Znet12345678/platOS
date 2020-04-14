#include <fs.h>
FSInstance::FSInstance(dev_t *dev,fs_t *fs){
	this->dev = dev;
	this->fs = fs;
}
int FSInstance::read(int fd,void *f,size_t n){
	return ((int(*)(dev_t*,int,void*,size_t))this->fs->read)(dev,fd,f,n);
}
int FSInstance::write(int fd,void *f,size_t n){
	return ((int(*)(dev_t*,int,void *,size_t))this->fs->write)(dev,fd,f,n);
}
int FSInstance::open(const char *path,int flags){
	return ((int(*)(dev_t*,const char *,int))this->fs->open)(dev,path,flags);
}
int FSInstance::lseek(int fd,unsigned long pos,int flag){
	return ((int(*)(dev_t*,int,unsigned long,int))this->fs->lseek)(dev,fd,pos,flag);
}
