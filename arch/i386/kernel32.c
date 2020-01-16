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
		x++;
	}
	if(x >= 80 || c == '\n'){
		if(c == '\n')
			*((uint16_t*)0xb8000 + y * 80 + x)  = ' ' | (ccolor << 8);
		y++;
		x = 0;
	}
	if(y >= 25){
		for(int i = 0; i < 80*25;i++)
			*((uint16_t*)0xb8000 + i) = *(uint16_t*)((uint16_t*)0xb8000 + i + 80);
		y--;
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
int xlen(int n){
	int ret = 0;
	while(n > 0){
		ret++;
		n/=16;
	}
	return ret-1;
}
char *toX(int n){
	char *x = malloc(xlen(n)+1);
	if(n == 0){
		x = realloc(x,2);
		memcpy(x,"0\0",2);
		return x;
	}
	int i = xlen(n);
	while(i >= 0){
		x[i] = (n & 15) >= 0xa ? (n & 0xf) - 0xa + 'a' : (n&15) + '0';
		n>>=4;
		i--;
	}
	return x;
}
void memcpy(void *dest,const void *src,unsigned long n){
	uint8_t *bdest = (uint8_t*)dest;
	uint8_t *bsrc = (uint8_t*)src;
	for(int i = 0; i < n;i++)
		bdest[i] = bsrc[i];
}
void panic(void *msg){
	puts(msg);
	puts("\npanic():Disabling Interrupts and Halting CPU");
	asm("cli");
	asm("hlt");
}
void putx(int n){
	if(n == 0){
		putc('0');
		return;
	}
	char *x = toX(n);
	puts(x);
	free(x);
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
void main(){
	unmap(1);
	init_vidmem();
	puts("platOS Kernel Up...\n");
#ifdef EARLY_MEM_PROTECT
	//libmem_init();
#endif
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
	map_devs(ata);
	puts("[Loader]Select root:");
	panic("Nothing to do");
}
