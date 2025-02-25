MOD_NAME=sync_mutex
obj-m += $(MOD_NAME).o

INSTALL_TARGET=root@192.168.53.89:~
#KDIR="../linux-4.10.4"
KDIR="/lib/modules/$(shell uname -r)/build"

all:
	make -C $(KDIR) M=$(PWD) modules

# To avoid the password prompt each time you do make install, just
# install the host ssh key into the qemu VM by doing (on the host):
# ssh-keygen
# ssh-copy-id <ip address of the qemu vm>
install: all
	scp $(MOD_NAME).ko $(INSTALL_TARGET)

clean:
	make -C $(KDIR) M=$(PWD) clean

