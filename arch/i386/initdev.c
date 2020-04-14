#include <libata.h>
#include <unistd.h>
#include <stdio.h>
#include <vfs.h>
#include <ps2.h>
#include <libio.h>
#include <libmem.h>
#include <stdlib.h>
#include <string.h>
#include <gpt.h>
#include <stdint.h>
#include <mbr.h>
#include <ll.h>
#include <KMEM.h>
extern void *kernel32_dev_first_node;
void diskPartProbe(struct dev *ata,struct dev *ataParts,ata_dev_t **lst);
void stdout_write(void *pntr,int offset, int n){
	puts(pntr);
}
int map_devs(ata_dev_t **lst){
#ifdef DEBUG
	puts("map_dev(...):init\n");
#endif
	struct dev *stdin_dev = malloc(sizeof(*stdin_dev));	
	strcpy(stdin_dev->name,"stdin\0");
	stdin_dev->begin = 0;
	stdin_dev->clustersize = 1;
	stdin_dev->read = (int*)stdin_read;
	stdin_dev->write = NULL;
	struct dev *stdout_dev = malloc(sizeof(*stdout_dev));
	stdout_dev->begin = 0;
	strcpy(stdout_dev->name,"stdout\0");
	stdout_dev->clustersize = 1;
	stdout_dev->read = NULL;
	stdout_dev->write = (int*)stdout_write;
	struct dev *ata = malloc(sizeof(*ata));
	ata->begin = 0;
	ata->clustersize = 512;
	ata->read = (int*)ata_read24;
	ata->write = (int*)ata_write24;
	ata->dataPntr = lst;
	strcpy(ata->name,"disk\0");
	struct dev *ataParts = malloc(sizeof(*ataParts));
#ifdef DEBUG
	puts("diskPartProbe(...):call\n");
#endif
	diskPartProbe(ata,ataParts,lst);
#ifdef DEBUG
	puts("diskPartProbe(...):returned control\n");
#endif
	ata->nxt = ataParts;
	int lld = llnew();

	struct LinkedList *llhd = (struct LinkedList *)llopen(lld);
	llhd->data = malloc(sizeof(struct dev_list));
	struct dev_list *dev_list = llhd->data;	
	dev_list->dev = stdin_dev;
	dev_list->nxt = malloc(sizeof(struct dev_list));
	dev_list = dev_list->nxt;
	dev_list->dev = stdout_dev;
	dev_list->nxt = malloc(sizeof(struct dev_list));
	dev_list = dev_list->nxt;
	dev_list->dev = ata;
	dev_t *d = ata->nxt;
	dev_list->nxt = NULL;
#ifdef DEBUG
	puts("map_devs(...):ret\n");
#endif
	return lld;
}
int kopen(int lld,const char *name){
	void *data = NULL;
	struct dev_list *dev_list = (struct dev_list *)((struct LinkedList *)llopen(lld))->data;
	while(dev_list != 0 && strcmp(dev_list->dev->name,name) != 0){
		if(dev_list->dev->nxt != 0){
			dev_t *dp = dev_list->dev;
			while(strcmp(dp->name,name) != 0 &&  dp != NULL){
				dp = dp->nxt;
			}
			if(dp != NULL){
				data = dp;
				break;
			}
		}
		dev_list=dev_list->nxt;
	}
	if(dev_list == 0 && data == NULL)
		return -1;
	int llfd = llnew();
	LinkedList *ll = llopen(llfd);
	kfd_t *kfd = malloc(sizeof(*kfd));
	if(data == NULL)
		kfd->data = dev_list->dev;
	else
		kfd->data = data;
	kfd->pos = 0;
	ll->data = kfd;
	ll->next = NULL;
	return llfd;
}

