#!/bin/zsh
sudo losetup /dev/loop20 bootloader.bin

sudo fdisk /dev/loop20
sudo dd if=bs.bin of=/dev/loop20 seek=36 bs=1 conv=notrunc
sudo mformat -i /dev/loop20p1 
sudo mcopy -i /dev/loop20p1 kernel32.bin ::/
sudo losetup -d /dev/loop20

