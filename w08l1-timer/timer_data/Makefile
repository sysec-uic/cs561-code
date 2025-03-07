obj-m += timer_data.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

install: all
	sudo insmod timer_data.ko

remove:
	sudo rmmod timer_data &> /dev/null || true

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
