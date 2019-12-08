/*
 * 8042 PS2 Keyboard controller
 */
#ifndef __PS2_H
#define __PS2_H
#define PS2_IO 0x60 //inb or outb
#define PS2_STATUS 0x64 //inb
#define PS2_COMMAND 0x64 //outb
int ps2_init();
char ps2_getc();
int stdin_read(void *buf, int offset,int count);
#endif
