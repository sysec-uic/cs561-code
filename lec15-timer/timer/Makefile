obj-m += timer_test.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

test: all
	sudo rmmod timer_test.ko &> /dev/null || true
	sudo insmod timer_test.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
