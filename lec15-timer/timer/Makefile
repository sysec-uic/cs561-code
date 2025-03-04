obj-m += timer_test.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

test: all
	sudo insmod timer_test.ko
	@sleep 3
	sudo rmmod timer_test &> /dev/null || true

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
