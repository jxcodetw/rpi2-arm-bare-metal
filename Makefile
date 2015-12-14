CROSS=arm-none-eabi-
CC=$(CROSS)gcc
OBJCPY=$(CROSS)objcopy
CFLAG=-mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O2 -Wall -Wextra

OBJS = boot.o \
	   kernel.o \
	   lib/string.o

RPI2OBJS = device/rpi2/uart.o \
	       device/rpi2/mmu.o  \
	       device/rpi2/timer.o \
	       device/rpi2/interrupts.o

QEMUOBJS = device/versatile_pb/uart.o \
		   device/versatile_pb/timer.o \
		   device/versatile_pb/interrupts.o \

%.o: %.c
	$(CC) $(CFLAG) -c -o $@ $<

%.o: %.S
	$(CC) -mcpu=cortex-a7 -fpic -ffreestanding -c -o $@ $<

RPI2: $(OBJS) $(RPI2OBJS) rpi2.ld
	$(CC) -D RPI2 -T rpi2.ld -o kernel7.elf -ffreestanding -O2 -nostdlib $(OBJS) $(RPI2OBJS)
	$(OBJCPY) kernel7.elf -O binary kernel7.img

kernel_qemu: $(OBJS) $(QEMUOBJS) versatile_pb.ld
	$(CC) -T versatile_pb.ld -o kernel_qemu.elf -ffreestanding -O2 -nostdlib $(OBJS) $(QEMUOBJS)

qemu: kernel_qemu
	@echo "Press Ctrl-A and then X to terminate QEMU session\n"
	@qemu-system-arm -M versatilepb -m 128 -cpu cortex-a9 -nographic -kernel kernel_qemu.elf

clean:
	rm -rf *.o
	rm -rf lib/*.o
	rm -rf device/rpi2/*.o
	rm -rf device/rpi2/*.gch
	rm -rf device/versatile_pb/*.o
	rm -rf device/versatile_pb/*.gch
	rm kernel_qemu.elf kernel7.elf kernel7.img
