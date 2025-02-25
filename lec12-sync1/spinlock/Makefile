MOD_NAME = sync_spinlock
obj-m += $(MOD_NAME).o

KDIR="/lib/modules/$(shell uname -r)/build"

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

