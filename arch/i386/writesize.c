#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
int main(int argc,char *argv[]){
	if(argc != 2)
		return -1;
	FILE *f = fopen(argv[1],"r+b");
	if(!f)
		return -1;
	fseek(f,0,SEEK_END);
	int size = ftell(f)%512 == 0 ? ftell(f)/512-1 : ftell(f)/512;
	fseek(f,2,SEEK_SET);
	char c = 0x7f;
	fwrite(&c,1,1,f);
	fwrite("platOS",1,strlen("platOS"),f);
	fwrite(&size,sizeof(int),1,f);
	fclose(f);
	return 0;
}
