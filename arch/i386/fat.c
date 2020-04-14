#include <fs.h>
#include <string.h>
#include <vfs.h>
#include <fat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#ifdef MODULE_BUILD
#include <module.h>
REGISTER(__fat_init)
REGISTER(stub);
REGISTER(stub);
REGISTER(stub);
REGISTER(stub);
REGISTER(stub);
REGISTER(stub);
#endif
extern int stub();
unsigned int active_cluster;
unsigned int first_fat_sector;
unsigned int fat_offset;
unsigned int section_size;
unsigned int total_sectors;
unsigned int root_dir_sectors;
unsigned int first_data_sector;
unsigned int first_fat_sector;
unsigned int data_sectors;
unsigned int root_cluster_32;
unsigned int total_clusters;
int fat_size;
int first_root_dir_sector;
uint8_t bits;
fs_t *fat_init(dev_t *fpart){
	void *head = malloc(getATA()->clustersize);
	int r = ((int(*)())fpart->read)(fpart,head,0,fpart->clustersize);

	if(r != getATA()->clustersize){

		return 0;
	}

	struct __fat_bpb *bpb = head;
	struct __fat_extBS_16 *extBS16 = head + sizeof(*bpb);
	struct __fat_extBS_32 *extBS32 = head + sizeof(*bpb);
	if(!(extBS16->sig == 0x28 || extBS16->sig == 0x29 || extBS32->sig == 0x28 || extBS32->sig == 0x29)){
		fs_t *ret = malloc(sizeof(*ret));
		ret->verify = 0;

		return ret;
	}
	total_sectors = bpb->tsect == 0 ? bpb->lsect : bpb->tsect;
	fat_size = (bpb->spf16 == 0) ? extBS32->tabsz32 : bpb->spf16;
      	root_dir_sectors = ((bpb->dirent*32)+(bpb->bps-1))/bpb->bps;
	first_data_sector = bpb->numResv + (bpb->tblcnt*fat_size)+root_dir_sectors;
	first_fat_sector = bpb->numResv;
	data_sectors = bpb->tsect - (bpb->numResv+(bpb->tblcnt*fat_size)+root_dir_sectors);
	total_clusters = data_sectors/bpb->spc;
	putx(total_clusters);
	if(total_clusters < 4085)
		bits = 12;
	else if(total_clusters < 65525)
		bits = 16;
	else if(total_clusters < 268435445)
		bits = 32;
	else
		bits = 0;
	first_root_dir_sector = first_data_sector-root_dir_sectors;
	if(bits == 32)
		root_cluster_32 = extBS32->rootCluster;
	fs_t *ret = malloc(sizeof(*ret));
	if(bits == 12){
		debug("fat_init","FAT12");
		ret->read =(int*) stub;
		ret->write = (int*)stub;
		ret->open = (int*)stub;
		ret->lseek = (int*)stub;
		ret->verify = 1;
		fpart->fs_type = FAT12;
		return ret;
	}
	if(bits == 16){
		debug("fat_init","FAT16");
		ret->read = (int*)stub;
		ret->write = (int*)stub;
		ret->open = (int*)stub;
		ret->lseek = (int*)stub;
		ret->verify = 1;
		fpart->fs_type = FAT16;
		return ret;
	}
	if(bits == 32){
		debug("fat_init","FAT32");
		ret->read = (int*)stub;
		ret->write = (int*)stub;
		ret->open = (int*)stub;
		ret->lseek = (int*)stub;
		ret->verify = 1;
		fpart->fs_type = FAT32;
		return ret;
	}
	ret->read = NULL;
	ret->write = NULL;
	ret->open = NULL;
	ret->lseek = NULL;
	ret->verify = 0;
	fpart->fs_type = FS_NONE;
	return ret;
}
uint32_t rootDirLba(){
	
}


