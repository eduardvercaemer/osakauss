global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

%macro pusha64 0

push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

%endmacro

%macro popa64 0

pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax

%endmacro

    
;32 IRQ:0
irq0:
    cli
    push byte 0    
    push byte 32
    jmp irq_common_stub

    
;33 IRQ:1
irq1:
    cli
    push byte 0    
    push byte 33
    jmp irq_common_stub

    
;34 IRQ:2
irq2:
    cli
    push byte 0    
    push byte 34
    jmp irq_common_stub

    
;35 IRQ:3
irq3:
    cli
    push byte 0    
    push byte 35
    jmp irq_common_stub

    
;36 IRQ:4
irq4:
    cli
    push byte 0    
    push byte 36
    jmp irq_common_stub

    
;37 IRQ:5
irq5:
    cli
    push byte 0    
    push byte 37
    jmp irq_common_stub

    
;38 IRQ:6
irq6:
    cli
    push byte 0    
    push byte 38
    jmp irq_common_stub

    
;39 IRQ:7
irq7:
    cli
    push byte 0    
    push byte 39
    jmp irq_common_stub

    
;40 IRQ:8
irq8:
    cli
    push byte 0    
    push byte 40
    jmp irq_common_stub

    
;41 IRQ:9
irq9:
    cli
    push byte 0    
    push byte 41
    jmp irq_common_stub

    
;42 IRQ:10
irq10:
    cli
    push byte 0    
    push byte 42
    jmp irq_common_stub

    
;43 IRQ:11
irq11:
    cli
    push byte 0    
    push byte 43
    jmp irq_common_stub

    
;44 IRQ:12
irq12:
    cli
    push byte 0    
    push byte 44
    jmp irq_common_stub

    
;45 IRQ:13
irq13:
    cli
    push byte 0    
    push byte 45
    jmp irq_common_stub

    
;46 IRQ:14
irq14:
    cli
    push byte 0    
    push byte 46
    jmp irq_common_stub

    
;47 IRQ:15
irq15:
    cli
    push byte 0    
    push byte 47
    jmp irq_common_stub

extern irq_handler

irq_common_stub:
    cld ; clears all the direction Flag
    pusha64
    mov rdi, rsp
    call irq_handler
    popa64
    add rsp, 16
    iretq