obj-m += timeofday.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

test: all
	sudo rmmod timeofday.ko &> /dev/null || true
	sudo insmod timeofday.ko
	sudo rmmod timeofday.ko

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
