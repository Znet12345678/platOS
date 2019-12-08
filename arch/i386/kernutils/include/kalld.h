//Zach Schlotman's idea for platOS
//Public Domain if I decide to share
//None of this is implemented yet but it's nice to think this stuff through
//Kernel As A Library "Linker"
//kalld kal.klib a.o a.os b.o b.os c.o c.os ...
//Structure of kal.klib
// _____________________	
//|			|
//|    Archive Header	|  Assume all types to be uint8_t unless structure or type is specified.
//|_____________________|
//|    Object 1		|
//|---------------------|
//|    Elf Headers      |  This is moreso the layout in memory I don't know the structure of an ELF file off my head
//|---------------------|
//|    .text .data      |  Basic Program Sections for execution and function
//|    .rodata .bss ... |
//|---------------------|
//|     .relocarr       |  Used to relocate all "dangerous" addresses e.g anything that conflicts with the kernel address space that is reserved for platOS
//|  nentries(uint32_t) |  will generate these entries through x86 assembly at first manually. Will make program to do it later  
//|     KlibRelocPM_A1  |  Structures for basic relocation info. If it's more complex it will be stored in .osdata
//|     KlibRelocPM_A2  |
//|     ...		|
//|     KlibRelocPM_AN  |
//|---------------------|
//|    .osdata          |  Could serve a lot of different functions. Will pack all necessary data to execute code from .ostext in .osdata.adata
//|   nentries(uint32_t)|  Also acts as a kind of "program header" for a simple program. The program should be 100% standalone assume no c library or interrupts. Try to pack it small 
//|    nbytesa(uint32_t)|  number of bytes in entry a
//|    aentry(pntrsize) |  entry point(where does execution begin)
//|    apntr(pntrsize)  |  loading address(where does the OS load this function/program
//|    adata(RAW)       |  RAW data to be interpreted by .ostext coorespoding program
//|    aident(char[4])  |  identifiction of the entry a
//|---------------------|
//|   .ostext		|
//|   nentries(uint32_t)|
//|   perms		|  Set to one if it needs access to protected memory kernel will then give it access
//|   nbytesa(uint32_t) |  Number of bytes in a. Entry a in ostext cooresponds to entry a in osdata
//|   RAW		|  RAW i80386 binary code for the function. DO NOT ASSUME ANY SORT OF ENVIRONMENT
//|_____________________|
//|   Object 2		|
//|   ...		|
//|_____________________|
//|   ... 		|
//|_____________________|
//|   Object N		|
//|   ... 		|
//|_____________________|
//Linker is to do the following tasks:
//Set up a table(.relocarr) for all addresses to be relocated AT RUNTIME! DONT FORGET TO SET UP RELOCATIONS for osdata and text as necessary.
//	*out = open(argv[1], O_WRONLY)
//	*readfile F.o,F.os
//	*for section in F.o,F.os
//		*rwsection(out,F.o,F.os,section)
//	*for symbol in F.o,F.os
//		*if addr is in RESERVED_MEM_RANGE(compile time macro at first) then mark relocation addresses
//			*lst.add(new KlibRelocPM(priority,addr,someaddr,someoff))
//	*for symbol in lst
//		writesection(out,symbol,.relocarr)
//Get all of the data needed to set up an OS from this kernel (IDT/IVT,GDT,page tables,etc.) and fill in respective tables.
//	*fd = open(F.os,O_RDONLY)
//	*read(somebuf,F.os)
//	*.data = malloc(F.os.datasize)
//	*.text = malloc(F.os.textsize)
//	*memcpy(.data,somebuf + someoffset,somesize)
//	*memcpy(.text,somebuf + someotheroffset,someothersize)
//Modify object.os to fit data
//	*relocate(.data)
//	*relocate(.text)
//Write object.os to .osdata and .ostext (osfiles are ELFs with only text and data sections. It's safe to ignore the program headers as they will(SHOULD) be relocated
//	wsection(out,.osdata,.ostext)
#ifndef __KALLD_H
#define __KALLD_H
/*TODO*/
#define PRIORITY_LOW 0
#define PRIORITY_MEDIUM 1
#define PRIORITY_HIGH 2
#include <libelf.h>
struct Klib_RelocPM{
	uint8_t priority;//0 forget it if fails, 1 allocate memory elsewhere and attempt to load, 2 panic if fails to reserve newaddr
	unsigned long baseaddr;
	unsigned long newaddr;// Set to 0 for runtime determination
	uint64_t foffset;
#ifdef __KERNEL_BUILD
	void *memoffset;
#endif
};
struct Klib_RelocRM{
	uint16_t segmentbase;
	uint16_t addrbase;
	uint16_t segmentnew;
	uint16_t addrnew;
	uint32_t foffset;
#ifdef __KERNEL_BUILD
	void *memoffset;
#endif
};
#endif
