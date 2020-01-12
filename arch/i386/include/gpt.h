#ifndef __GPT
#define __GPT
#define PART_HDR_LBA 1
#include <stdint.h>
struct Partition_Table_header{
	uint8_t sig[8];
	uint32_t revision;
	uint32_t headerBytes;
	uint32_t CRC32;
	uint32_t resvZero;
	uint64_t currentLBA;
	uint64_t backupLba;
	uint64_t firstUsableLba;
	uint64_t lastUsableLba;
	uint64_t UUID[2];
	uint64_t partEnt_Start;
	uint32_t nPartEnt;
	uint32_t sizeOfPartEnt;
	uint32_t CRC32PartEnt;
}__attribute__((packed));
#define GPT_SIG "EFI PART"
#endif
