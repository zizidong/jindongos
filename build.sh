#!/bin/bash

set -e

BUILD_DIR="build"
KERNEL="$BUILD_DIR/kernel.elf"
DISK="$BUILD_DIR/disk.img"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_status() {
    echo -e "${GREEN}[*]${NC} $1"
}

print_error() {
    echo -e "${RED}[!]${NC} $1"
}

while [[ $# -gt 0 ]]; do
    case $1 in
        --build-only)
            BUILD_ONLY=1
            shift
            ;;
        --run)
            RUN_QEMU=1
            shift
            ;;
        --clean)
            print_status "Cleaning build directory..."
            rm -rf "$BUILD_DIR"
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            exit 1
            ;;
    esac
done

mkdir -p "$BUILD_DIR"

print_status "Building kernel..."
make -j$(nproc) all

if [ $? -ne 0 ]; then
    print_error "Build failed!"
    exit 1
fi

print_status "Kernel built successfully: $KERNEL"

if [ $BUILD_ONLY -eq 0 ]; then
    print_status "Creating disk image..."
    make disk
    
    if [ $? -ne 0 ]; then
        print_error "Disk creation failed!"
        exit 1
    fi
    
    print_status "Disk image created: $DISK"
fi

if [ $RUN_QEMU -eq 1 ]; then
    print_status "Starting QEMU..."
    make run
fi

print_status "Build complete!"
