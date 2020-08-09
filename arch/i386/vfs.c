#include <string.h>
#include <ll.h>
#include <fcntl.h>
#include <vfs.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
int __mount(int llfd,const char *mountpoint,dev_t *d,fs_t *fs){
	struct LinkedList *ll = llopen(llfd);
	mount_t *pntr = (mount_t*)ll->data;
	while(pntr->nxt != NULL)
		pntr = pntr->nxt;
	dbg();
	pntr->nxt = malloc(sizeof(*pntr->nxt));
	dbg();
	mount_t *sv = pntr;
	pntr = pntr->nxt;
	pntr->dev = d;
	pntr->fs = fs;
	pntr->fd = ((int(*)())fs->open)(NULL,O_RDWR);
	memcpy(pntr->path,mountpoint,MAX_PATH_LEN);
	if(!fs->verify){
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
dev_t *getATA(){
	int kfd = kopen(getdevs(),"disk");
	LinkedList *ll = llopen(kfd);
	return (dev_t*)(((kfd_t*)ll->data)->data);
}
int vfs_init(){
	int llfd = llnew();
	struct LinkedList *ll = llopen(llfd);

	mount_t *pntr = (mount_t*)malloc(sizeof(*pntr));
	ll->data = pntr;

	return llfd;
}	
