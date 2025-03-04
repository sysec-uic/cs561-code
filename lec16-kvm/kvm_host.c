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

int main(void)
{
    // Open the KVM device and create a VM using ioctl
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

    // Allocate and map memory for the VM (4KB)
    void *mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("Failed to mmap memory");
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    // Copy guest code into VM memory
    if (guest_bin_len > 0x1000) {
        fprintf(stderr, "Guest code too large for memory region\n");
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }
    memcpy(mem, guest_bin, guest_bin_len);

    // Load the simple guest code and set up the guest memory
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

    // Create a vCPU
    int vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, 0);
    if (vcpu_fd < 0) {
        perror("Failed to create vCPU");
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    // Set up vCPU registers
    struct kvm_regs regs;
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

    if (ioctl(vcpu_fd, KVM_GET_REGS, &regs) < 0) {
        perror("Failed to get registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }
    regs.rip = 0x0;
    regs.rflags = 0x2; // Minimum flags required
    if (ioctl(vcpu_fd, KVM_SET_REGS, &regs) < 0) {
        perror("Failed to set registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    // Print initial register state
    printf("Initial registers:\n");
    if (ioctl(vcpu_fd, KVM_GET_REGS, &regs) < 0) {
        perror("Failed to get initial registers");
    } else {
        printf("rax: %llu, rcx: %llu, rip: %llu\n", regs.rax, regs.rcx, regs.rip);
    }

    // Run the vCPU
    struct kvm_run *run;
    int run_size = ioctl(kvm_fd, KVM_GET_VCPU_MMAP_SIZE, 0);
    run = mmap(NULL, run_size, PROT_READ | PROT_WRITE, MAP_SHARED, vcpu_fd, 0);
    if (run == MAP_FAILED) {
        perror("Failed to mmap vCPU run structure");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    printf("Running VM... (should increment ax to 10)\n");
    if (ioctl(vcpu_fd, KVM_RUN, 0) < 0) {
        perror("KVM_RUN failed");
    } else {
        if (run->exit_reason == KVM_EXIT_HLT) {
            printf("VM exited with HLT instruction\n");
            if (ioctl(vcpu_fd, KVM_GET_REGS, &regs) < 0) {
                perror("Failed to get registers after exit");
            } else {
                printf("Final registers:\n");
                printf("rax: %llu, rcx: %llu, rip: %llu\n",
                       regs.rax, regs.rcx, regs.rip);
            }
        } else {
            printf("VM exited with unexpected reason: %d\n", run->exit_reason);
        }
    }

    // Cleanup
    munmap(run, run_size);
    close(vcpu_fd);
    munmap(mem, 0x1000);
    close(vm_fd);
    close(kvm_fd);
    return 0;
}
