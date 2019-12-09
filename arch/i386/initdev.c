#include <stdio.h>
#include <vfs.h>
#include <ps2.h>
#include <libio.h>
#include <libmem.h>
#include <stdlib.h>
#include <stdint.h>
#include <mbr.h>
extern void *kernel32_dev_first_node;
void diskPartProbe(struct dev *ata,struct dev *ataParts,ata_dev_t **lst);
void stdout_write(void *pntr,int offset, int n){
	puts(pntr);
}
void map_devs(ata_dev_t **lst){
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
	ata->read = (int*)ata_read48;
	ata->write = (int*)ata_write48;
	strcpy(ata->name,"diskcntrl");
	struct dev *ataParts = malloc(sizeof(*ataParts));

	diskPartProbe(ata,ataParts,lst);

	struct dev_list *dev_list = (struct dev_list *)kernel32_dev_first_node;
	if(!page_mapped(kernel32_dev_first_node)){
		puts("mapping address 0x");
		putx((uint32_t)kernel32_dev_first_node);
		puts("\n");
		identp(kernel32_dev_first_node);
	}
	dev_list->dev = stdin_dev;
	dev_list->nxt = malloc(sizeof(*dev_list->nxt));
	dev_list = dev_list->nxt;
	dev_list->dev = stdout_dev;
	dev_list->nxt = malloc(sizeof(*dev_list->nxt));
	dev_list = dev_list->nxt;
	dev_list->dev = ata;
	dev_list->nxt = malloc(sizeof(*dev_list->nxt));
	dev_list = dev_list->nxt;
	dev_list->dev = ataParts;
}
int ATADEVREAD(struct dev *d,void *buf,unsigned int offset,unsigned int nb){
	return ((int (*)())d->read)(((ata_dev_t*)d->dataPntr)->ioaddr,((ata_dev_t*)d->dataPntr)->slavebyte,d->begin+offset/d->clustersize,nb%d->clustersize == 0 ? nb/d->clustersize:nb/d->clustersize+1);
}
int ATADEVWRITE(struct dev *d,void *buf,unsigned int offset,unsigned int nb){
	return ((int (*)())d->write)(((ata_dev_t*)d->dataPntr)->ioaddr,((ata_dev_t*)d->dataPntr)->slavebyte,d->begin+offset/d->clustersize,nb%d->clustersize==0?nb/d->clustersize:nb/d->clustersize+1);
}
int GPT(struct dev *ata,struct dev *ataparts,struct ata_dev *lst,struct partEnt *ent);
void diskPartProbe(struct dev *ata,struct dev *ataParts,ata_dev_t **lst){
	for(int i = 0; i < 8;i++){
		if(lst[i]){
			uint8_t *buf = malloc(512);
			((int(*)())ata->read)(lst[i]->ioaddr,lst[i]->slavebyte,buf,0,1);
			for(int i = 0; i < 4;i++){
				struct partEnt *pentry = (struct partEnt*)(buf + 0x1be);
				if(pentry[i].lba > 0){
					if(GPT(ata,ataParts,lst[i],pentry))return;
					strcpy(ataParts->name,"disk");
					ataParts->name[4] = 'a' + i;
					ataParts->name[5] = '0' + i;
					ataParts->begin = pentry[i].lba;
					ataParts->clustersize = 512;
					ataParts->dataPntr = lst[i];
					ataParts->read = (int*)ATADEVREAD;
					ataParts->write = (int*)ATADEVWRITE;
					ataParts->nxt = malloc(sizeof(*ataParts->nxt));
					ataParts=ataParts->nxt;	
					puts("Init ");
					puts(ataParts->name);
				}
			}		
			free(buf);
		}	
	}
}
int GPT(struct dev *ata,struct dev *ataparts, struct ata_dev *lst,struct partEnt *ent){

}
