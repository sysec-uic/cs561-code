// kvm_host.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <sys/mman.h>
#include <string.h>
#include "guest_bin.h" // Include the guest binary

int main(void) {
    int kvm_fd = open("/dev/kvm", O_RDWR);
    if (kvm_fd < 0) {
        perror("Failed to open /dev/kvm");
        return 1;
    }

    int api_ver = ioctl(kvm_fd, KVM_GET_API_VERSION, 0);
    if (api_ver != 12) {
        fprintf(stderr, "KVM API version mismatch: %d\n", api_ver);
        close(kvm_fd);
        return 1;
    }

    int vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, 0);
    if (vm_fd < 0) {
        perror("Failed to create VM");
        close(kvm_fd);
        return 1;
    }

    void *mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("Failed to mmap memory");
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    if (guest_bin_len > 0x1000) {
        fprintf(stderr, "Guest code too large for memory region\n");
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }
    memcpy(mem, guest_bin, guest_bin_len);

    struct kvm_userspace_memory_region region = {
        .slot = 0,
        .guest_phys_addr = 0x0,
        .memory_size = 0x1000,
        .userspace_addr = (unsigned long)mem,
    };
    if (ioctl(vm_fd, KVM_SET_USER_MEMORY_REGION, &region) < 0) {
        perror("Failed to set memory region");
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    int vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, 0);
    if (vcpu_fd < 0) {
        perror("Failed to create vCPU");
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    struct kvm_sregs sregs;
    if (ioctl(vcpu_fd, KVM_GET_SREGS, &sregs) < 0) {
        perror("Failed to get special registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }
    sregs.cs.base = 0;
    sregs.cs.selector = 0;
    if (ioctl(vcpu_fd, KVM_SET_SREGS, &sregs) < 0) {
        perror("Failed to set special registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    struct kvm_regs regs;
    if (ioctl(vcpu_fd, KVM_GET_REGS, &regs) < 0) {
        perror("Failed to get registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }
    regs.rip = 0x0;
    regs.rflags = 0x2;
    if (ioctl(vcpu_fd, KVM_SET_REGS, &regs) < 0) {
        perror("Failed to set registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    int run_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
    struct kvm_run *run = mmap(NULL, run_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0);
    if (run == MAP_FAILED) {
        perror("Failed to mmap vCPU run structure");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    printf("Running VM... (should print 'Hello')\n");
    while (1) {
        if (ioctl(vcpu_fd, KVM_RUN, 0) < 0) {
            perror("KVM_RUN failed");
            break;
        }

        switch (run->exit_reason) {
            case KVM_EXIT_HLT:
                printf("\nVM exited with HLT instruction\n");
                goto done; // Exit the loop
            case KVM_EXIT_IO:
                if (run->io.direction == KVM_EXIT_IO_OUT && run->io.port == 0xe9) {
                    char c = *(char *)((char *)run + run->io.data_offset);
                    putchar(c); // Print the character
                    fflush(stdout);
                } else {
                    printf("Unexpected IO exit: direction=%d, port=%d\n",
                           run->io.direction, run->io.port);
                }
                break;
            default:
                printf("Unexpected exit reason: %d\n", run->exit_reason);
                goto done;
        }
    }

done:
    munmap(run, run_size);
    close(vcpu_fd);
    munmap(mem, 0x1000);
    close(vm_fd);
    close(kvm_fd);
    return 0;
}