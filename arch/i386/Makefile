size=1024
LDR_CFILES = loader.c
LDR_OFILES = loader.c.o _start.c.o multiboot.o libmem.c.o
KERNEL_CFILES = kernel32.c llds.c  acpi.c initdev.c inter.c libmem.c vfs.c libata.c  ps2.c fat.c 
KERNEL32_OFILES = kernel32.c.o llds.c.o acpi.c.o _start.o  initdev.c.o vfs.c.o multiboot.o inter.c.o fat.c.o libmem.c.o libata.c.o  ps2.c.o
CFLAGS = -DKERNEL_IDENT=\"v0-ALPHA\" -DKERNEL_ARCH="\"i386(x86)\"" -DRELEASE="\"Protected DEBUG\"" -DDEBUG -DEARLY_MEM_PROTECT -Werror -g  
all:
	@rm -f *.o
	i386-elf-gcc -c loader.c -o loader.o -Iinclude -ffreestanding -nostdlib
	bcc -0   -c bcc_long.c -o bcc_long.o  -DL___idivu -DL___lcmpl -DL___ldivl -DL___lmodl -DL___ldivl -DL___landl 
	bcc -0  -c  ldivmod.c -o ldivmod.o
	nasm -f bin bs.asm -o bs.bin
	nasm -f as86 putc.asm -o putc.o
	nasm -f as86 goprot.asm -o goprot.o
	nasm -f as86 gdt.asm -o gdt.o
	nasm -f as86 chain.asm -o chain.o
	nasm -f as86 start.asm -o start.o
	qemu-img create padding.bin 32M
	bcc -0   -ansi -Iinclude -c _start.c -o _start.c.o 
	ld86 -T0x1000   -0 -7  -s chain.o _start.c.o start.o putc.o gdt.o goprot.o bcc_long.o ldivmod.o -o bs2.bin
	ld86 -T0x1000 -0 chain.o _start.c.o start.o putc.o gdt.o goprot.o bcc_long.o ldivmod.o -o bs2.sym
	dd if=bs.bin of=bootloader.bin seek=62 bs=1 conv=notrunc
	dd if=bs2.bin of=bootloader.bin seek=1 bs=512 conv=notrunc
	cat bs.bin bs2.bin padding.bin > bootloader.bin 
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
	@echo "(AS) cmdline.o"
	@nasm -f elf cmdline.asm -o cmdline.o
	@echo "(LD) kernel32.elf"
	@i386-elf-ld -Tlinker32.ld ${KERNEL32_OFILES}  -o kernel32.sym -nostdlib
	@cp kernel32.sym kernel32.elf
	@echo "(STRIP) kernel32.elf"
	@strip kernel32.elf
	@echo "(GEN) kernel32.bin"
	@i386-elf-gcc -c pre.c -o pre.o -nostdlib -Iinclude -Werror -ffreestanding
	@i386-elf-gcc pre.o -o pre.elf -Tlinker.3.ld -nostdlib
	@i386-elf-objcopy -O binary pre.elf pre.bin
#	@echo $((512 - $(shell stat -L -c %s pre.bin))
	@dd if=/dev/zero of=padding.2.bin bs=1 count=$$(($(size) - $(shell stat -L -c %s pre.bin)))
	@cat pre.bin padding.2.bin kernel32.elf > kernel32.bin
#	@i386-elf-objcopy -O binary kernel32.elf KERNEL.BIN
#	@i386-elf-objcopy --only-keep-debug kernel32.elf kernel32.sym
#	@echo "(GEN) kernel32.bin"
#	@cat loader32.elf kernel32.elf > kernel32.bin
bootloader:
	nasm -f bin bs.asm -o bs.o

clean:
	rm -f *.BIN *.ELF *.aout *.o *.img* *.a *.so*  *.1 *.SYS* *.elf *.bin *.sym
