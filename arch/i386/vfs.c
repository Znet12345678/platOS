#include <string.h>
#include <ll.h>
#include <fcntl.h>
#include <vfs.h>
#include <stdio.h>
#include <stdlib.h>
int __mount(int llfd,const char *mountpoint,dev_t *d,fs_t *fs){
	struct LinkedList *ll = llopen(llfd);
	mount_t *pntr = (mount_t*)ll->data;
	while(pntr->nxt != NULL)
		pntr = pntr->nxt;
	pntr->nxt = malloc(sizeof(*pntr->nxt));
	mount_t *sv = pntr;
	pntr = pntr->nxt;
	pntr->dev = d;
	pntr->fd = ((int(*)())fs->open)(NULL,O_RDWR);
	memcpy(pntr->path,mountpoint,MAX_PATH_LEN);
	if(!((int(*)())fs->verify)(d)){
		debug("VFS","Tried to mount device without the correct File System");
		sv->nxt = NULL;
		free(pntr);
		return 0;
	}
	return 1;
}
fs_t *resolvFS(const char *path){
	
}
int open(const char *path,unsigned int f){
	
}
int vfs_init(){
	int llfd = llnew();
	struct LinkedList *ll = llopen(llfd);

	mount_t *pntr = (mount_t*)malloc(sizeof(*pntr));
	ll->data = pntr;

	return llfd;
}	
