#include <stdio.h>
#include <libio.h>
#include <string.h>
#include <stdlib.h>
#include <libmem.h>
#include <acpi.h>
struct RSDPDescriptor *findRSDP(){
	const char *chk = "RSD PTR ";
	struct RSDPDescriptor *pntr = (struct RSDPDescriptor *)EBDA_BASE;
#ifdef EARLY_MEM_PROTECT
	if(!page_mapped(pntr)){
		puts("mapping address ");
		putx((int)pntr);
		puts("\n");
		identp(pntr);
	}
#endif
	while(1){
		if(memcmp(pntr->sig,chk,sizeof(pntr->sig)) == 0){

			return pntr;
		}
		if(pntr > (struct RSDPDescriptor *)EBDA_END)
			break;
		pntr = (struct RSDPDescriptor *)((uint8_t*)pntr + 0x10);
	}
	pntr = (struct RSDPDescriptor *)BIOS_BASE;
	while(1){
		if(memcmp(pntr->sig,chk,sizeof(pntr->sig)) == 0)
			return pntr;
		pntr = (struct RSDPDescriptor *)((uint8_t*)pntr + 0x10);
		if(pntr > (struct RSDPDescriptor *)BIOS_END)
			_panic("Failed to init ACPI\n");
	}
	return 0;
}
int acpi_getVersion(){
	struct RSDPDescriptor *rsdp = findRSDP();
#ifdef  EARLY_MEM_PROTECT
	if(!page_mapped(rsdp))
		identp(rsdp);
#endif
	if(!rsdp)
		return 0;
	return rsdp->Revision;
}
struct FADT *getFADT(){
	struct RSDPDescriptor *rsdp = findRSDP();
#ifdef EARLY_MEM_PROTECT
	if(!page_mapped(rsdp)){
		puts("Mapping ");
		putx(rsdp);
		puts("\n");
		identp(rsdp);
//		puts("mapping address ");
//		putx((int)rsdp);
//		puts("\n");
	}
#endif
//	puts("0x");
//	putx((int)rsdp);
//	puts("\n");
	struct RSDTa *rsdt = (struct RSDTa *)rsdp->RsdtAddress;
#ifdef EARLY_MEM_PROTECT
	if(!page_mapped(rsdt)){
		identp(rsdt);
		puts("mapping address ");
		putx((int)rsdt);
		puts("\n");
	
	}
#endif
//	puts("0x");
//	putx((int)rsdt);
//	puts("\n");
	int offset = 0;
	int ent = (rsdt->len - sizeof(*rsdt))/4;
	for(int i = 0; i < ent;i++,offset+=sizeof(*rsdt)+rsdt->len){
		struct RSDTa *h = (struct RSDTa*)(uint8_t *)rsdt + offset + sizeof(*rsdt);
#ifdef EARLY_MEM_PROTECT
		if(!page_mapped(h)){
			puts("mapping address ");
			putx((int)h);
			puts("\n");
			identp(h);
		}
#endif
		if(memcmp(h->sig,"FACP",4) == 0)
			return (struct FADT *)h;
	}
}
