typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
#include <libelf.h>
void memcpy(dest,src,n){
	int i = 0;
	while(i < n){
		((char*)dest)[i] = ((char*)src)[i];
		i++;
	}
}
extern void putc(c);
int strlen(str)
{
	int i = 0;
	while(((char*)str)[i] != 0)
		i++;
	return i;
}
int strcmp(str,two){
	int i =0;
	if(strlen(str) != strlen(two))
		return -1;
	while(i < strlen(str)){
		if(((char*)str)[i]!=((char*)two)[i])
			return i+1;
		i++;
	}
	return 0;
}
//void ata_read(void *pntr,unsigned long lba,unsigned int n);
typedef struct fat_extBS_32
{
	//extended fat32 stuff
	unsigned int		table_size_16;
	unsigned short		extended_flags;
	unsigned short		fat_version;
	unsigned int		root_cluster;
	unsigned short		fat_info;
	unsigned short		backup_BS_sector;
	unsigned char 		reserved_0[12];
	unsigned char		drive_number;
	unsigned char 		reserved_1;
	unsigned char		boot_signature;
	unsigned int 		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
 
}  fat_extBS_32_t;
 
typedef struct fat_extBS_16
{
	//extended fat12 and fat16 stuff
	unsigned char		bios_drive_num;
	unsigned char		reserved1;
	unsigned char		boot_signature;
	unsigned int		volume_id;
	unsigned char		volume_label[11];
	unsigned char		fat_type_label[8];
 
} fat_extBS_16_t;
 
typedef struct fat_BS
{
	unsigned char 		bootjmp[3];
	unsigned char 		oem_name[8];
	unsigned short 	        bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		table_size_16;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int 		hidden_sector_count;
	unsigned int 		total_sectors_32;
 
	//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	unsigned char		extended_section[54];
 
}  fat_BS_t;
struct dap{
	unsigned char size;
	unsigned char zero;
	unsigned short sectors;
	unsigned short offset;
	unsigned short segment;
	unsigned int startsh;
	unsigned int startsl;
};
struct parttab{
	unsigned char attr;
	unsigned char chs[3];
	unsigned char partt;
	unsigned char chs2[3];
	unsigned int lba;
	unsigned int nsect;
};
extern void puts(str);
extern void putx(x);
extern void _ata_read(buf,lba,n,seg);
extern void err();
int data = 0;
void ata_reads(buf,lba,n,seg){
	int i = 0;
	unsigned char *rs = 0xfe00;
	if(data != 0){
		//putx(buf);
		//putx((unsigned int)(lba+data));
		//putx(n);
		_ata_read(buf,(unsigned int)(lba+data),n,seg);
		return;
	}
	_ata_read(rs,0,1,0);
	while(i < 4){
		struct parttab *pt = (struct parttab *)(rs + i*0x10 + 0x1be);
//		puts(".");	
		if((pt->attr >> 7) & 1){
			_ata_read(buf,(unsigned int)(lba+pt->lba),n,seg);
	//		putx(lba+pt->lba);		
			data = pt->lba;
			return;
		}

		i++;
	}

	_ata_read(buf,(unsigned int)(lba),n,seg);
}
void ata_read_hi(buf,lba,n){
	ata_reads(buf,lba,n,0x1000);
}
void ata_read(buf,lba,n){
	ata_reads(buf,lba,n,0);
}
void putx(n){
	int i = 28;
	while(i >= 0)
	{
		unsigned char p = ((n >> i) & 0xf);
		putc(p <= 9 ? p+'0' : p+'A'-10);
		i-=4;
	}
}

/*void _ata_read(buf,lba,n){
	struct dap d;
	unsigned int b;
	d.size = 0x10;
	d.zero = 0;
	d.sectors=n;
	d.offset = buf;
	d.segment = 0x1000;
	d.startsh = lba >> 32;
	d.startsl = lba & 0xffffffff;
	b = (unsigned int)buf;
//	putx(buf);
//	putc(' ');
//	putx(lba);
//	putc(' ');
//	putx(n);
//	putx(*(unsigned char*)0x9000)
	inter(d);
}*/

void puts(pntr){
	int i = 0;
	while(((char*)pntr)[i] != '\0'){
		putc(((char*)pntr)[i]);
		i++;
	}
}
void err(){
	puts("->err\0");
	while(1);
}
void main(){
	void *buf = 0x3000;
	struct fat_extBS_16 *fat_boot_ext_16 = (struct fat_extBS_16 *)buf;
	struct fat_extBS_32 *extBS32 = (struct fat_extBS_32 *)buf;
	unsigned int total_sectors,fat_size,root_dir_sectors,first_data_sector,first_fat_sector,data_sectors,total_clusters,first_root_dir_sector,root_cluster_32,first_sector_of_cluster;
	struct fat_extBS_32 *fat_boot_ext_32 = extBS32;
	fat_BS_t *fat_boot;
	unsigned int spc;
	puts("_start->");
	ata_read(buf,0,1);
	fat_boot = (fat_BS_t*)buf;
	if(*(unsigned short *)((uint8_t*)buf +0x0b) == 0){
		puts("bps=0->");
		err();
	}
	if(*(unsigned char *)((uint8_t*)buf+0x0d) == 0){
		puts("spc=0->");
		err();
	}
	extBS32 = (struct fat_extBS_32*)buf + 0x20;
	fat_boot_ext_16 = (struct fat_extBS_16*)buf + 0x20;
	total_sectors = (*(uint16_t*)((uint8_t*)buf+0x13) == 0)?*(uint32_t*)((uint8_t*)buf+0x20) : *(uint16_t*)((uint8_t*)buf+0x13);;
	fat_size = *(uint16_t*)((uint8_t*)buf+0x16);
	root_dir_sectors = ((*(uint16_t*)((uint8_t*)buf+0x11) * 32) + 511) / 512;
	first_data_sector = *(uint16_t*)((uint8_t*)buf+0x0e) + (*((uint8_t*)buf+0x10) * fat_size) + root_dir_sectors;
	//putx(*((uint8_t*)buf+0x10)*fat_size);
	first_fat_sector = *(uint16_t*)((uint8_t*)buf+0x0e);
	data_sectors = *(uint16_t*)((uint8_t*)buf+0x13) - (*(uint16_t*)(((uint8_t*)buf+0x0e)) + (*((uint8_t*)buf+0x10) * fat_size) + root_dir_sectors);
	total_clusters = data_sectors / *((uint8_t*)buf+0x0d);
	//putx(data_sectors);
	if(total_clusters < 4085){
		root_cluster_32 = extBS32->root_cluster;
		puts("FAT12->");
		//putx(total_clusters);
	}else if(total_clusters < 0xffff){
		unsigned short l16,h16;
		unsigned int v;
		unsigned char FAT_table[512];
		unsigned int fat_offset,fat_sector,ent_offset;
		unsigned short table_value;
		unsigned char *sbuf = 0x3800;
		unsigned char name[12];
		unsigned int read = 0;
		puts("FAT16->");
		spc = *((unsigned char *)buf + 0x0d);
		first_root_dir_sector = first_data_sector - root_dir_sectors;
		puts("a");
		ata_read(sbuf,first_root_dir_sector,root_dir_sectors);
		puts("b");
		l16 = *(unsigned short *)((unsigned char *)sbuf + 26);
		first_sector_of_cluster = first_root_dir_sector;

		goto a;
		v = l16;
b:;       	fat_offset = v * 2;
		fat_sector = first_fat_sector + (fat_offset/512);
		ata_read(FAT_table,fat_sector,1);
		ent_offset = fat_offset % 512;
		table_value = *(unsigned short *)&FAT_table[ent_offset];

		if(table_value >= 0xFFF8){
			puts("table_value->");
			err();
		}
		if(table_value == 0xfff7){
			puts("table_value->");
			err();
		}
		first_sector_of_cluster = ((table_value-2)*fat_boot->sectors_per_cluster)+ first_data_sector;
		ata_read(sbuf,first_sector_of_cluster,spc);
		read = 0;
a:;
 		//puts(sbuf);
  //		putx(first_data_sector);
		if(read > spc*512){
			v = l16;
			goto b;
		}
		
		if(sbuf[0] == 0){

			puts("EOF->");
			err();
		}

		if(sbuf[0]  == 0xe5){
			sbuf+=0x20;
			read+=0x20;
			goto a;
		}
		if(sbuf[11] == 0x0f){
			sbuf+=0x20;
			read+=0x20;
			goto a;
		}
		memcpy(name,sbuf,12);
//		putx(fat_boot->table_count);
		//puts(name);	
		if(strcmp(name,"KERNEL32BIN \0") == 0){
			unsigned char *tbuf,*head;
			tbuf = 0x00;
			head = tbuf;
			v = *(unsigned short*)&sbuf[26];
			table_value = v;
			//putx(v);
			puts("Loading Kernel");
			goto d;
c:;			fat_offset=v*2;
  			putc('.');
			fat_sector = first_fat_sector+(fat_offset/512);
			ent_offset = fat_offset%512;	
//			putx(ent_offset);
//			putx(*(unsigned int *)tbuf);		
			ata_read(FAT_table,fat_sector,1);
			table_value = *(unsigned short *)&FAT_table[ent_offset];
		//	putx(table_value);
			v = table_value;
			
			if(table_value >= 0xfff7){	
				puts("Entering protected mode");
				
		
				go_prot32();
				
			}
d:;
		putx(tbuf);
 		 ata_read_hi(tbuf,((table_value-2)*spc)+first_data_sector,spc);
		 //		putx(*tbuf);
 // 		putx(((table_value-2)*fat_boot->sectors_per_cluster)+first_data_sector);
		//	putx(spc);	
//  tbuf+=fat_boot->sectors_per_cluster*512;
		//	puts("!");
//			putx(table_value);
		//	putx(tbuf);

			tbuf+=512*spc;
		  goto c;
		

		}else{
			sbuf+=0x20;
			read+=0x20;
			goto a;
		}


	}else if(total_clusters < 268435445){
		putx(total_clusters);
	}
	else{
	
	}
	puts("Fell off the edge");
	err();
}


