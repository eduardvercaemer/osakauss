#include <kernel/log.h>
#include <kernel/paging.h>
#include <kernel/phys.h>
#include <kernel/GDT.h>
#include <kernel/IDT.h>
#include <kernel/ISR.h>
#include <kernel/IRQ.h>

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
	
	paging_init();  // this identity-maps the early kernel (i.e. 0 -> physmem_base will be mapped)
	physmem_init();
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
	
	
	tracef("testing page faults\n", NULL);
	u32 * ptr = (u32*)0xa0000000;
	u32 tmp;
	logf("reading from [%p]\n", 0xa0000000);
	tmp = *ptr;
	logf("value read: %d\n", tmp);
	logf("writing to [%p]\n", 0xa0000000);
	*ptr = 0xdeadbeef;
	
	// trigger breakpoint
	asm volatile("int $3");
 
	//loops forever
    for (;;);
}