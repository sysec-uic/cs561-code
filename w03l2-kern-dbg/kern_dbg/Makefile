obj-m := lkp.o

CONFIG_MODULE_SIG=n
KDIR := /lib/modules/$(shell uname -r)/build
PWD	:= $(shell pwd)

all: lkp.c
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean
