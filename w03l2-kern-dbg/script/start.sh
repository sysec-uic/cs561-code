#!/bin/sh
export PATH=$PATH:/sbin:/usr/sbin
mount -t proc proc /proc
mount -t sysfs none /sys
ip addr add 127.0.0.1 dev lo
ip link set dev lo up
