#include "strtok_r.h"
#include <fs.h>
#include <string.h>
#include <vfs.h>
#include <fat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#ifdef MODULE_BUILD
#include <module.h>
REGISTER(__fat_init);
REGISTER(stub);
REGISTER(stub);
REGISTER(stub);
REGISTER(stub);
REGISTER(stub);
REGISTER(stub);
#endif
void breakpoint(){

}
extern int stub();
extern int mountllfd;
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
unsigned int spc;
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
	spc = bpb->spc;
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
struct LinkedList *readCluster(dev_t *dev,unsigned int sector){
	uint8_t *buf = malloc(dev->clustersize*spc);
	((int (*)())dev->read)(dev,buf,sector,dev->clustersize*spc);
	struct LinkedList *ret = malloc(sizeof(*ret));
	struct LinkedList *head = ret;
	char *tmpbuf = 0;
	while(1){
		if(buf[0] == 0){
			free(buf);
			return head;
		}
		else if(buf[0] == 0xe5){
			buf+=sizeof(struct __fat_dir);
			continue;
		}else if(buf[0xa] == 0xf){
			if(tmpbuf == 0)
				tmpbuf = malloc(0xff);
			memcpy(tmpbuf + strlen(tmpbuf),buf,0xa);
			buf+=sizeof(struct __fat_dir);
			continue;
		}
		ret->data = malloc(sizeof(struct __fat_dir)+0xb);
		memcpy(ret->data,buf,sizeof(struct __fat_dir));
		if(tmpbuf != 0){
			ret->data = realloc(ret,sizeof(struct __fat_dir)+strlen(tmpbuf)+1);
			bzero(ret->data,0xb);
			memcpy(ret->data + sizeof(struct __fat_dir),tmpbuf,strlen(tmpbuf)+1);
			tmpbuf = 0;	
		}
		else
			memcpy(ret->data+sizeof(struct __fat_dir),ret->data,0xb);
		ret->next = malloc(sizeof(*ret->next));
		ret = ret->next;
	}
	return head;
}
int __fat_open(dev_t *dev,const char *path,unsigned int o){
	breakpoint();
	char *pathc = malloc(strlen(path)+1);
	breakpoint();
	strcpy(pathc,path);
	breakpoint();
	char **saveptr = malloc(strlen(path)*sizeof(*saveptr));
	breakpoint();
	char *tok = strtok_r(pathc,"/",saveptr);
	breakpoint();
	struct LinkedList *entry = readCluster(dev,first_root_dir_sector);

	while(entry->next != 0){
		for (int i = 0; i < 0xb;i++)
			putc(*((char*)entry->data + i));
		entry = entry->next;
	}
}
/*
   int __fat_open(dev_t *dev, const char *path,int o){
   char **saveptr = malloc(sizeof(*saveptr)*PATH_MAX);
   char *tok = strtok_r(path,"/",saveptr);
   void *buf = malloc(getATA()->clustersize);
   ((int (*)())dev->read)(dev,buf,first_root_dir_sector,dev->clustersize*spc);
   int i = 0;int j = 0;
   char *lname = 0;
   int lba = first_root_dir_sector;
   while(tok != NULL){
   while(1){
   if(i > dev->clustersize*(spc+j)){
   j++;
   ((int (*)())dev->read)(dev,buf,lba+j,dev->clustersize*spc);
   }
   struct __fat_dir *fat_dir = (struct __fat_dir *)(buf + i);
   if(fat_dir->name[0] == 0)
   return -1;
   if(fat_dir->name[0] != 0xe5){
   if(fat_dir->name[10] == 0x0f){
   if(lname == 0)
   lname = malloc(1024);
   strcat(lname,fat_dir->name);
   }else{
   if(lname != 0){
   if(strcmp(lname,tok) == 0){
   lba = ((fat_dir->clusterHigh << 16 | fat_dir->clusterLow) - 2)* spc + first_data_sector;
   break;
   }
   free(lname);
   lname = 0;
   }else{
   if(strcmp(fat_dir->name,tok) == 0){
   lba = ((fat_dir->clusterHigh << 16 | fat_dir->clusterLow) - 2)*spc + first_data_sector;
   break;
   }
   }
   }
   }
   i+=0x20;
   }
   unsigned int table_value = 0;
   if(bits == 12){
   unsigned char FAT_table[512];
   unsigned int fat_offset = active_cluster + (active_cluster/2);
   unsigned int fat_sector = first_fat_sector + (fat_offset/section_size);
   unsigned int ent_offset = fat_offset%section_size;
   ((int (*)())dev->read)(dev,FAT_table,fat_sector,dev->clustersize);
   unsigned short table_value = *(unsigned short *)&FAT_table[ent_offset];
   if(lba & 1)
   table_value>>=4;
   else
   table_value&=0x0fff;

   }else if(bits == 16){
   unsigned char FAT_table[512];
   unsigned int fat_offset = active_cluster*2;
   unsigned int fat_sector = first_fat_sector + (fat_offset/sector_size);
   unsigned int ent_offset = fat_offset % sector_size;
   ((int(*)())dev->read)(dev,FAT_table,fat_sector,dev->clustersize);
   unsigned short table_value = *(unsigned short *)&FAT_table[ent_offset];


   }

   }
   free(saveptr);
   free(tok);
   }
   */
