KERNEL_CFILES = kernel32.c acpi.c inter.c libmem.c libata.c libfat.c ps2.c 
KERNEL32_OFILES = kernel32.c.o acpi.c.o inter.c.o multiboot.o libmem.c.o libata.c.o libfat.c.o ps2.c.o
CFLAGS = -DEARLY_MEM_PROTECT
all:
	@for file in ${KERNEL_CFILES} ;do \
		echo "(CC) $$file" ; \
		i386-elf-gcc -c $$file -o $$file.o -nostdlib -ffreestanding -I. -Iinclude ${CFLAGS}  ; \
	    	done
	@echo "(AS) multiboot.o"
	@nasm -f elf multiboot.asm -o multiboot.o
	@echo "(LD) kernel32.elf"
	@i386-elf-ld -Tlinker32.ld ${KERNEL32_OFILES} -o kernel32.elf -nostdlib
clean:
	rm -f *.o *.img* *.a *.so* *.1 *.SYS* *.elf *.bin
