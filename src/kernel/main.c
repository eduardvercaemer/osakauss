#include <kernel/log.h>
#include <kernel/speaker.h>
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
#include <kernel/input.h>

const u32 magic = 0xdeadbeef;

static void
init(void)
{
	
	require_log(LOG_BOTH);
	
	logf("   ...:::   osakauss v0.0.0  :::...\n\n");
	
	// todo, move all these into descriptors_init or similar
	gdt_init();
	idt_init();
	isr_init();
	irq_init();
	enable_interrupts();
	timer_init(100);
	
	/*
	 * First step is to initialize the physmem bookkeeper. This uses a small
	 * bitmap to keep track of physical frame usage.
	 *
	 * Gives access to:
	 * - physmem_alloc
	 * - physmem_free
	 */
	physmem_init();
	/*
	 * This allocates a paging directorry for kernel memory, and then proceeds
	 * to identity map the early kernel (until the end symbol from the linker).
	 *
	 * The directory itself is mapped at 0x4040000. The paging tables reside in
	 * the table starting at 0x40000000, so table 0 is 0x40000000, table 1 is
	 * 0x4001000, etc.
	 *
	 * We then enable paging.
	 */
	paging_init();

	heap_init();    // after this, and _only_ after this, we can use the usual kmalloc etc
	syscall_init(); // broken
	require_input(INPUT_BOTH);
}

void main() {
	trace = true;
	init();
	trace = true;
	tracef("init successful !\n", NULL);
	
	tracef("testing physmem allocation\n", NULL);
	u32* paddr1 = (u32*) physmem_alloc();
	u32* paddr2 = (u32*) physmem_alloc();
	u32* paddr3 = (u32*) physmem_alloc();
	tracef("> paddr1 [%p]\n", paddr1);
	tracef("> paddr2 [%p]\n", paddr2);
	tracef("> paddr3 [%p]\n", paddr3);
	physmem_free((u32) paddr2);
	physmem_free((u32) paddr1);
	paddr2 = (u32*) physmem_alloc();
	paddr1 = (u32*) physmem_alloc();
	tracef("> paddr1 [%p]\n", paddr1);
	tracef("> paddr2 [%p]\n", paddr2);
	tracef("> paddr3 [%p]\n", paddr3);
		
	tracef("testing heap\n", NULL);
	u32 *heap1 = (u32 *) kmalloc(16);
	u32 *heap2 = (u32 *) kmalloc(16);
	tracef("> heap1 [%p]\n", heap1);
	tracef("> heap2 [%p]\n", heap2);
	*heap1 = 0xdeadbeef;
	*heap2 = *heap1;
	tracef("> read and write %x\n", *heap2);
	
	tracef("testing timers\n", NULL);
	timer_wait(100);
	tracef("> waited 100 ticks\n", NULL);
	
	tracef("testing audio\n", NULL);
	beep(1000,10);
	
	tracef("testing syscalls\n",NULL);
	tracef("> syscall output: ",NULL);
	syscall_log_best_number_ever();

	tracef("Press any key\n",NULL);
	char *key;
	input_read(&key,1);
	logf("\n");	          





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