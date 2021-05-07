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
#include <kernel/multiboot.h>
#include <kernel/drivers/fs.h>
#include <kernel/drivers/initrd.h>

const u32 magic = 0xdeadbeef;



static void
init(struct multiboot *mboot_ptr)
{
	
	gdt_init();
	trace = true;
	require_log(LOG_BOTH);
	
	logf("   ...:::   osakauss v0.0.0  :::...\n\n");
	
	// todo, move all these into descriptors_init or similar
	idt_init();
	isr_init();
	irq_init();
	enable_interrupts();
	timer_init(100);

	/* can not be after any of memory management is setup because it will cause a memory fault */
	bool ramdisk = (mboot_ptr->mods_count > 0);
	u32 placement_address;
	u32 initrd_location;
	u32 initrd_end;
	u32 module_size;

	trace = true;
	if (ramdisk) {
		
		tracef("Found ramdisk\n",NULL);
		initrd_location = *((u32*)mboot_ptr->mods_addr);
		initrd_end = *(u32*)(mboot_ptr->mods_addr+4); // forbidden location in memory. can not read from

		// Don't trample our module with placement accesses, please!
		placement_address = initrd_end;
		module_size = placement_address - initrd_location;
	} else {
		tracef("No ramdisk to load\n",NULL);
	}
	
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

	/*
	 * Initiaize heap for kmalloc and kfree
	 */
	if (heap_init() < 0) {
		tracef("fatal: failed heap init\n", NULL);
		for (;;);
	}
	
	if (ramdisk) {
		tracef("> WE ARE RIGHT BEFORE INITRD\n", NULL);
		fs_root = initialise_initrd(initrd_location); // this is where it breaks. 
		tracef("Loaded ramdisk\n",NULL);
	} else {
		tracef("Did not load ramdisk",NULL);
	}

	syscall_init(); // broken
	
	require_input(INPUT_BOTH);
}


void main(struct multiboot *mboot_ptr) {	
	init(mboot_ptr);

	trace = true;
	tracef("init successful !\n", NULL);
	
	tracef("testing physmem allocation\n", NULL);
	u32 paddr1 = physmem_alloc();
	u32 paddr2 = physmem_alloc();
	u32 paddr3 = physmem_alloc();
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
	
	tracef("testing heap\n", NULL);
	u32 *heap1 = (u32 *) kmalloc(16);
	u32 *heap2 = (u32 *) kmalloc(64);
	u32 *heap3 = (u32 *) kmalloc(32);
	tracef("> heap1 [%p]\n", heap1);
	tracef("> heap2 [%p]\n", heap2);
	tracef("> heap3 [%p]\n", heap3);
	*heap1 = 0xdeadbeef;
	*heap2 = *heap1;
	*heap3 = *heap2;
	tracef("> read and write %x\n", *heap3);
	tracef("> free middle buffer of 64 bytes\n", NULL);
	kfree(heap2);
	tracef("> allocate now 2 blocks of 16 bytes\n", NULL);
	heap2 = kmalloc(16);
	heap3 = kmalloc(16);
	tracef("> heap2 [%p]\n", heap2);
	tracef("> heap3 [%p]\n", heap3);
	
	tracef("testing timers\n", NULL);
	timer_wait(100);
	tracef("> waited 100 ticks\n", NULL);
	
	tracef("testing audio\n", NULL);
	beep(1000,10);

	tracef("testing page dropping\n", NULL);
	paddr1 = physmem_alloc();
	paging_kmap(paddr1, 0x90000000);
	paging_kdrop(0x90000000);
	paging_kdrop(0x90000000);
	
	/* currently broken --
	tracef("testing syscalls\n",NULL);
	tracef("> syscall output: ",NULL);
	syscall_log_best_number_ever();
	*/

	tracef("Press any key ",NULL);
	char key[2] = {0};
	input_read(key,1);
	logf("\n");
	

	tracef("Key read: %s\n", key);

	tracef("allocating huge amounts of memory\n", NULL);
	heap1 = kmalloc(200000000);
	tracef("> result: [%p]\n", heap1);

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