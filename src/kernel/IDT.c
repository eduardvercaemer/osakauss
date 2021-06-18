#include <kernel/IDT.h>
#include <x86.h>



struct IDT _idt[256] = {0};
struct IDTPtr idtp = {0};


/* define all the isr's */

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


extern void empty_stub();

static void IDTFlush(struct IDTPtr *);
static void IDTLoad();
static void PicRemap(void);
static void FillPreIdt();


#define SELECTOR 0x8


extern bool
ISRInit(){
    IDTSetGate( 0, (u64)isr0 , 0x8E);
   IDTSetGate( 1, (u64)isr1 , 0x8E);
   IDTSetGate( 2, (u64)isr2 , 0x8E);
   IDTSetGate( 3, (u64)isr3 , 0x8E);
   IDTSetGate( 4, (u64)isr4 , 0x8E);
   IDTSetGate( 5, (u64)isr5 , 0x8E);
   IDTSetGate( 6, (u64)isr6 , 0x8E);
   IDTSetGate( 7, (u64)isr7 , 0x8E);
   IDTSetGate( 8, (u64)isr8 , 0x8E);
   IDTSetGate( 9, (u64)isr9 , 0x8E);
   IDTSetGate( 10, (u64)isr10 , 0x8E);
   IDTSetGate( 11, (u64)isr11 , 0x8E);
   IDTSetGate( 12, (u64)isr12 , 0x8E);
   IDTSetGate( 13, (u64)isr13 , 0x8E);
   IDTSetGate( 14, (u64)isr14 , 0x8E);
   IDTSetGate( 15, (u64)isr15 , 0x8E);
   IDTSetGate( 16, (u64)isr16 , 0x8E);
   IDTSetGate( 16, (u64)isr16 , 0x8E);
   IDTSetGate( 16, (u64)isr16 , 0x8E);
   IDTSetGate( 17, (u64)isr17 , 0x8E);
   IDTSetGate( 18, (u64)isr18 , 0x8E);
   IDTSetGate( 19, (u64)isr19 , 0x8E);
   IDTSetGate( 20, (u64)isr20 , 0x8E);
   IDTSetGate( 21, (u64)isr21 , 0x8E);
   IDTSetGate( 22, (u64)isr22 , 0x8E);
   IDTSetGate( 23, (u64)isr23 , 0x8E);
   IDTSetGate( 24, (u64)isr24 , 0x8E);
   IDTSetGate( 25, (u64)isr25 , 0x8E);
   IDTSetGate( 26, (u64)isr26 , 0x8E);
   IDTSetGate( 27, (u64)isr27 , 0x8E);
   IDTSetGate( 28, (u64)isr28 , 0x8E);
   IDTSetGate( 29, (u64)isr29 , 0x8E);
   IDTSetGate( 30, (u64)isr30 , 0x8E);
   IDTSetGate( 31, (u64)isr31 , 0x8E);

   IDTSetGate( 32, (u64)irq0 , 0x8E);
   IDTSetGate( 33, (u64)irq1 , 0x8E);
   IDTSetGate( 34, (u64)irq2 , 0x8E);
   IDTSetGate( 35, (u64)irq3 , 0x8E);
   IDTSetGate( 36, (u64)irq4 , 0x8E);
   IDTSetGate( 37, (u64)irq5 , 0x8E);
   IDTSetGate( 38, (u64)irq6 , 0x8E);
   IDTSetGate( 39, (u64)irq7 , 0x8E);
   IDTSetGate( 40, (u64)irq8 , 0x8E);
   IDTSetGate( 41, (u64)irq9 , 0x8E);
   IDTSetGate( 42, (u64)irq10 , 0x8E);
   IDTSetGate( 43, (u64)irq11 , 0x8E);
   IDTSetGate( 44, (u64)irq12 , 0x8E);
   IDTSetGate( 45, (u64)irq13 , 0x8E);
   IDTSetGate( 46, (u64)irq14 , 0x8E);
   IDTSetGate( 47, (u64)irq15 , 0x8E);

    PicRemap();

    IDTFlush(&idtp);
    return true;
}

extern void
IDTSetGate(u32 num, u64 base, u8 type_attr)
{   
    _idt[num].reserved = 0;
    _idt[num].offset_low = (u16)((base));
    _idt[num].offset_mid = (u16)((base) >> 16);
    _idt[num].offset_high = (u32)((base) >> 32);
    _idt[num].ist = 0;
    _idt[num].selector = SELECTOR   ;
    _idt[num].types_attr = type_attr;
}



bool 
IDTInit(){
    idtp.limit = (u16)(sizeof(struct IDT) * 256) - 1;
    idtp.base = (u64)&_idt;
    FillPreIdt();
    IDTLoad();
    return true;
}

static void 
IDTLoad(){
    asm volatile("lidt %0" :: "m"( idtp ) : "memory");
}
static void
IDTFlush(struct IDTPtr *_idtp){
    asm volatile("lidt %0" :: "m"( idtp ) : "memory");
}

static void
PicRemap(void)
{
    outb(0x20, 0x11);
    IOWait();
    outb(0xA0, 0x11);
    IOWait();
    outb(0x21, 0x20);
    IOWait();
    outb(0xA1, 0x28);
    IOWait();
    outb(0x21, 0x04);
    IOWait();
    outb(0xA1, 0x02);
    IOWait();
    outb(0x21, 0x01);
    IOWait();
    outb(0xA1, 0x01);
    IOWait();
    // Disable all interrupts from PIC for now
    /*
    outb(0x21, 0xff);
    IOWait();
    outb(0xA1, 0xff);
    IOWait();
    */
   
    // Enables all interrupts from PIC for now
    outb(0x21, 0x0);
    IOWait();
    outb(0xA1, 0x0);
    IOWait();
}


static void 
FillPreIdt(){
    for (u32 i = 0; i < 256; i++){
        IDTSetGate( i, empty_stub, 0x8E);
    }
}