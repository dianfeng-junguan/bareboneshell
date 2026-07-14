SRC_DIR = src/
BIN_DIR = bin/
.PHONY: all
all: load
bootloader:
	nasm $(SRC_DIR)/boot.asm -o $(BIN_DIR)/boot.bin -f bin -g
program: bootloader
	nasm $(SRC_DIR)/kernelhead.asm -o $(BIN_DIR)/kernelhead.o -f elf32 -g
	gcc -c $(SRC_DIR)/main.c -o $(BIN_DIR)/main.o -g -m32 -fno-pie -nostdlib 
	ld $(BIN_DIR)/kernelhead.o $(BIN_DIR)/main.o -e 0x7e00 -m elf_i386 --oformat elf32-i386 -o $(BIN_DIR)/bareshell.elf
	objcopy $(BIN_DIR)/bareshell.elf $(BIN_DIR)/bareshell.bin -I elf32-i386 -O binary
load:program
	dd if=$(BIN_DIR)/boot.bin of=hda.img bs=512 count=1
	dd if=$(BIN_DIR)/bareshell.bin of=hda.img bs=512 seek=1
run:
	qemu-system-x86_64 -hda hda.img -m 512M
debug:
	qemu-system-x86_64 -hda hda.img -m 512M -s -S
createimg:
	qemu-img create -f raw hda.img 1M
