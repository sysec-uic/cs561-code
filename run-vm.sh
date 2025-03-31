#!/bin/bash

# The QEMU binary for running the VM and the default CPU and memory setting.
QEMU_BIN=qemu-system-x86_64
NCPU=2
MEMSIZE=2G

KNL_SRC=~/workspace/linux       # TODO: Change with your kernel base location
BZIMAGE=${KNL_SRC}/arch/x86/boot/bzImage
CMDLINE="nokaslr console=ttyS0 root=/dev/sda1 rw debug"
IMG=ubuntu.img

USE_CUSTOM_KERNEL=false

# Usage function
usage() {
    echo "Usage: $0 [-k|--kernel] [-c NCPU] [-m MEMSIZE] [-i IMAGE] [-b BZIMAGE] [-h|--help]"
    echo "  -k, --kernel      Use custom kernel image."
    echo "  -c NCPU           Number of CPUs to use (default: 2)."
    echo "  -m MEMSIZE        Amount of memory to allocate (default: 2G)."
    echo "  -i IMAGE          Disk image to use (default: ubuntu.img)."
    echo "  -b BZIMAGE        Path to the kernel image (bzImage)."
    echo "  -h, --help        Display this help message."
}

# Parse command-line options
while [[ $# -gt 0 ]]; do
    case $1 in
        -k|--kernel)
            USE_CUSTOM_KERNEL=true
            shift
            ;;
        -c)
            NCPU="$2"
            shift 2
            ;;
        -m)
            MEMSIZE="$2"
            shift 2
            ;;
        -i)
            IMG="$2"
            shift 2
            ;;
        -b)
            BZIMAGE="$2"
            shift 2
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            usage
            exit 1
            ;;
    esac
done

# Check if the disk image file exists
if [ ! -f "$IMG" ]; then
    echo "Error: Disk image file '$IMG' not found."
    exit 1
fi

# Check if the kernel image exists when using a custom kernel
if [ "$USE_CUSTOM_KERNEL" = true ] && [ ! -f "$BZIMAGE" ]; then
    echo "Error: Kernel image file '$BZIMAGE' not found."
    exit 1
fi

QEMU_ARGS="-enable-kvm -s -nographic \
    -smp ${NCPU} -m ${MEMSIZE} \
    -nic user,host=10.0.2.10,hostfwd=tcp:127.0.0.1:2200-:22 \
    -net nic,model=e1000 \
    -drive file=${IMG},format=qcow2"

# Append kernel-related arguments if a custom kernel is being used
if [ "$USE_CUSTOM_KERNEL" = true ]; then
    echo "Running with custom kernel: $BZIMAGE"
    echo "$QEMU_BIN $QEMU_ARGS -kernel $BZIMAGE -append \"$CMDLINE\""
    $QEMU_BIN $QEMU_ARGS -kernel "$BZIMAGE" -append "$CMDLINE"
else
    echo "Running with default kernel from disk image."
    echo "$QEMU_BIN $QEMU_ARGS"
    $QEMU_BIN $QEMU_ARGS
fi