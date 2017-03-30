# 1 o 2 digitos: Ojo, los ceros a la izquierda significarian OCTAL
VERSION    = 2
# 2 digitos: poner cero a la izquierda si necesario
SUBVERSION = 0

YEAR 	= $(shell date +'%y')
MONTH 	= $(shell date +'%m')
DAY 	= $(shell date +'%d')

VCODE=$(VERSION).$(SUBVERSION) (20$(YEAR)/$(MONTH)/$(DAY))

#TOOL	= arm-linux
TOOL	= /opt/arm-toolchain/bin/arm-none-eabi
CC      = $(TOOL)-gcc
LD      = $(TOOL)-ld -v
AR      = $(TOOL)-ar
AS      = $(TOOL)-as
CP      = $(TOOL)-objcopy
OD	= $(TOOL)-objdump

CFLAGS  = -I./ -fno-common -O2 -g -mtune=arm7tdmi -nostartfiles -static
AFLAGS  = -ahls -mapcs-32
CPFLAGS = -O ihex
ODFLAGS	= -x --syms

all:	bt2

clean:
	-rm -f *~ bt2 boot2.elf boot2m.elf boot2.bin boot2m.bin

bt2:	bt2.c lpcid.c boot2.h boot2m.h Makefile
	gcc -w -O2 -DVERSION="\"$(VCODE)\"" -o $@ $<
    
boot2.h:	boot2.bin hexador
	./hexador boot2.bin >$@

boot2m.h:	boot2m.bin hexador
	./hexador boot2m.bin >$@

hexador:	hexador.c
	gcc -O2 -o $@ $<

boot2.bin:	boot2.elf
	$(CP) -O binary $< $@

boot2.elf:	boot2.s Makefile
	$(CC) $(CFLAGS) -o $@ $<

boot2m.bin:	boot2m.elf
	$(CP) -O binary $< $@

boot2m.elf:	boot2m.s Makefile
	$(CC) $(CFLAGS) -mthumb  -o $@ $<

