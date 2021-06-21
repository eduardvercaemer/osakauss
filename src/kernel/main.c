#include <stdint.h>
#include <stivale2.h>
#include <x86.h>
#include <types.h>
#include <kernel/drivers/console.h>
#include <kernel/GDT.h>
#include <kernel/IDT.h>
#include <kernel/ISR.h>
#include <kernel/log.h>
#include <kernel/input.h>
u8 stack[4096];

GDTDescriptor gdtDescriptor = {};


static bool gdtInit(){
    gdtDescriptor.Size = sizeof(GDT) - 1;
    gdtDescriptor.Offset = (u64)&DefaultGDT;
    LoadGDT(&gdtDescriptor);
    return true;
}


bool init(struct stivale2_struct *stivale2_struct){
    require_log(LOG_BOTH);

    logf("   ...:::   osakauss v0.0.0  :::...\n\n");

    gdtInit();
    IDTInit();
    ISRInit();
    enable_interrupts();    
    require_input(INPUT_BOTH);
    
    return true;
}

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {

    .entry_point = 0,

    .stack = (uintptr_t)stack + sizeof(stack),

    .flags = (1 << 1),

    //.tags = (uintptr_t)&framebuffer_hdr_tag
};

void *stivale2_get_tag(struct stivale2_struct *stivale2_struct, uint64_t id) {
    struct stivale2_tag *current_tag = (void *)stivale2_struct->tags;
    for (;;) {

        if (current_tag == NULL) {
            return NULL;
        }

        if (current_tag->identifier == id) {
            return current_tag;
        }


        current_tag = (void *)current_tag->next;
    }
}

void main(struct stivale2_struct *stivale2_struct) {
    init(stivale2_struct);
    char buf[3] = {0};
    logf("input > ");
    SetBarrier();
    input_read(&buf, 2);
    logf("\nyou input: %s\n", buf);
    for (;;){
        asm volatile("hlt");
    }
}


