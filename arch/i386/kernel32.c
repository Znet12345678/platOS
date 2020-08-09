#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <fat.h>
#include <vfs.h>
#include <stdio.h>
#include <libmem.h>
#include <stdlib.h>
#include <libata.h>
#include <ps2.h>
#include <acpi.h>
#include <libio.h>
#include <stdint.h>
#include <tty.h>
#include "strtok_r.h"
void bzero(void *buf,unsigned long n);
unsigned long strlen(const char *str);
int strcmp(const char *str,const char *s){
	if(strlen(str) != strlen(s))
		return -1;
	for(int i = 0; i < strlen(s);i++)
		if(s[i]!=str[i])
			return i+1;
	return 0;
}
int memcmp(const void *a,const void *b,unsigned long n){
	for(int i = 0; i < n;i++)
		if(*(uint8_t*)(a + i) != *(uint8_t*)(b + i))
			return i+1;
	return 0;
}
void strcpy(char *str,const char *str2){
	for(int i = 0; i < strlen(str2);i++)
		str[i] = str2[i];
}
void strcat(char *str,const char *str2){
	strcpy(&str[strlen(str)-1],str2);
}

void disable_stdcursor(){ 
	asm("mov $0x3D4,%dx");
	asm("mov $0xa,%al");
	asm("out %al,%dx");
	asm("inc %dx");
	asm("mov $0x20,%al");
	asm("out %al,%dx");
}
int x = 0,y = 0;
uint16_t *vgabase = (uint16_t*)0xa0000;


void putc(uint8_t c){
	if(c == 0)
		return;
	ccolor = (VGA_COLOR_BLACK << 4 | VGA_COLOR_LIGHT_GREY);
	if(c != '\n'){
		for(int i = 0; i < 8; i++){
			for(int j = 0; j < 8;j++){
				if((font8x8_basic[c][i] >> j) & 1){
					*(char*)(0xa0000 + x+j + (y+i)*320)=0xf;
				}
			}
		}
		x+=8;
	}
	if(x >= 320 || c == '\n'){
//		*((uint16_t*)0xb8000 + y * 80 + x + 1) = 0;
		y+=8;
		x=0;
	}
	if(y >= 200){
		for(int i = 0; i < 320*292;i++)
			*((uint8_t*)0xa0000 + i) = *(uint8_t*)((uint8_t*)0xa0000 + i + 320*8);
		for(int i = 472*640;i < 480*640;i++)
			*((uint8_t*)0xa0000 + i) = 0;
		y-=8;
		x=0;
	}
	if(x == 0){
		*((uint16_t*)0xb8000+y*80+x) = ' ' | VGA_COLOR_LIGHT_GREY << 12;
	}

}
int intlen(unsigned int n){
	int ret = 0;
	while(n > 0){
		n/=10;
		ret++;
	}
	return ret;
}
int pow(int a,int b){
	int ret = 1;
	for(int i = 0; i < b;i++)
		ret*=a;
	return ret;
}
void puti(int n){
	if(n == 0)	
		putc('0');
	if(n < 0){
		putc('-');
		n*=-1;
	}
	for(int i = intlen(n)-1; i >= 0;i--){
		putc(n/pow(10,i)+'0');
		n-=(n/pow(10,i))*pow(10,i);
	}
}
void vga_putent(uint16_t off,uint16_t val){
	*(uint16_t*)(0xb8000 + off*2) = val;
}
//uint8_t cccolor = ccolor;
void blink(){
	vga_putent(y*80+x,' ' | ccolor << 8);
	if(ccolor ==15 << 4) ccolor = 0;
	else	ccolor = 15 << 4;	
}
void init_vidmem(){
	x = 0; y = 0;
	for(int i = 0; i < 80*26;i++)
		*((uint16_t*)0xb8000+i) = ' ' |  (ccolor << 8);
}
unsigned long strlen(const char *str){
	unsigned long ret = 0;
	while(str[ret] != 0)
		ret++;
	return ret;
}
void puts(const char *str){
	for(int i = 0; i < strlen(str);i++)
		putc(str[i]);
}
int xlen(unsigned int n){
	unsigned int ret = 0;
	while(n > 0 && !(n == 0 && n % 16 != 0)){
		ret++;
		n/=16;
	}
	return ret;
}

