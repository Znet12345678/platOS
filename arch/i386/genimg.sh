#!/bin/zsh
sudo fdisk /dev/loop20
sudo mformat -i /dev/loop20p1 
sudo mcopy -i /dev/loop20p1 kernel32.bin ::/

