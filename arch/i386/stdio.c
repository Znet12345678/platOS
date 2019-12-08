#include <stdio.h>
#include <tty.h>
void stdout_write(void *buf,int offset,int n){
	for(int i = 0; i < n;i++){
		*(uint16_t*)(0xb8000 + offset + i) = *(uint16_t*)(buf + i) | (uint16_t)ccolor << 8;
	}
}	
