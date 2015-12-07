CROSS=arm-none-eabi-
CC=$(CROSS)gcc
OBJCPY=$(CROSS)objcopy
CFLAG=-mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O2 -Wall -Wextra

PROG=kernel.img
OBJS = boot.o \
	   kernel.o \
	   lib/string.o \
	   device/uart.o \
	   device/mmu.o

%.o: %.c
	$(CC) $(CFLAG) -c -o $@ $<

%.o: %.S
	$(CC) -mcpu=cortex-a7 -fpic -ffreestanding -c -o $@ $<


$(PROG): $(OBJS) linker.ld
	$(CC) -T linker.ld -o kernel.elf -ffreestanding -O2 -nostdlib $(OBJS)
	$(OBJCPY) kernel.elf -O binary kernel.img

sd:
	cp kernel.img /media/removable/USB\ Drive

clean:
	rm -rf *.o
	rm -rf lib/*.o
	rm -rf device/*.o
	rm -rf device/*.gch
	rm kernel.elf
	rm kernel.img
