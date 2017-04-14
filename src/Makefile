TOOL = /opt/arm-toolchain/bin/arm-none-eabi
LD = $(TOOL)-gcc
CP = $(TOOL)-objcopy

LDFLAGS = -Wall -g -Os -mcpu=cortex-m0 -mthumb -nostartfiles

BT2 = ../bootloader/bt2

l = lib/
b = boot/
k = kernel/

DIRS = $(k) $(b) $(l)
OBJS = $(k)*.o $(b)*.o $(l)*.a

EXECNAME = rtos

.PHONY: all debug $(DIRS)

all: $(EXECNAME).bin
debug: LDFLAGS += -DDEBUG
debug: MKTGT = debug
debug: all

%.bin: %.elf
	$(CP) -O binary $< $@

%.elf: $(DIRS)
	$(LD) $(LDFLAGS) -Wl,-T$(b)linker_scriptRAM.ld -Wl,-Map=a.map $(OBJS) -o $@ 
	$(TOOL)-size $@

$(DIRS):
	cd $@ && make $(MKTGT)

execute: 
	$(BT2) -d /dev/tty.usbserial-AH00LSB3 -s 115200 -sb 115200 -l $(EXECNAME).bin

clean:
	rm -rf *.elf a.map *~ *.bin *.elf

cleanall:
	cd $(l) && make clean
	cd $(b) && make clean
	cd $(k) && make clean
	make clean
