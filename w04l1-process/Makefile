CFLAGS=-Wall -Werror

all: process

process: process.c
	gcc $(CFLAGS) $^ -o $@

run: process
	reset && strace -f ./process > /dev/null

clean:
	rm -rf *.o process
