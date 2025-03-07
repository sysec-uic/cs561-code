; guest.asm
BITS 16
section .text
start:
    mov si, message    ; SI points to the string
print_loop:
    lodsb              ; Load byte from [SI] into AL, increment SI
    cmp al, 0          ; Check for null terminator
    je done            ; If zero, jump to done
    out 0xe9, al       ; Output AL to port 0xE9
    jmp print_loop     ; Repeat for next character
done:
    hlt                ; Halt the VM

section .data
message:
    db "Hello", 0      ; Null-terminated string
