CFLAGS=-Wall -Werror

all: busy-rt sched

busy-rt: busy-rt.c
	gcc $(CFLAGS) $^ -o $@

sched: sched.c
	gcc $(CFLAGS) $^ -o $@

run: sched
	sudo ./sched

clean:
	rm -rf *.o busy-rt sched
