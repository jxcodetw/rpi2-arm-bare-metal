CROSS=arm-none-eabi-
CC=$(CROSS)gcc
OBJCPY=$(CROSS)objcopy
CFLAG=-mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O2 -Wall -Wextra


kernel.img: boot.o uart.o mmu.o kernel.o string.o
	$(CC) -T linker.ld -o kernel.elf -ffreestanding -O2 -nostdlib boot.o kernel.o device/uart.o device/mmu.o lib/string.o
	$(OBJCPY) kernel.elf -O binary kernel.img

boot.o: boot.s
	$(CC) -mcpu=cortex-a7 -fpic -ffreestanding -c boot.s -o boot.o

uart.o: device/uart.c device/uart.h
	$(CC) $(CFLAG) -c device/uart.c -o device/uart.o

mmu.o: device/mmu.c device/mmu.h
	$(CC) $(CFLAG) -c device/mmu.c -o device/mmu.o

string.o: lib/string.c lib/string.h
	$(CC) $(CFLAG) -c lib/string.c -o lib/string.o

kernel.o: kernel.c
	$(CC) $(CFLAG) -c kernel.c -o kernel.o

sd:
	cp kernel.img /media/removable/USB\ Drive
clean:
	rm -rf *.o
	rm -rf lib/*.o
	rm -rf device/*.o
	rm kernel.elf
	rm kernel.img
