#include <stdio.h>
#include <libmem.h>
#include <stdint.h>
#include <libata.h>
#include <libio.h>
uint8_t bus[4] = {0,0,0,0};
uint8_t drives[8] = {0,0,0,0,0,0,0,0};
uint32_t lastcommand;
ata_dev_t **libata_init(){
	ata_dev_t **ret = malloc(8*sizeof(*ret));
	debug("libata","init");
	debug("libata","Detecting ATA buses...");
	bus[0] = inb(0x1f7) == 0xff ? 0 : 1;
	bus[1] = inb(0x177) == 0xff ? 0 : 1;
	bus[2] = inb(0x1e8) == 0xff ? 0 : 1;
	bus[3] = inb(0x168) == 0xff ? 0 : 1;
	debug("libata","Detecting ATA compliant drives...\n");
	if(bus[0]){
		drives[0] = ident(0x1f0,0xa0);
		drives[1] = ident(0x1f0,0xb0);
	}
	if(bus[1]){
		drives[2] = ident(0x170,0xa0);
		drives[3] = ident(0x170,0xb0);
	}
	if(bus[2]){
		drives[4] = ident(0x1e8,0xa0);
		drives[5] = ident(0x1e8,0xb0);
	}
	if(bus[3]){
		drives[6] = ident(0x168,0xa0);
		drives[7] = ident(0x168,0xb0);
	}
	debug("libata","Found drives:");
	for(int i = 0; i < 8;i++){
		if(drives[i]){
			puts("Drive ");
			putc('0'+i);
			puts(" up!\n");
			ret[i] = malloc(sizeof(*ret[i]));
			ret[i]->ioaddr = i < 2 ? 0x1f0 : 0x170;
			ret[i]->slavebyte = i % 2 == 0 ? 0xa0 : 0xb0;
		}
	}
	return ret;

}
uint8_t ident(uint16_t port,uint8_t slavebyte){
	debug("libata","identify");
	outb(port+6,slavebyte);
	for(int i = port+2; i <= port+5;i++)
		outb(i,0);
	outb(port+7,0xec);
	uint8_t val = inb(port+7);
	if(val == 0){
		debug("libata","Error 0x00");
		return 0;
	}
a:;
	while(((val >> 7) & 1)){
		val = inb(port+7);
	}
	uint16_t chk = inb(0x1f4) | inb(0x1f5) << 8;
	if(chk){
		debug("libata","Error 0x01");
		return 0;
	}
	if(val & 1){
		debug("libata","Error 0x02");
		return 0;
	}
	if(!((val >> 3) & 1)){
		val = inb(port+7);
		goto a;
	}
	for(int i = 0; i < 256;i++)
		inw(port);
	lastcommand = port | slavebyte << 16 | 0xec << 24;
	debug("libata","Success");
	return 1;
}

int poll(uint16_t port){
	uint8_t val = inb(port + STATUS_OFFSET);
	while(1){
		if(!(val & 0x80) && val & 8)
			break;
		if(val & 1 || val & 0x20)
			return -1;
		val = inb(port +STATUS_OFFSET);
	}
	return 0;

}
void _delay(){
	for(int i = 0; i < 4096;i++);//Extra long delay
}
void ata_generic24(uint16_t port,uint8_t slave,unsigned int lba,unsigned int n,uint8_t op){
	outb(port+6,slave | ((lba >> 24) & 0xf));
	outb(port+2,n);
	outb(port+3,lba&0xff);
	outb(port+4,(lba >> 8) & 0xff);
	outb(port+5,(lba >> 16) & 0xff);
	outb(port+7,op);
}
int ata_read24(uint16_t port,uint8_t slave,void *buf,unsigned int lba,unsigned int n){
	ata_generic24(port,slave == 0xa0 ? 0xe0 : 0xf0,lba,n,0x20);
	unsigned int offset = 0;
	while(n > 0){
		_delay();
		int chk = poll(port);
		if(chk < 0)
			return -1;
		for(int i = 0; i < 256;i++,offset++)
			*((uint16_t*)buf + offset) = inw(port);
		n--;
	}
}
int ata_write24(uint16_t port,uint8_t slave,void *buf,unsigned int lba,unsigned int n){
	ata_generic24(port,slave == 0xa0 ? 0xe0 : 0xf0,lba,n,0x30);
	unsigned int offset = 0;
	while(n > 0){
		_delay();
		int chk = poll(port);
		if(chk < 0)
			return -1;
		for(int i = 0; i < 256;i++,offset++){
			outw(port,*((uint16_t*)buf + offset));
		}
		n--;
		ata_generic24(port,slave,lba,n,0xe7);
		uint8_t val = inb(port+STATUS_OFFSET);
		while(val & 0x80)val = inb(port+STATUS_OFFSET);
	}
}
void ata_generic48(uint16_t port,uint8_t slave,unsigned long lba, unsigned short sc,unsigned int cmd){
	outb(port+6,slave);
	outb(port+2,(sc >> 8) & 0xff);
	outb(port+3,((uint8_t*)&lba)[3]);
	outb(port+4,((uint8_t*)&lba)[4]);
       	outb(port+5,((uint8_t*)&lba)[5]);
	outb(port+2,(sc) & 0xff);
	outb(port+3,((uint8_t*)&lba)[0]);
	outb(port+4,((uint8_t*)&lba)[1]);
	outb(port+5,((uint8_t*)&lba)[2]);
	outb(port+7,cmd);
}
int ata_read48(uint16_t port,uint8_t slave,void *buf,unsigned long lba,unsigned short sc){
	ata_generic48(port,slave == 0xa0 ? 0x40 : 0x50,lba,sc,0x24);
	unsigned int offset = 0;
	while(sc > 0){
		_delay();
		int chk = poll(port);
		if(chk < 0){
			puts("I/O Error\n");
			return -1;
		}
		for(int i = 0; i < 256;i++,offset++){
			*((uint16_t*)buf + offset) = inw(port);

		}
		sc--;
	}
}
int ata_write48(uint16_t port,uint8_t slave,void *buf,unsigned long lba,unsigned short sc){
	ata_generic48(port,slave == 0xa0 ? 0x40 :0x50,lba,sc,0x34);
	unsigned int offset = 0;
	while(sc > 0){
		_delay();
		int chk = poll(port);
		if(chk < 0)
			return -1;
		for(int i = 0; i < 256;i++,offset++){
			*((uint16_t*)(buf) + offset) = inw(port);
		}
		ata_generic24(port,slave,lba,sc,0xe7);
		uint8_t v = inb(port +STATUS_OFFSET);
		while(v & 0x80) v = inb(port+STATUS_OFFSET);
	}
}
int aread24(struct ata_dev *dev,void *buf,unsigned int lba,unsigned int count){
	return ata_read24(dev->ioaddr,dev->slavebyte,buf,lba,count);
}
int awrite24(struct ata_dev *dev,void *buf,unsigned int lba,unsigned int count){
	return ata_write24(dev->ioaddr,dev->slavebyte,buf,lba,count);
}
int aread48(struct ata_dev *dev,void *buf,unsigned int lba,unsigned int count){
	return ata_read48(dev->ioaddr,dev->slavebyte,buf,lba,count);
}
int awrite48(struct ata_dev *dev,void *buf,unsigned int lba,unsigned int count){
	return ata_write48(dev->ioaddr,dev->slavebyte,buf,lba,count);
}
