#include <kernel/log.h>
#include <kernel/heap.h>
#include <x86.h>
#include "phys.h"
#include "frames.h"

// 8M of phys mem
#define FRAME_COUNT 0x800

// begin address of the phys. memory we manage
static u32 physmem_base;
// the frame bitmap for the physical memory
static struct frame_bitmap *frames;
// are we ready ?
bool physmem_ready = false;

/* linker */
extern u32 end;
u32 placement_address = (u32) &end;

/* exports */

extern void
physmem_init(void)
{
	tracef("setting up %d frames\n", FRAME_COUNT);
	
	// create the frame map
	frames = frame_new(FRAME_COUNT);
	//tracef("> frame map at [%p]\n", frames->frames);
	//tracef("> frame count %d\n", frames->nframes);
}

extern void
physmem_start(void)
{
	// align placement address
	if (placement_address & 0xfff) placement_address += 0x1000;
	placement_address &= 0xfffff000;
	physmem_base = placement_address;
	physmem_ready = true;
	
	tracef("physmem at [%p]\n", physmem_base);
}

extern u32
physmem_alloc(void)
{
	u32 frame = frame_first(frames);
	if (frame == FRAME_COUNT) {
		tracef("out of frames !\n", NULL);
		hang();
	}
	frame_set(frames, frame);
	u32 paddr = physmem_base + frame * 0x1000;
	//tracef("frame [%d] paddr [%p]\n", frame, paddr);
	return paddr;
}

extern void
physmem_free(u32 paddr)
{
	//tracef("freeing paddr [%p]\n", paddr);
	u32 frame = (paddr - physmem_base) / 0x1000;
	if (!frame_test(frames, frame)) {
		//tracef("> double free !", NULL);
		return;
	}
	frame_clear(frames, frame);
	//tracef("> done\n", NULL);
}