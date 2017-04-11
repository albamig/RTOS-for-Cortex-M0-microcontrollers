TOOL = /opt/arm-toolchain/bin/arm-none-eabi
CC = $(TOOL)-gcc
LD = $(TOOL)-gcc

l = ../lib
CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles -c
LDFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles 

OBJS = main.o systick.o

all: $(OBJS) 
	$(LD) $(LDFLAGS) $(OBJS) $(l)/lib.a -Wl,-T../boot/linker_scriptRAM.ld -o kernel

main.o: main.c
	$(CC) $(CFLAGS) $^

systick.o: systick.c
	$(CC) $(CFLAGS) $^

clean:
	rm *.o kernel
