# Makefile
obj-m += vfs_inspector.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

install: all
	sudo insmod vfs_inspector.ko
	sudo dmesg -c

uninstall:
	sudo rmmod vfs_inspector