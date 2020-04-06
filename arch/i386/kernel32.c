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
uint16_t *vgabase = (uint16_t*)0xb8000;


void putc(uint8_t c){
	if(c == 0)
		return;
	ccolor = (VGA_COLOR_BLACK << 4 | VGA_COLOR_LIGHT_GREY);
	if(c != '\n'){
		*((uint16_t*)0xb8000 + y*80+x) = c | (ccolor << 8);
		*((uint16_t*)0xb8000 + y*80+x+1) = ' ' | VGA_COLOR_LIGHT_GREY << 12;
		x++;
	}
	if(x >= 80 || c == '\n'){
		*((uint16_t*)0xb8000 + y * 80 + x ) = 0;
//		*((uint16_t*)0xb8000 + y * 80 + x + 1) = 0;
		y++;
		x = 0;
	}
	if(y >= 25){
		for(int i = 0; i < 80*25;i++)
			*((uint16_t*)0xb8000 + i) = *(uint16_t*)((uint16_t*)0xb8000 + i + 80);
		y--;
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
void panic(){
	void *eip;
	void *cs;
	asm("movl -4(%esp),%eax");
	asm("movl 0(%esp),%ebx");
	asm("mov %%eax,%0" :"=m"(eip));
	asm("mov %%ebx,%0" : "=m"(cs));
	puts("\n");
	for(int i = 0; i < 80;i++)
		putc('#');
	puts("->0x");
	putx((uint32_t)eip+(uint32_t)cs);
	puts("\n");
	puts("panic():Disabling Interrupts and Halting CPU\n");
	asm("cli");
	for(int i = 0; i < 80;i++)
		putc('#');
	while(1){
		asm("hlt");
	}
}

void putx(unsigned long n){
	char arr[xlen(n)+1];
	bzero(arr,xlen(n)+1);
	int i = 0;
	if(n == 0){
		puts("0");
		return;
	}
	int size = xlen(n);
	while(size-i-1 >= 0){
		arr[size-i-1]=n%16 < 10 ? n%16+'0' : n%16-10+'A';
		n/=16;
		i++;
	}
	for(int i = 0; i < size;i++)
		putc(arr[i]);

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
	_panic("nothing to do\n");	
	
}
