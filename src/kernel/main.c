/*
EFI BOOTABLE : YES
LEGACY BOOTABLE : YES
TOTAL RAM : 4096 MB
GRAPHICS : YES
FONT : CUSTOM MADE
DRIVE : 40 MB
IDT : YES
GDT : YES
TSS : WORKING ON IT
MMAP : WORKING ON IT
*/

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
#include <kernel/drivers/graphics/framebuffer.h>
#include <kernel/mm/phys.h>
u8 stack[16000];


static struct stivale2_header_tag_framebuffer framebuffer_header_tag = 
{
    .tag = {
        .identifier = STIVALE2_HEADER_TAG_FRAMEBUFFER_ID,
        .next = 0 
    },
    .framebuffer_width  = 800,
    .framebuffer_height = 600,
    .framebuffer_bpp    = 32
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

bool init(struct stivale2_struct *stivale2_struct){

    struct stivale2_struct_tag_framebuffer *framebuffer_tag;

    framebuffer_tag = stivale2_get_tag(stivale2_struct, STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID);

    framebufferInit(framebuffer_tag->framebuffer_addr,framebuffer_tag->framebuffer_addr, framebuffer_tag->framebuffer_width,framebuffer_tag->framebuffer_height,framebuffer_tag->framebuffer_bpp,framebuffer_tag->framebuffer_pitch);
    
    require_log(LOG_BOTH);

    trace = true;

    logf("   ...:::   osakauss v0.0.0  :::...\n\n");
    gdtInit();
    IDTInit();
    ISRInit();
    enable_interrupts();    
    require_input(INPUT_BOTH);
    physmem_init();
    
    return true;
}

__attribute__((section(".stivale2hdr"), used))
static struct stivale2_header stivale_hdr = {

    .entry_point = 0,

    .stack = (uintptr_t)stack + sizeof(stack),

    .flags = (1 << 1),

    .tags = (uintptr_t)&framebuffer_header_tag
};






void main(struct stivale2_struct *stivale2_struct) {
    init(stivale2_struct);

    
    
    char * hello = physmem_alloc(200);
    physmem_free(hello);
    

    char * buf = physmem_alloc(0x10000);

    memset(buf,0,10000);

    logf("\ninput > ");

    input_readln(buf);

    logf("\nyour input = %s\n",buf);
    physmem_free(buf);
  
    for (;;){
        asm volatile("hlt");
    }
}
