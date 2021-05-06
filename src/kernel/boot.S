bits 32

; defined in linker scripted
extern end


; Multiboot headers
section .multiboot
        align 4
        dd 0x1BADB002           
        dd 0x00                  
        dd - (0x1BADB002 + 0x00) 
        dd end

section .text

global start
extern main	        

%include "kernel/GDT.S"
%include "kernel/IDT.S"
%include "kernel/ISR.S"
%include "kernel/IRQ.S"

start:
  cli 			
  mov esp, stack_space	
  ; calls the main kernel function
  call main
  ; halts the cpu
  hlt		 	

section .bss
resb 8192		
stack_space: