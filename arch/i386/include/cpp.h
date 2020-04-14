#ifndef __CPP_H
#define __CPP_H
extern "C"{
       	size_t strlen(const char *);
	extern int total_sectors,fat_size,root_dir_sectors,first_data_sector,first_fat_sector,data_sectors,total_clusters,first_root_dir_sector;
	void *malloc(size_t);
	void strcpy(char *a,const char *b);
	char *strtok_r(char *,const char *,char**);
	void free(void *);	
}
#endif
