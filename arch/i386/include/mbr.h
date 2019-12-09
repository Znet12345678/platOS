#ifndef __MBR_H
#define __MBR_H
#include <stdint.h>
#define MBR_BS_OFFSET 0
#define MBR_BS_SIZE 440
#define MBR_ID_OFFSET 0x1b8
#define MBR_ID_SIZE 4
#define MBR_RES_OFFSET 0x1BC
#define MBR_RES_SIZE 2
#define MBR_FIRST_PART 0x1BE
#define MBR_SECOND_PART 0x1CE
#define MBR_THIRD_PART 0x1DE
#define MBR_FOURTH_PART 0x1EE
#define MBR_PART_SIZE 0x10
#define SIG 0xaa55
struct partEnt{
	uint8_t attr;
	uint8_t CHSS[3];
	uint8_t pt;
	uint8_t CHSL[3];
	uint32_t lba;
	uint32_t nsect;

}__attribute__((packed));
#endif
