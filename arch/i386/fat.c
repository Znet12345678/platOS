#include <fat.h>
#include <stdlib.h>

#ifdef MODULE_BUILD
#include <module.h>
REGISTER(__fat12_read);
REGISTER(__fat16_read);
REGISTER(__fat32_read);
REGISTER(__fat12_write);
REGISTER(__fat16_write);
REGISTER(__fat32_write);
#endif
uint32_t rootDirLba(){
	
}
int __fat12_read(int kfd,const char *path,void *buf, unsigned long n){
	int i = 0;
	int j = 0;
a:;
	while(path[i] == '/')
		i++;
	char *current = malloc(strlen(path));
	while(path[i] != 0 && path[i] != '/'){
		curret[j] = path[i];
		j++;
		i++;		
	}
	
}
int __fat16_read(int kfd,const char *path,void *buf, unsigned long n){

}
int __fat32_read(int kfd,const char *path,void *buf, unsigned long n){

}
int __fat12_write(int kfd,const char *path,void *buf, unsigned long n){

}
int __fat16_write(int kfd,const char *path,void *buf, unsigned long n){

}
int __fat32_write(int kfd,const char *path,void *buf, unsigned long n){

}
