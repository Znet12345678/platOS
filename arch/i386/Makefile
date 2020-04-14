DEPS=include/* Makefile
CC=i386-elf-gcc
CXX=i386-elf-g++
CC16=bcc
AS16=nasm -f as86
ASBIN=nasm -f bin
AS=nasm -f elf

IMAGETOOL=qemu-img
DD=dd
CFLAGS=-nostdlib -Iinclude -ffreestanding -DKERNEL_IDENT=\"v0-Alpha\" -DKERNEL_ARCH="\"i386(x86)\"" -DRELEASE="\"DEBUG\"" -DEARLY_MEM_PROTECT 
LDFLAGS=-nostdlib -Tlinker32.ld
all: Makefile bootloader.bin
kernel: kernel32.elf 
bootloader.bin: kernel32.elf bs.bin padding o16 bs2.bin
	$(IMAGETOOL) create tail.img 32M
	qemu-img create bs.img 512B
	dd if=bs.bin of=bs.img bs=1 seek=36 conv=notrunc
	cat bs.img bs2.bin tail.img > bootloader.bin
	cat pre.bin padding.bin kernel32.elf > kernel32.bin
kernel32.elf.of: Makefile kernel32.o strchrnul.o stub.o llds.o acpi.o __start.o initdev.o vfs.o multiboot.o inter.o fat.o libmem.o libata.o ps2.o strtok_r.o strcspn.o memset.o strspn.o FSInstance.o fatInstance.o
%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CFLAGS)
kernel32.elf: kernel32.elf.of kernel32
%.o: %.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)
%.o: %.asm $(DEPS)
	$(AS) $< -o $@
o16: bs.bin putc.o16 goprot.o16 gdt.o16 chain.o16 start.o16 _start.aout ldivmod.aout bcc_long.aout 
%.o32: %.asm $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
%.bin: %.asm $(DEPS)
	$(ASBIN) $< -o $@
%.o16: %.asm 
	$(AS16) $< -o $@
bs2.bin: *.o16 *.aout
	ld86 -T0x1000 -0 -7 -s *.o16 *.aout -o bs2.bin
%.aout: %.c $(DEPS) Makefile
	$(CC16) -ansi -0 -7 -c -o $@ $< -Iinclude -DL___idivu -DL___lcmpl -DL___ldivl -DL___lmodl -DL___ldivl -DL___landl
kernel32.bin: padding kernel32
padding: kernel32.elf
	cp kernel32.elf kernel32.sym
	strip kernel32.elf
	i386-elf-gcc -c pre.c -o pre.oelf -nostdlib -Iinclude -Werror -ffreestanding
	i386-elf-gcc pre.oelf -o pre.elf -Tlinker.3.ld -nostdlib
	i386-elf-objcopy -O binary pre.elf pre.bin
	gcc createpad.c -o createpad
	./createpad pre.bin padding.bin
kernel32: kernel32.elf.of
	$(CC) $(LDFLAGS) $(CFLAGS) *.o -o kernel32.elf
clean:
	rm -f createpad *.o32 *.o *.o16 *.bin *.a *.BIN *.ELF *.aout *.img *.so *.SYS *.elf *.sym
