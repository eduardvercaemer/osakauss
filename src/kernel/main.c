#include <kernel/log.h>
#include <kernel/console.h>
#include <kernel/serial.h>
#include <kernel/paging.h>
#include <kernel/phys.h>
#include <kernel/heap.h>
#include <kernel/GDT.h>
#include <kernel/IDT.h>
#include <kernel/ISR.h>
#include <kernel/IRQ.h>
#include <kernel/timer.h>
#include <x86.h>
#include <kernel/syscall.h>

const u32 magic = 0xdeadbeef;

static void
init(void)
{
	require_log(LOG_SERIAL);
	logf("   ...:::   asokauss v0.0.0  :::...\n\n");
	
	// todo, move all these into descriptors_init or similar
	gdt_init();
	idt_init();
	isr_init();
	irq_init();
	enable_interrupts();
	timer_init(100);
	
	paging_init();  // this identity-maps the early kernel (i.e. 0 -> physmem_base will be mapped)
	physmem_init(); // at this point, we can use the physmem allocator (i.e. alloc, free)
	heap_init();    // after this, and _only_ after this, we can use the usual kmalloc etc
	syscall_init(); // broken
}

void main() {
	trace = false;
	init();
	trace = true;
	tracef("init successful !\n", NULL);
	
	tracef("testing physmem allocation\n", NULL);
	u32* paddr1 = physmem_alloc();
	u32* paddr2 = physmem_alloc();
	u32* paddr3 = physmem_alloc();
	tracef("> paddr1 [%p]\n", paddr1);
	tracef("> paddr2 [%p]\n", paddr2);
	tracef("> paddr3 [%p]\n", paddr3);
	physmem_free(paddr2);
	physmem_free(paddr1);
	paddr2 = physmem_alloc();
	paddr1 = physmem_alloc();
	tracef("> paddr1 [%p]\n", paddr1);
	tracef("> paddr2 [%p]\n", paddr2);
	tracef("> paddr3 [%p]\n", paddr3);
	
	tracef("testing heap table\n", NULL);
	u32 heap_test = *((u32*)0x80000000);
	tracef("> test %x\n", heap_test);
	
	tracef("testing heap\n", NULL);
	u32 *heap1 = kmalloc(16);
	u32 *heap2 = kmalloc(16);
	tracef("> heap1 [%p]\n", heap1);
	tracef("> heap2 [%p]\n", heap2);
	*heap1 = 0xdeadbeef;
	*heap2 = *heap1;
	tracef("> read and write %x\n", *heap2);
	
	tracef("testing timers\n", NULL);
	timer_wait(100);
	tracef("> waited 100 ticks\n", NULL);
	
	tracef("testing audio\n", NULL);
	beep(100, 10);
	
	tracef("testing page faults\n", NULL);
	u32 * ptr = (u32*)0xa0000000;
	u32 tmp;
	logf("reading from [%p]\n", 0xa0000000);
	tmp = *ptr;
	logf("value read: %d\n", tmp);
	logf("writing to [%p]\n", 0xa0000000);
	*ptr = 0xdeadbeef;
	
	// trigger breakpoint
	asm volatile("int $0x03");
 
	//loops forever
    for (;;);
}