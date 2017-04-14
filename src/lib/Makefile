TOOL = /opt/arm-toolchain/bin/arm-none-eabi
CC = $(TOOL)-gcc
AR = $(TOOL)-ar
RL = $(TOOL)-ranlib

CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles -c

OBJS = delay.o printf.o

all: lib.a

lib.a: $(OBJS)
	$(AR) -r $@ $(OBJS)
	$(RL) $@

%.o: %.c
	$(CC) $(CFLAGS) $^

clean:
	rm -rf *.o lib.a
