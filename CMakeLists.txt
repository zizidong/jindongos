cmake_minimum_required(VERSION 3.10)
project(MyOS C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fPIE -m64 -march=x86-64 -mno-red-zone")

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/gui)
include_directories(${CMAKE_SOURCE_DIR}/drivers)
include_directories(${CMAKE_SOURCE_DIR}/fs)

# Kernel sources
set(KERNEL_SOURCES
    src/kernel.c
    gui/gui.c
    gui/buffer.c
    gui/graphics.c
    gui/window.c
    gui/button.c
    gui/cursor.c
    gui/text.c
    drivers/keyboard.c
    drivers/mouse.c
    fs/ext3.c
)

# Create kernel executable
add_executable(kernel.elf ${KERNEL_SOURCES})

# Linker script
set_target_properties(kernel.elf PROPERTIES
    LINK_FLAGS "-nostdlib -static -pie --no-dynamic-linker -z text -z max-page-size=0x1000 -T ${CMAKE_SOURCE_DIR}/boot/linker.ld"
)

# Custom target for disk image
add_custom_target(disk
    COMMAND dd if=/dev/zero of=disk.img bs=512 count=131072
    COMMAND mformat -i disk.img -F ::
    COMMAND mmd -i disk.img ::/EFI
    COMMAND mmd -i disk.img ::/EFI/BOOT
    COMMAND mcopy -i disk.img ${CMAKE_SOURCE_DIR}/limine/BOOTX64.EFI ::/EFI/BOOT/
    COMMAND mcopy -i disk.img ${CMAKE_SOURCE_DIR}/boot/limine.cfg ::/
    COMMAND mcopy -i disk.img kernel.elf ::/
    DEPENDS kernel.elf
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Custom target for running in QEMU
add_custom_target(run
    COMMAND qemu-system-x86_64 -bios ${CMAKE_SOURCE_DIR}/OVMF.fd -drive file=disk.img,format=raw -m 256M -serial stdio
    DEPENDS disk
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
