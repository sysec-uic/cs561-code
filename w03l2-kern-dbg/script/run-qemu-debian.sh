#!/bin/bash
QEMU_BIN=qemu-system-x86_64
ROOT_DIR=${PWD}/linux-chroot
# KNL_SRC=/lib/modules/$(shell uname -r)/build
KNL_SRC=~/workspace/research/linux # TODO: set your kernel source directory
BZIMAGE=${KNL_SRC}/arch/x86/boot/bzImage
NCPU=2
MEMSIZE=2G

sudo ${QEMU_BIN} \
     -s \
     -nographic \
     -kernel ${BZIMAGE} \
     -fsdev local,id=root,path=${ROOT_DIR},security_model=passthrough \
     -device virtio-9p-pci,fsdev=root,mount_tag=/dev/root \
     -append 'nokaslr root=/dev/root rw rootfstype=9p rootflags=trans=virtio console=ttyS0 init=/bin/bash' \
     -smp cpus=${NCPU} \
     -device e1000,netdev=net0 \
     -netdev user,id=net0,hostfwd=tcp::2222-:22 \
     -m ${MEMSIZE}

###############################################################################
# Additional QEMU options
###############################################################################
#
# -S
#  . pause on the first kernel instruction waiting for 
#    a GDB client connection to continue. 
#  . useful for debugging boot code
#
# -enable-kvm
#  . use kvm for QEMU virtualization
#  . only available when you run QEMU on Linux
#
# -cpu host
#  . do not emulate CPU
#  . only available when you run QEMU on Linux
#
###############################################################################

