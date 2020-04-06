#include <acpi.h>
#include <ps2.h>
#include <stdint.h>
#include <stdlib.h>
#include <libio.h>
#include <stdio.h>
int init = 0;
int ps2_init(){
	debug("ps2_acpi","init");
	struct FADT *fadt = getFADT();
	int version = acpi_getVersion();
	if(version > 0 && !(fadt->BootArchitectureFlags & 2)){
		debug("ps2","Warning: Missing hardware: 8042 PS/2 Controller. OS will likely crash\n");
		puts("BootArchitectureFlags 0x");
		putx(fadt->BootArchitectureFlags);
		puts("\n");
	//	return 0;
	}else{
		debug("ps2","Warning:ACPI Version 1.0 detected");
	}
	debug("ps2","init");
	init = 1;
	int read = 0;
	outb(PS2_COMMAND,0xAD);
	outb(PS2_COMMAND,0xA7);
	inb(0x60);
//	while(!(inb(PS2_STATUS) & 1)){if(inb(PS2_IO) & 1) break;blink();}
	outb(PS2_COMMAND,0x20);
	int byte = inb(PS2_IO);
	byte&= ~(0b01000011);
	outb(PS2_COMMAND,0x60);
	outb(PS2_IO,byte);
	outb(PS2_COMMAND,0xAA);
	int b = inb(PS2_IO);
	if(b != 0x55){
		debug("ps2","I/O Error on PS/2 Controller");
		return 0;
	}
	outb(PS2_COMMAND,0xAE);
	outb(PS2_COMMAND,0xFF);

	while(read != 0xFA){//ACK
		while(!(inb(PS2_STATUS) & 0x10));
		outb(PS2_IO,0xF0);//Scancode set command
		outb(PS2_IO,1);//Use scancode 1
		read = inb(PS2_IO);//Read Status for acknowledge byte
		blink();
	}
}
int sc1[] = {0,0,'1','2','3','4','5','6','7','8','9','0','-','=',1,'\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',3,'a','s','d','f','g','h','j','k','l',';','\'','`',2,'\\','z','x','c','v','b','n','m',',','.','/',3,'*',4,' ',5,6,7,8,9,10,11,12,13,14,15,16,'7','8','9','-','4','2','3','0','.'};
char sc12ascii(char c,int caps){
	return sc1[c] + (caps ? -0x20 : 0);
}
int lock = 0;
int caps = 0;
char ps2_getc(){
	if(!init){
		_panic("PS/2 driver not initialized but has been called. Refusing to countinue!\n\0");
//		return 0;
	}
	while(!(inb(PS2_STATUS) & 1));
	inb(0x60);
a:;	while(!(inb(PS2_STATUS) & 1));
	int b = inb(0x60);
	if(b == 0x3a){
		lock^=1;
	}
	if(b == 0x2a || b == 0x36)
	       	caps=b;	
	if(b == 0xaa || b == 0x36)
		caps = 0;
	if(b == 0x1C)
		return '\n';
	if(sc12ascii(b,0) < 0x20)
		return 0;

	return sc12ascii(b,(caps || lock) && !(caps && lock));

}
int stdin_read(void *buf,int offset,int count){		//Ignores offset
	if(!init){
		_panic("8042 PS/2 driver not initialized but has been called. Refusing to continue!\n\0"); 
	}
	int i = 0;
	while(i < count){
		*(uint8_t*)(buf + i) = ps2_getc();
		i++;
	}
	return 1;
}
char *gets(){
	char *pntr = malloc(1);
	unsigned char c;
	int size = 1;
	while((c = ps2_getc()) != '\n'){
		putc(c);	
		*(pntr + size-1) = c;
		pntr = realloc(pntr,size+1);
		size++;
	}
	return pntr;
}
