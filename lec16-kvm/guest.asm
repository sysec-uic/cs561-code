; guest.asm
; increase ax register to 10, then hlt
; hlt instruction: stops the CPU’s execution until an external interrupt occurs
; (e.g., a timer tick or hardware interrupt)
BITS 16
section .text
start:
    xor ax, ax      ; Clear ax to 0
    mov cx, 10      ; Set loop counter to 10
loop_start:
    inc ax          ; Increment ax
    dec cx          ; Decrement cx
    jnz loop_start  ; Jump back if cx != 0
    nop
    hlt             ; Halt the VM