void memcpy(void *dest,const void *src,unsigned long n){
	uint8_t *bdest = (uint8_t*)dest;
	uint8_t *bsrc = (uint8_t*)src;
	for(int i = 0; i < n;i++)
		bdest[i] = bsrc[i];
}
void panic(uint32_t ip){
//	asm("movl -4(%esp),%eax");
//	asm("movl 0(%esp),%ebx");
//	asm("mov %%eax,%0" :"=m"(eip));
//	asm("mov %%ebx,%0" : "=m"(cs));
	puts("\n");
	puts("->0x");
	putx(ip);
	puts("\n");
	puts("panic():Disabling Interrupts and Halting CPU\n");
	asm("cli");
	for(int i = 0; i < 80;i++)
		putc('#');
	while(1){
		asm("hlt");
	}
}
char lower(char upper){
	if(upper >= 'A' && upper <= 'Z')
		return upper+0x20;
}
int atox(const char *str){
	if(strncmp(str,"0x",2) == 0)
		str+=2;
	int ret = 0;
	int multiplier = 1;
	for(int i = strlen(str)-1; i >= 0;i--,multiplier*=16){
		int val = str[i] >= '0' && str[i] <= '9' ? str[i]-'0' : (lower(str[i])-'a'+10);
		ret+=val*multiplier;
	}
	return ret;
}
int strncmp(const char *s1,const char *s2,size_t n){
	for(int i = 0; i < n;i++)
		if(s1[i] != s2[i])
			return -1;
	return 0;
}
void dbg(){
	while(1){
		puts("\n(DBG)");
		char *str = gets();
		char **saveptr = malloc(strlen(str)*sizeof(*saveptr));
		char *tok = strtok_r(str," ",saveptr);
		if(strcmp(tok,"x") == 0){
			puts("\n");
			putx(*(uint32_t*)atox(strtok_r(NULL," ",saveptr)));
		}
		else if(strcmp(tok,"q") == 0)
			break;
		else if(strcmp(tok,"j") == 0){
			void (*func)() = (void(*)())atox(strtok_r(NULL," ",saveptr));
			func();
		}
	}
}
void putx(unsigned long n){
	for(int i = 28; i >= 0;i-=4){
		char c = (n >> i) & 0xf;
	       	if(c > 9)
			putc(c-10+'A');
		else
			putc(c+'0');	
	}

}
void debug(const char *task,const char *msg){
	puts("[");
	puts(task);
	puts("]:");
	puts(msg);
	puts("\n");
}
uint8_t blacklist_shellcode[] = {0x66, 0xb8, 0x00, 0x00, 0x00, 0x00, 0xc3};
void blacklist(void *pntr){
	memcpy(pntr,blacklist_shellcode,sizeof(blacklist_shellcode));
}
void _panic(const char *msg){
	puts("_panic():");
	puts(msg);
	asm("cli");
	asm("hlt");
}
int dev_llfd;
int getdevs(){
	return dev_llfd;
}
extern char *argv[];
int mountllfd;
void main(){

//	libmem_init();
	unmap(1);
	init_vidmem();
	//*(uint16_t*) 0xa8000 = ' ';

	puts("platOS Kernel ");
	puts(KERNEL_IDENT);
	puts("\n[Kernel Architecture:");
	puts(KERNEL_ARCH);
	puts(" Release: ");
	puts(RELEASE);
	puts("]\n");	
	init_int();
	ata_dev_t **ata = libata_init();
	disable_stdcursor();
	int res = ps2_init();
	if(!res){
		puts("ps2 failed\n");
		blacklist(ps2_getc);
		blacklist(stdin_read);
	}
	else{
		puts("PS/2 Driver Initialization Successful\n");
	}

	puts("Mapping devices...\n");
	int llfd = map_devs(ata);
	dev_llfd = llfd;
	puts("init vfs\n");
	vfs_init();
	puts("[Loader]Select root:\n");
	int fd = kopen(llfd,"disk");
	if(fd <0){
		puts("Failed to open disk driver\nhlt");
		asm("hlt");
	}
	int r = 0;
	dev_t *dev = malloc(sizeof(*dev));
	while(kiter(fd,dev,sizeof(*dev))!= 0 ){
		puts(dev->name);
		puts(":0x");
		putx((uint32_t)dev);
		puts("\n");
	}
	char *pntr = gets();
	int kfd = kopen(llfd,pntr);
	if(kfd < 0){
		puts("\n");
		puts(pntr);
		puts(":");
		_panic("failed to open device");
	}
	dev_t *d;
	LinkedList *ll = llopen(kfd);
	d = (((kfd_t*)(ll->data))->data);
	fs_t *f = fat_init(d);
#ifdef DEBUG
	puts("fat_init(...)\n");
#endif
	if(!f || !f->verify){
		puts("NOT FAT\n");
	}
	mountllfd = llnew();
	struct LinkedList *mountll = llopen(mountllfd);

	mountll->data = malloc(MOUNT_MAX*sizeof(mount_t));
	if(!__mount(mountllfd,"/",d,f)){
		_panic("Failed to mount rootfs\n");
	}
	__fat_open(d,"/init",O_RDONLY);
	_panic("nothing to do\n");	
	
}
