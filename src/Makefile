TOOL = /opt/arm-toolchain/bin/arm-none-eabi

CC = $(TOOL)-gcc
CP = $(TOOL)-objcopy

BT2 = ../bootloader/bt2 -sb 115200 -s 115200

CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles
SRCS = main.c lpc111x.h printf.c init.c

all: code.bin 

code.bin: code.elf
	$(CP) -O binary $< $@

code.elf: $(SRCS) linker_scriptRAM.ld Makefile 
	$(CC) $(CFLAGS) -Wl,-Tlinker_scriptRAM.ld -Wl,-Map=a.map -o $@ init.c main.c
	$(TOOL)-size $@

clean:
	rm -rf *.elf a.map *~ *.bin *.elf

execute: code.bin
	$(BT2) -d /dev/tty.usbserial-AH00LSB3 -s 115200 -sb 115200 -l code.bin
