#!/bin/bash
QEMU_BIN=qemu-system-x86_64
# KNL_SRC=/lib/modules/$(shell uname -r)/build
KNL_SRC=~/workspace/research/linux
BZIMAGE=${KNL_SRC}/arch/x86_64/boot/bzImage
VMIMAGE=${PWD}/ubuntu.qcow2
NCPU=2
MEMSIZE=2G

sudo ${QEMU_BIN} \
     -s \
     -nographic \
     -hda ${VMIMAGE} \
     -kernel ${BZIMAGE} \
     -append "nokaslr root=/dev/sda1 console=ttyS0" \
     -smp cpus=${NCPU} \
     -device e1000,netdev=net0 \
     -netdev user,id=net0,hostfwd=tcp::5555-:22 \
     -m ${MEMSIZE}

#     -enable-kvm \ # only available when you run QEMU on Linux
#     -cpu host \   # only available when you run QEMU on Linux

# Converting vmdk to qcow2
# ------------------------
#
# qemu-img convert -O qcow2 ubuntu-1704.vmdk ubuntu.qcow2

# Essential kernel configurations for gdb debugging
# -------------------------------------------------
#
# CONFIG_VIRTIO=y
# CONFIG_NET_9P=y
# CONFIG_NET_9P_VIRTIO=y
# CONFIG_9P_FS=y
# CONFIG_9P_FS_POSIX_ACL=y
# CONFIG_9P_FS_SECURITY=y
#
# CONFIG_DEBUG_INFO=y
# CONFIG_GDB_SCRIPTS=y
# CONFIG_FRAME_POINTER=y
#
# CONFIG_E1000=y
