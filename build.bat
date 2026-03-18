@echo off
setlocal enabledelayedexpansion

set ROOT=/mnt/c/Users/User/Desktop/dos\ v2
set WROOT=C:\Users\User\Desktop\dos v2
set BDIR=!ROOT!/build
set QEMU=C:\Program Files\qemu\qemu-system-x86_64.exe

if not exist "!QEMU!" (
    echo Error: QEMU not found
    pause
    exit /b 1
)

set CFLAGS=-std=c2x -ffreestanding -fno-stack-protector -fno-stack-check -fno-lto -fPIE -m64 -march=x86-64 -mno-red-zone -Wall -Wextra
set INC=-I!ROOT!/include -I!ROOT!/drivers -I!ROOT!/gui

wsl mkdir -p !BDIR!
wsl bash -c "apt-get install -y musl-tools gdisk mtools 2>/dev/null | tail -1"

echo [1/9] kernel.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/src/kernel.c -o !BDIR!/kernel.o
if errorlevel 1 goto fail

echo [2/9] syscall.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/src/syscall.c -o !BDIR!/syscall.o
if errorlevel 1 goto fail

echo [3/9] loader.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/src/loader.c -o !BDIR!/loader.o
if errorlevel 1 goto fail

echo [4/9] vfs.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/src/vfs.c -o !BDIR!/vfs.o
if errorlevel 1 goto fail

echo [5/9] pthread.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/src/pthread.c -o !BDIR!/pthread.o
if errorlevel 1 goto fail

echo [6/9] thread.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/src/thread.c -o !BDIR!/thread.o
if errorlevel 1 goto fail

echo [7/9] gtk.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/gui/gtk.c -o !BDIR!/gtk.o
if errorlevel 1 goto fail

echo [8/9] mouse.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/drivers/mouse.c -o !BDIR!/mouse.o
if errorlevel 1 goto fail

echo [9/9] keyboard.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/drivers/keyboard.c -o !BDIR!/keyboard.o
if errorlevel 1 goto fail

echo [9/9] libc.c
wsl musl-gcc !CFLAGS! !INC! -c !ROOT!/src/libc.c -o !BDIR!/libc.o
if errorlevel 1 goto fail

echo [link] kernel.elf
wsl ld -nostdlib -static -z max-page-size=0x1000 -T !ROOT!/boot/linker.ld !BDIR!/kernel.o !BDIR!/syscall.o !BDIR!/loader.o !BDIR!/vfs.o !BDIR!/pthread.o !BDIR!/thread.o !BDIR!/gtk.o !BDIR!/mouse.o !BDIR!/keyboard.o !BDIR!/libc.o -o !BDIR!/kernel.elf
if errorlevel 1 goto fail

echo [disk] building disk image
wsl bash /mnt/c/Users/User/Desktop/dos\ v2/build/mkdisk.sh
if errorlevel 1 goto fail

echo [run] QEMU
start "" "!QEMU!" -bios "!WROOT!\OVMF.fd" -drive file="!WROOT!\build\disk.img",format=raw -m 512M -usb -device usb-mouse -device usb-kbd

goto end

:fail
echo Build failed!
pause
exit /b 1

:end
