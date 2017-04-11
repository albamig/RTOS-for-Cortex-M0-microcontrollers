TOOL = /opt/arm-toolchain/bin/arm-none-eabi
CC = $(TOOL)-gcc

CFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles -c
OBJS = init.o

all: $(OBJS) 

init.o: init.c
	$(CC) $(CFLAGS) $^

clean:
	rm *.o
