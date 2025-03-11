all: hello-syscall hello-asm

hello-syscall: hello-syscall.c
	gcc -o hello-syscall hello-syscall.c

hello-asm: hello-asm.S
	gcc -c hello-asm.S
	ld -o hello-asm hello-asm.o

clean:
	rm -f hello-syscall hello-asm hello-asm.o

.PHONY: all hello-syscall hello-asm clean
