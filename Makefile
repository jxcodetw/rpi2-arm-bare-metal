CROSS=arm-none-eabi-
CC=$(CROSS)gcc
OBJCPY=$(CROSS)objcopy
CFLAG=-mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O2 -Wall -Wextra
BUILD_DIR=build

OBJS = \
	lib/string.o \
	\
	boot.o\
	kernel.o\
    uart.o\
	mmu.o\
	timer.o\
	interrupts.o

build-directory = $(shell mkdir -p $(BUILD_DIR) $(BUILD_DIR)/lib)


$(BUILD_DIR)/%.o: %.c
	$(call build-directory)
	$(CC) $(CFLAG) -c -o $@ $<

$(BUILD_DIR)/%.o: %.S
	$(call build-directory)
	$(CC) -mcpu=cortex-a7 -fpic -ffreestanding -c -o $@ $<

RPI2: $(addprefix $(BUILD_DIR)/, $(OBJS)) linker.ld
	$(CC) -T linker.ld -o $(BUILD_DIR)/kernel7.elf -ffreestanding -O2 -nostdlib $(addprefix $(BUILD_DIR)/, $(OBJS)) 
	$(OBJCPY) $(BUILD_DIR)/kernel7.elf -O binary kernel7.img

clean:
	rm -rf build
	rm kernel7.elf kernel7.img
