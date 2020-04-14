#include <types.h>
#include <vfs.h>
#include <fs.h>
#include <limits.h>
#include <cpp.h>
int __open(dev_t *dev,const char *path,int flags){
	char **saveptr = (char**)malloc(sizeof(*saveptr)*PATH_MAX);	
	char *patha = (char*)malloc(strlen(path)+1);
	strcpy(patha,path);
	char *first = strtok_r(patha,"/",saveptr);
	if(dev->fs_type != FAT16)
		return -1;

	free(saveptr);
	free(patha);
}
