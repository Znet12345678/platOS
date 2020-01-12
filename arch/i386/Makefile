LDR_CFILES = loader.c
LDR_OFILES = loader.c.o _start.c.o multiboot.o libmem.c.o
KERNEL_CFILES = kernel32.c _start.c acpi.c initdev.c inter.c libmem.c libata.c libfat.c ps2.c 
KERNEL32_OFILES = kernel32.c.o acpi.c.o _start.o _start.c.o initdev.c.o multiboot.o inter.c.o libmem.c.o libata.c.o libfat.c.o ps2.c.o
CFLAGS = -DEARLY_MEM_PROTECT -Werror -g
all:
	@rm -f *.o
#	@echo "(CC) _start.c.o"
#	@i386-elf-gcc -c _start.c -o _start.c.o -nostdlib -ffreestanding -I. -Iinclude ${CFLAGS} --function-sections
	@for file in ${KERNEL_CFILES} ;do \
		echo "(CC) $$file" ; \
		i386-elf-gcc -c $$file -o $$file.o -nostdlib -ffreestanding -I. -Iinclude ${CFLAGS}  ; \
	    	done
	@echo "(AS) multiboot.o"
	@nasm -f elf multiboot.asm -o multiboot.o
	@echo "(AS) _start.o"
	@nasm -f elf _start.asm -o _start.o
#	@echo "(LD) loader32.elf"
#	@i386-elf-ld -Tllinker.ld ${LDR_OFILES} -o loader32.elf
	@echo "(LD) kernel32.elf"
	@i386-elf-ld -Tlinker32.ld ${KERNEL32_OFILES} -o kernel32.elf -nostdlib
	@i386-elf-objcopy --only-keep-debug kernel32.elf kernel32.sym
#	@echo "(GEN) kernel32.bin"
#	@cat loader32.elf kernel32.elf > kernel32.bin
clean:
	rm -f *.o *.img* *.a *.so* *.1 *.SYS* *.elf *.bin *.sym
