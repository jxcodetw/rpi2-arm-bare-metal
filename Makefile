CROSS=arm-none-eabi-
CC=$(CROSS)gcc
LD=$(CROSS)ld
OBJCOPY=$(CROSS)objcopy
OBJDUMP=$(CROSS)objdump
CFLAGS=-mcpu=cortex-a7 -fno-pic -static -ffreestanding -std=gnu99 -O2 -Wall -Wextra -I.
ASFLAGS=-mcpu=cortex-a7 -fno-pic -ffreestanding -I.
LDFLAGS=-L.
BUILD_DIR=build

OBJS = \
	asm.o\
	entry.o\
	trap_asm.o\
	swtch.o\
	\
	lib/string.o\
	\
	arm.o\
	picirq.o\
	start.o\
	spinlock.o\
	trap.o\
	timer.o\
	uart.o\
	vm.o\
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

kernel7.img: $(addprefix $(BUILD_DIR)/, $(OBJS)) kernel.ld build/initcode
	$(call quiet-command, $(LD) $(LDFLAGS) \
		-T kernel.ld \
		-o $(BUILD_DIR)/kernel7.elf \
		$(addprefix $(BUILD_DIR)/, $(OBJS)) \
		-b binary build/initcode, "[LINK] $(TARGET_DIR)$@")
	@$(OBJCOPY) $(BUILD_DIR)/kernel7.elf -O binary kernel7.img
	@echo kernel image has been built.
	@$(OBJDUMP) -d $(BUILD_DIR)/kernel7.elf > $(BUILD_DIR)/kernel7.asm

INITCODE_OBJ = initcode.o
$(addprefix $(BUILD_DIR)/,$(INITCODE_OBJ)): initcode.S
	$(call build-directory)
	$(call quiet-command,$(CC) $(ASFLAGS)\
		-nostdinc -I. -c -o $@ $<,"[AS] $(TARGET_DIR)$@")

build/initcode: $(addprefix $(BUILD_DIR)/,$(INITCODE_OBJ))
	$(call quiet-command,$(LD) $(LDFLAGS) \
	 	-N -e start -Ttext 0 -o $@.out $<, "[LINK] $(TARGET_DIR)$@")
	$(call quiet-command,$(OBJCOPY) \
		-S -O binary --prefix-symbols="_binary_$@" $@.out $@, "[OBJCOPY] $(TARGET_DIR)$@")
	@$(OBJDUMP) -S $< > $(BUILD_DIR)/initcode.asm


sd: kernel7.img
	cp kernel7.img /media/removable/USB\ Drive\ 2/

clean:
	rm -rf build
	rm kernel7.img
