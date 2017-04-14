TOOL = /opt/arm-toolchain/bin/arm-none-eabi
CC = $(TOOL)-gcc
AR = $(TOOL)-ar
RL = $(TOOL)-ranlib

CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles -c

OBJS = delay.o printf.o

.PHONY: all debug

all: lib.a
debug: CFLAGS += -DDEBUG
debug: all

lib.a: $(OBJS)
	$(AR) -r $@ $(OBJS)
	$(RL) $@

%.o: %.c
	$(CC) $(CFLAGS) $^

clean:
	rm -rf *.o lib.a
