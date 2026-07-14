SRC_DIR = src/
BIN_DIR = bin/
.PHONY: all
all: load
bootloader:
	nasm $(SRC_DIR)/boot.asm -o $(BIN_DIR)/boot.bin -f bin -g
load:bootloader
	dd if=$(BIN_DIR)/boot.bin of=hda.img bs=512 count=1
run:
	qemu-system-x86_64 -hda hda.img -m 512M
debug:
	qemu-system-x86_64 -hda hda.img -m 512M -s -S
createimg:
	qemu-img create -f raw hda.img 1M
