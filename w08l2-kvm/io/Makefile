# Variables for tools and flags
CC = gcc
CFLAGS = -Wall -g
NASM = nasm
NASMFLAGS = -f bin

# Targets
all: kvm_host

# VM has 6 instructions that increase the AX register
kvm_host: kvm_host.c guest_bin.h
	$(CC) $(CFLAGS) -o $@ $<

guest_bin.h: guest.bin
	xxd -i $< > $@

guest.bin: guest.asm
	$(NASM) $(NASMFLAGS) -o $@ $<

disasm_guest: guest.bin
	ndisasm -b 16 guest.bin

clean:
	rm -f guest.bin guest_bin.h kvm_host

.PHONY: all clean
