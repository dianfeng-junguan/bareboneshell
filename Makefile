SRC_DIR = src
BIN_DIR = bin
CC_SOURCES := $(wildcard $(SRC_DIR)/*.c)
CC_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(CC_SOURCES))
KERNEL_CFLAGS = -g -m32 -fno-pie -nostdlib -fno-leading-underscore -fno-builtin -ffreestanding -Wall -Wextra -Werror -nostartfiles -fno-stack-protector -msoft-float -mno-80387 -mno-sse -mno-mmx -MMD -MP

ifeq ($(shell uname), Darwin)
	CC = i686-elf-gcc
	LD = i686-elf-ld
	OBJCOPY = i686-elf-objcopy
	OBJ_TARGET = i686-elf
	NASM_TARGET = elf32
else
	ifeq ($(shell uname), Linux)
		CC = gcc
		LD = ld
		OBJCOPY = objcopy
		OBJ_TARGET = elf_i386
		NASM_TARGET = elf32
	else
		CC = x86_64-elf-gcc
		LD = x86_64-elf-ld
		OBJCOPY = x86_64-elf-objcopy
		OBJ_TARGET = elf_i386
		NASM_TARGET = elf32
	endif
endif 
.PHONY: all
all: load
bootloader:
	nasm $(SRC_DIR)/boot.asm -o $(BIN_DIR)/boot.bin -f bin -g

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@ $(KERNEL_CFLAGS)

-include $(wildcard $(BIN_DIR)/*.d)

$(BIN_DIR)/kernelhead.o: $(SRC_DIR)/kernelhead.asm
	nasm $< -o $@ -f $(NASM_TARGET) -g

$(BIN_DIR)/keyboard.o: $(SRC_DIR)/keyboard.asm
	nasm $< -o $@ -f $(NASM_TARGET) -g

program: bootloader $(BIN_DIR)/kernelhead.o $(BIN_DIR)/keyboard.o $(CC_OBJECTS)
	$(LD) $(BIN_DIR)/kernelhead.o $(BIN_DIR)/keyboard.o $(CC_OBJECTS) -Ttext 0x7e00 -e kernelhead_start -m $(OBJ_TARGET) -o $(BIN_DIR)/bareshell.elf
	$(OBJCOPY) $(BIN_DIR)/bareshell.elf $(BIN_DIR)/bareshell.bin -O binary
load:program
	dd if=$(BIN_DIR)/boot.bin of=hda.img bs=512 count=1 conv=notrunc
	dd if=$(BIN_DIR)/bareshell.bin of=hda.img bs=512 seek=1 conv=notrunc
run:
	qemu-system-x86_64 -hda hda.img -m 512M
debug:
	qemu-system-x86_64 -hda hda.img -m 512M -s -S
createimg:
	qemu-img create -f raw hda.img 1M