int klseek(int llfd,int pos,int flags){
	if(flags == SEEK_SET){
		LinkedList *ll = llopen(llfd);
		((kfd_t*)ll->data)->pos = pos;
		return pos;
	}else if(flags == SEEK_CUR){
		LinkedList *ll = llopen(llfd);
		return ((kfd_t*)ll->data)->pos;
	}else if(flags == SEEK_END){
		while(kread(llfd,NULL,1) == 1);//We can't know how big the device is through an llfd
		return klseek(llfd,0,SEEK_CUR);
	}
	return -1;

}
int kread(int llfd,void *buf,unsigned long n){
	LinkedList *ll = llopen(llfd);
	dev_t *d = (dev_t*)((kfd_t*)ll->data)->data;
	int kfd = kopen(getdevs(),"disk");
	dev_t *disk = ((dev_t*)((LinkedList*)llopen(kfd))->data);
	int r = ((int (*)())(d->read))(disk,buf,((kfd_t*)ll->data)->pos,n);
	((kfd_t*)ll->data)->pos+=r;
	return r;
}
int kiter(int llfd,void *buf,unsigned long n){
	LinkedList *ll = llopen(llfd);	
	dev_t *d = (dev_t*)((kfd_t*)ll->data)->data;
	int pos = ((kfd_t*)ll->data)->pos;

		
	while(pos > 0){
		if(pos >= sizeof(*d)){
			pos-=sizeof(*d);
			d = d->nxt;
			continue;
		}
		d=(dev_t*)((uint8_t*)d+1);
		pos--;
	}
	if(d == NULL)
		return 0;
	unsigned long i = 0;
	while(i < n){
		if(n-i >= sizeof(*d)){
			if(buf != NULL)
				memcpy(buf + i,(uint8_t*)(d + i),sizeof(*d));
			i+=sizeof(*d);
			d = d->nxt;
			if(d == NULL)
				break;
			continue;
		}
		if(buf != NULL)
			memcpy(buf + i,(uint8_t*)(d + i),1);
		d=(dev_t*)((uint8_t*)d+1);
		i++;
	}
        ((kfd_t*)ll->data)->pos+= i;
	return i;
}

int ATADEVREAD(struct dev *d,void *buf,unsigned int offset,unsigned int nb)
{
	
	return ((int (*)())getATA()->read)(((ata_dev_t*)d->dataPntr)->ioaddr,((ata_dev_t*)d->dataPntr)->slavebyte,buf,d->begin+offset/d->clustersize,nb%d->clustersize == 0 ? nb/d->clustersize:nb/d->clustersize+1)*d->clustersize;
}
int ATADEVWRITE(struct dev *d,void *buf,unsigned int offset,unsigned int nb){
	return ((int (*)())getATA()->write)(((ata_dev_t*)d->dataPntr)->ioaddr,((ata_dev_t*)d->dataPntr)->slavebyte,buf,d->begin+offset/d->clustersize,nb%d->clustersize==0?nb/d->clustersize:nb/d->clustersize+1) *d->clustersize;
}
int GPT(struct dev *ata,struct dev *ataparts,struct ata_dev *lst,struct partEnt *ent);
void diskPartProbe(struct dev *ata,struct dev *ataparts,ata_dev_t **lst){
	for(int i = 0; i < 8;i++){
		if(lst[i]){
			uint8_t *buf = malloc(512);
			((int(*)())ata->read)(lst[i]->ioaddr,lst[i]->slavebyte,buf,0,1);
			for(int j = 0; j < 4;j++){
				struct partEnt *pentry = (struct partEnt*)((uint8_t*)buf + 0x1be + j * 0x10);
				if(pentry->lba > 0){
//					if(GPT(ata,ataparts,lst[i],pentry))return;
					strcpy(ataparts->name,"disk");
					ataparts->name[4] = 'a' + i;
					ataparts->name[5] = '0' + j;
					ataparts->begin = pentry->lba;
					ataparts->clustersize = 512;
					ataparts->dataPntr = lst[i];
					ataparts->read = (int*)ATADEVREAD;
					ataparts->write = (int*)ATADEVWRITE;

					ataparts->nxt = malloc(sizeof(*ataparts->nxt));
#ifdef DEBUG
					puts(ataparts->name);
#endif
					ataparts=ataparts->nxt;
				}
			}		
			free(buf);

		}	
	}
}
uint32_t crc32(void *pntr,uint32_t *CRCTab,unsigned long n){
	uint32_t start = -1;
	for(int i = 0; i < n;i++){
		uint8_t indx = (start ^ ((uint8_t*)pntr)[i]);
		start = (start >> 8) ^ CRCTab[indx];
	}
	start^=-1;
	return start;
}
int GPT(struct dev *ata,struct dev *ataparts, struct ata_dev *lst,struct partEnt *ent){
	uint8_t *sectorBuffer = malloc(512);
	((int(*)())ata->read)(lst->ioaddr,lst->slavebyte,sectorBuffer,1,1);
	if(memcmp(sectorBuffer,GPT_SIG,8))
		return 0;
	uint32_t hdrSize = *(uint32_t*)(sectorBuffer+3);
	//uint64_t partLba = *(uint32_t*)(sectorBuffer+10)/;
}
