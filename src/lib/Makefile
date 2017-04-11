TOOL = /opt/arm-toolchain/bin/arm-none-eabi
CC = $(TOOL)-gcc
AR = $(TOOL)-ar

CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles -c
OBJS = delay.o printf.o

all: lib.a

lib.a: $(OBJS)
	ar -r $@ $(OBJS)

delay.o: delay.c
	$(CC) $(CFLAGS) $^

printf.o: printf.c
	$(CC) $(CFLAGS) $^

clean:
	rm *.o
