[biTs 64]
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31 
global idt_flush

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




isr0:
  cli                 
  push  0         
  push  0         
  jmp isr_common_stub
isr1:
  cli                 
  push  0         
  push  1        
  jmp isr_common_stub 
isr2:
  cli                 
  push  0         
  push  2         
  jmp isr_common_stub 
isr3:
  cli                 
  push  0         
  push  3        
  jmp isr_common_stub 
isr4:
  cli                 
  push  0         
  push  4         
  jmp isr_common_stub 
isr5:
  cli                
  push  0         
  push  5         
  jmp isr_common_stub 
isr6:
  cli                 
  push  0         
  push  6         
  jmp isr_common_stub 
isr7:
  cli                 
  push  0         
  push  7         
  jmp isr_common_stub 
isr8:
  cli                
  push  8         
  jmp isr_common_stub 
isr9:
  cli                 
  push  0         
  push  9         
  jmp isr_common_stub
isr10:
  cli                 
  push  10         
  jmp isr_common_stub
 isr11:
  cli                 
  push  11        
  jmp isr_common_stub
isr12:
  cli                 
  push  12        
  jmp isr_common_stub 
isr13:
  cli                 
  push  13        
  jmp isr_common_stub 
isr14:
  cli                 
  push  14         
  jmp isr_common_stub 
isr15:
  cli                 
  push  0         
  push  15         
  jmp isr_common_stub 
isr16:
  cli                 
  push  0         
  push  16         
  jmp isr_common_stub 
isr17:
  cli                 
  push  0         
  push  17         
  jmp isr_common_stub 
isr18:
  cli                 
  push  0         
  push  18         
  jmp isr_common_stub 
isr19:
  cli                 
  push  0         
  push  19         
  jmp isr_common_stub 
isr20:
  cli                 
  push  0         
  push  20         
  jmp isr_common_stub 
isr21:
  cli                 
  push  0         
  push  21         
  jmp isr_common_stub 
isr22:
  cli                 
  push  0         
  push  22         
  jmp isr_common_stub 
isr23:
  cli                 
  push  0         
  push  23        
  jmp isr_common_stub 
isr24:
  cli                 
  push  0         
  push  24         
  jmp isr_common_stub 
isr25:
  cli                 
  push  0         
  push  25         
  jmp isr_common_stub
isr26:
  cli                 
  push  0         
  push  26         
  jmp isr_common_stub 
isr27:
  cli                 
  push  0         
  push  27        
  jmp isr_common_stub 
isr28:
  cli                 
  push  0        
  push  28         
  jmp isr_common_stub 
isr29:
  cli                 
  push  0         
  push  29         
  jmp isr_common_stub 
isr30:
  cli                 
  push  0         
  push  30         
  jmp isr_common_stub 
isr31:
  cli                 
  push  0         
  push  31         
  jmp isr_common_stub

extern fault_handler

isr_common_stub:
    cld ; clears all the direction Flag
    pusha64
    mov rdi, rsp
    call fault_handler
    popa64
    add rsp, 16
    iretq