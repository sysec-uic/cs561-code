#!/bin/bash
sudo debootstrap --no-check-gpg --arch=amd64 \
     --include=kmod,net-tools,iproute2,iputils-ping \
     --variant=minbase \
     sid linux-chroot
sudo cp start.sh linux-chroot/
echo 'set auto-load safe-path ~/' > ~/.gdbinit
