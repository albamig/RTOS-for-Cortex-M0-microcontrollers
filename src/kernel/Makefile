TOOL = /opt/arm-toolchain/bin/arm-none-eabi
CC = $(TOOL)-gcc

CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles -c

l = ../lib
b = ../boot

OBJS = main.o systick.o

.PHONY: all debug

all: $(OBJS) 
debug: CFLAGS += -DDEBUG
debug: all

%.o: %.c
	$(CC) $(CFLAGS) $^

clean:
	rm -rf *.o
