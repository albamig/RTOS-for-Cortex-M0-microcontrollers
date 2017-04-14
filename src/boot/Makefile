TOOL = /opt/arm-toolchain/bin/arm-none-eabi
CC = $(TOOL)-gcc

CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles -c

l = ../lib

OBJS = init.o

.PHONY: all debug

all: $(OBJS) 
debug: CFLAGS += -DDEBUG
debug: all

%.o: %.c
	$(CC) $(CFLAGS) $^

clean:
	rm -rf *.o
