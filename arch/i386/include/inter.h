#ifndef __INT_H
#define __INT_H
#include <stdint.h>
struct idtr{
	uint16_t limit;
	uint32_t base;
}__attribute__((packed));
struct IDTDescr{
	uint16_t offset_1;
	uint16_t selector;
	uint8_t zero;
	uint8_t type_attr;
	uint16_t offset_2;
};
//struct IDTDescr arr[256];
#endif
