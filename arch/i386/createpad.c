#include <stdio.h>
int main(int argc,char *argv[]){
	if(argc < 3)
		return -1;
	FILE *f = fopen(argv[1],"rb");
	if(!f)
		return -1;
	FILE *f2 = fopen(argv[2],"wb");
	if(!f2)
		return -1;
	fseek(f,0,SEEK_END);
	int pos = ftell(f);
	fclose(f);
	for(int i = 0; i < 1024-pos;i++)
		fputc(0,f2);
	fclose(f2);
	return 0;
}
