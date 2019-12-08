#ifndef __MBR_H
#define __MBR_H
#include <stdint.h>
#define PART_OFF 0x1be
#define PART_SIZE 0x10
struct PartTabEnt{
	uint8_t attr;
	uint8_t chs[3];
	uint8_t part_t;
	uint8_t chsL[3];
	uint32_t nsect;
}__attribute__((packed));
#endif
