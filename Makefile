.PHONY: all clean build run disk help

CC = gcc
LD = ld
CFLAGS = -I./include -I./drivers -Wall -Wextra -std=c11 -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fPIE -m64 -march=x86-64 -mno-red-zone
LDFLAGS = -nostdlib -static -pie --no-dynamic-linker -z text -z max-page-size=0x1000 -T ./boot/linker.ld

BUILD_DIR = build
SRC_DIR = src
DRIVERS_DIR = drivers

SOURCES = \
	$(SRC_DIR)/kernel.c \
	$(DRIVERS_DIR)/mouse.c \
	$(DRIVERS_DIR)/keyboard.c

OBJECTS = $(SOURCES:%.c=$(BUILD_DIR)/%.o)
KERNEL = $(BUILD_DIR)/kernel.elf
DISK = $(BUILD_DIR)/disk.img

all: $(KERNEL)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/$(SRC_DIR)
	@mkdir -p $(BUILD_DIR)/$(DRIVERS_DIR)

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJECTS)
	@echo "Linking kernel..."
	@$(LD) $(LDFLAGS) $(OBJECTS) -o $@
	@echo "Kernel built: $@"

disk: $(KERNEL)
	@echo "Creating disk image..."
	@dd if=/dev/zero of=$(DISK) bs=512 count=131072 2>/dev/null
	@echo "Formatting disk..."
	@mformat -i $(DISK) -F :: 2>/dev/null
	@echo "Creating directories..."
	@mmd -i $(DISK) ::/EFI 2>/dev/null
	@mmd -i $(DISK) ::/EFI/BOOT 2>/dev/null
	@echo "Copying files..."
	@mcopy -i $(DISK) ./limine/BOOTX64.EFI ::/EFI/BOOT/ 2>/dev/null
	@mcopy -i $(DISK) ./boot/limine.cfg ::/ 2>/dev/null
	@mcopy -i $(DISK) $(KERNEL) ::/ 2>/dev/null
	@echo "Disk image created: $(DISK)"

run: disk
	@echo "Running QEMU..."
	@qemu-system-x86_64 -bios ./OVMF.fd -drive file=$(DISK),format=raw -m 256M -serial stdio -usb -device usb-mouse -device usb-kbd -display default,show-cursor=on

clean:
	@echo "Cleaning build directory..."
	@rm -rf $(BUILD_DIR)
	@echo "Clean complete"

help:
	@echo "MyOS Build System"
	@echo "================="
	@echo "Targets:"
	@echo "  all   - Build kernel (default)"
	@echo "  disk  - Create bootable disk image"
	@echo "  run   - Build and run in QEMU"
	@echo "  clean - Remove build artifacts"
	@echo "  help  - Show this help message"
