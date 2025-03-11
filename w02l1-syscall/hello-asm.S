.data

msg:
    .ascii "Hello, world!\n"
    len = . - msg

.text
    .global _start

_start:
    mov  $1, %rax     # syscall id: write
    mov  $1, %rdi     # 1st arg: fd (standard output)
    mov  $msg, %rsi   # 2nd arg: msg
    mov  $len, %rdx   # 3rd arg: length of msg
    syscall           # switch from user space to kernel space

    mov  $60, %rax    # syscall id: exit
    xor  %rdi, %rdi   # 1st arg: 0
    syscall           # switch from user space to kernel space
