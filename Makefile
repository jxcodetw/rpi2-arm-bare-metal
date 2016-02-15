CROSS=arm-none-eabi-
CC=$(CROSS)gcc
OBJCPY=$(CROSS)objcopy
CFLAGS=-mcpu=cortex-a7 -fpic -ffreestanding -std=gnu99 -O2 -Wall -Wextra -I.
ASFLAGS=-mcpu=cortex-a7 -fpic -ffreestanding -I.
BUILD_DIR=build

OBJS = \
	boot.o\
	\
	lib/string.o \
	\
	arm.o\
	uart.o\
	mmu.o\
	timer.o\
	interrupts.o\
	main.o

quiet-command = $(if $(V),$1,$(if $(2),@echo $2 && $1, @$1))
build-directory = $(shell mkdir -p $(BUILD_DIR) $(BUILD_DIR)/lib)


$(BUILD_DIR)/%.o: %.c
	$(call build-directory)
	$(call quiet-command,$(CC) $(CFLAGS) \
		-c -o $@ $<,"[CC] $(TARGET_DIR)$@")

$(BUILD_DIR)/%.o: %.S
	$(call build-directory)
	$(call quiet-command,$(CC) $(ASFLAGS) \
		-c -o $@ $<,"[AS] $(TARGET_DIR)$@")

kernel7.img: $(addprefix $(BUILD_DIR)/, $(OBJS)) linker.ld
	$(call quiet-command, $(CC) -T linker.ld -o $(BUILD_DIR)/kernel7.elf -ffreestanding -O2 -nostdlib $(addprefix $(BUILD_DIR)/, $(OBJS)), "[Build] $(TARGET_DIR)$@")
	@$(OBJCPY) $(BUILD_DIR)/kernel7.elf -O binary kernel7.img
	@echo kernel image has been built.
	@$(CROSS)objdump -d $(BUILD_DIR)/kernel7.elf > $(BUILD_DIR)/kernel7.dump

sd:
	cp target/kernel7.img /media/removable/USB\ Drive/

clean:
	rm -rf build
	rm kernel7.img
