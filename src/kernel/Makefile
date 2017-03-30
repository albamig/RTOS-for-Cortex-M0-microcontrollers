TOOL = /opt/arm-toolchain/bin/arm-none-eabi

CC = $(TOOL)-gcc
CP = $(TOOL)-objcopy

BT2 = ../../bootloader/bt2 -sb 115200 -s 115200

l = ../lib
CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles
SRCS = main.c systick.c
LIB = $(l)/lpc111x.h $(l)/printf.c $(l)/init.c

all: code.bin 

code.bin: code.elf
	$(CP) -O binary $< $@

code.elf: $(SRCS) $(LIB) linker_scriptRAM.ld Makefile 
	$(CC) $(CFLAGS) -Wl,-Tlinker_scriptRAM.ld -Wl,-Map=a.map -o $@ $(l)/init.c systick.c $(l)/printf.c main.c
	$(TOOL)-size $@

clean:
	rm -rf *.elf a.map *~ *.bin *.elf

execute: code.bin
	$(BT2) -d /dev/tty.usbserial-AH00LSB3 -s 115200 -sb 115200 -l code.bin
