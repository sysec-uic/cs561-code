#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/kvm.h>
#include <sys/mman.h>
#include <string.h>

int main(void) {
    // Step 1: Open the KVM device
    int kvm_fd = open("/dev/kvm", O_RDWR);
    if (kvm_fd < 0) {
        perror("Failed to open /dev/kvm");
        return 1;
    }

    // Step 2: Check KVM API version
    int api_ver = ioctl(kvm_fd, KVM_GET_API_VERSION, 0);
    if (api_ver != 12) { // KVM_API_VERSION is typically 12
        fprintf(stderr, "KVM API version mismatch: %d\n", api_ver);
        close(kvm_fd);
        return 1;
    }

    // Step 3: Create a VM
    int vm_fd = ioctl(kvm_fd, KVM_CREATE_VM, 0);
    if (vm_fd < 0) {
        perror("Failed to create VM");
        close(kvm_fd);
        return 1;
    }

    // Step 4: Allocate and map memory for the VM (1MB)
    void *mem = mmap(NULL, 0x1000, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (mem == MAP_FAILED) {
        perror("Failed to mmap memory");
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    // Step 5: Load simple guest code (x86 "hlt" instruction: 0xF4)
    unsigned char code[] = {0xF4}; // Halt instruction
    memcpy(mem, code, sizeof(code));

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

    // Step 6: Create a vCPU
    int vcpu_fd = ioctl(vm_fd, KVM_CREATE_VCPU, 0);
    if (vcpu_fd < 0) {
        perror("Failed to create vCPU");
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    // Step 7: Set up vCPU registers
    struct kvm_regs regs;
    struct kvm_sregs sregs;

    // Get special registers
    if (ioctl(vcpu_fd, KVM_GET_SREGS, &sregs) < 0) {
        perror("Failed to get special registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    // Minimal segment setup (flat memory model)
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

    // Set general-purpose registers (RIP = 0 to start at our code)
    if (ioctl(vcpu_fd, KVM_GET_REGS, &regs) < 0) {
        perror("Failed to get registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }
    regs.rip = 0x0; // Instruction pointer starts at guest address 0
    regs.rflags = 0x2; // Minimal flags
    if (ioctl(vcpu_fd, KVM_SET_REGS, &regs) < 0) {
        perror("Failed to set registers");
        close(vcpu_fd);
        munmap(mem, 0x1000);
        close(vm_fd);
        close(kvm_fd);
        return 1;
    }

    // Step 8: Run the vCPU
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

    printf("Running VM...\n");
    if (ioctl(vcpu_fd, KVM_RUN, 0) < 0) {
        perror("KVM_RUN failed");
    } else {
        switch (run->exit_reason) {
            case KVM_EXIT_HLT:
                printf("VM exited with HLT instruction\n");
                break;
            default:
                printf("VM exited with reason: %d\n", run->exit_reason);
                break;
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
